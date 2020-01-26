#include "litefs.h"

__u64 lite_fs_inode_block(struct super_block *sb, __u32 ino)
{
    struct lite_fs_vfs_super_info *vfs_sbi;

    vfs_sbi = (struct lite_fs_vfs_super_info *)sb->s_fs_info;

    return (ino / LITE_FS_PER_BLOCK_INODE) + vfs_sbi->disk_info.s_first_inode_block;
}

struct inode *lite_fs_new_inode(struct inode *dir_inode, int mode)
{
    struct super_block *sb;
    struct lite_fs_super_info *lsb;
    struct lite_fs_vfs_super_info *vfs_sbi;
    struct buffer_head *bitmap_bh;
    unsigned short bitmap_block = 0;
    struct inode *inode;
    struct lite_fs_vfs_inode_info *vfs_inode_info;
    __u32   ino = 0;
    int err;
    char *kaddr;

    LOG_INFO();

    sb = dir_inode->i_sb;
    vfs_sbi = (struct lite_fs_vfs_super_info *)sb->s_fs_info;
    lsb = &vfs_sbi->disk_info;
    inode = new_inode(sb);
    vfs_inode_info = container_of(inode, struct lite_fs_vfs_inode_info, vfs_inode);

    bitmap_block = 0;
    for ( ; bitmap_block < lsb->s_inode_bitmap_blocks; bitmap_block ++) {
        bitmap_bh = sb_bread(sb, lsb->s_first_inode_bitmap_block + bitmap_block);
        kaddr = kmap(bitmap_bh->b_page);
        ino = lite_fs_find_first_zero_bit((unsigned long *)(kaddr + bh_offset(bitmap_bh)), lsb->s_bitmap_size);
        if (ino >= lsb->s_bitmap_size) {
            kunmap(bitmap_bh->b_page);
            brelse(bitmap_bh);
            continue;
        }
        if (lite_fs_set_bit_atomic(&vfs_sbi->inode_bitmap_lock, ino, kaddr + bh_offset(bitmap_bh))) {
            kunmap(bitmap_bh->b_page);
            brelse(bitmap_bh);
            continue;
        }
        kunmap(bitmap_bh->b_page);
        goto got_it;
    }
    if (bitmap_block >= lsb->s_inode_bitmap_blocks) {
        err = -ENOSPC;
        goto create_err;
    }

got_it:
    mark_buffer_dirty(bitmap_bh);
    brelse(bitmap_bh);

    spin_lock(&vfs_sbi->vfs_super_lock);
    vfs_sbi->disk_info.s_free_inode_count --;
    sb->s_dirt = 1;
    spin_unlock(&vfs_sbi->vfs_super_lock);

    inode->i_mode = mode;
    inode->i_ino = ino;
    inode->i_blocks = 0;
    inode->i_mtime = inode->i_atime = inode->i_ctime = CURRENT_TIME_SEC;
    memset(vfs_inode_info->i_block, 0, sizeof(vfs_inode_info->i_block));
    if (insert_inode_locked(inode) < 0) {
        err = -EINVAL;
        goto create_err;
    }
    mark_inode_dirty(inode);
    return inode;

create_err:
    if (inode) {
        inode->i_nlink = 0;
        unlock_new_inode(inode);
        make_bad_inode(inode);
        iput(inode);
    }
    return ERR_PTR(err);
}

int lite_fs_match(int namelen, const char *name, struct lite_fs_dirent *de)
{
    if (namelen != de->name_len) {
        return 0;
    }
    if (!de->inode) {
        return 0;
    }
    return !memcmp(name, de->name, namelen);
}

/*
 * when call this, vfs have confirm this dentry not exist int
 * this directory
 */
int lite_fs_add_link(struct dentry *dentry, struct inode *inode)
{
    struct inode *dir_inode = dentry->d_parent->d_inode;
    const char *name = dentry->d_name.name;
    int namelen = dentry->d_name.len;
    struct page *page = NULL;
    unsigned long npages = (dir_inode->i_size + PAGE_CACHE_SIZE - 1) >> PAGE_CACHE_SHIFT;
    unsigned long n;
    struct lite_fs_dirent *dirent;
    char *kaddr;
    loff_t pos;
    int err = 0;

    LOG_INFO();

    for (n = 0; n <= npages; n ++) {
        char *dir_end;

        page = lite_fs_get_page(dir_inode, n);
        err = PTR_ERR(page);
        if (IS_ERR(page)) {
            goto out;
        }
        lock_page(page);
        kaddr = kmap(page);
        dir_end = kaddr + lite_fs_last_byte(dir_inode, n);
        dirent = (struct lite_fs_dirent *)kaddr;

        while ((char *)dirent < dir_end) {
            if (!dirent->inode) {
                goto got_it;
            }
            dirent ++;
        }
        if ((char *)dirent == dir_end && ((char *)dirent - kaddr) < PAGE_CACHE_SIZE) {
            goto got_it;
        }

        kunmap(page);
        unlock_page(page);
        lite_fs_put_page(page);
    }
    BUG();

 got_it:
    pos = page_offset(page) + (char *)dirent - (char *)kaddr;
    LOG_INFO("%llu", pos);
    err = __lite_fs_write_begin(NULL, page->mapping, pos, LITE_FS_DIRENT_SIZE, 0, &page, NULL);
    if (err) {
        LOG_INFO();
        goto out;
    }
    LOG_INFO("page %llx, pos %llu", (unsigned long long)page, pos);

    dirent->rec_len = LITE_FS_DIRENT_SIZE;
    dirent->name_len = namelen;
    memcpy(dirent->name, name, namelen);
    dirent->inode = inode->i_ino;
    lite_fs_set_dirent_type(dirent, inode);
    kunmap(page);

    block_write_end(NULL, page->mapping, pos, dirent->rec_len, dirent->rec_len, page, NULL);
    if ((pos + dirent->rec_len) > dir_inode->i_size) {
        i_size_write(dir_inode, pos + dirent->rec_len);
    }
    LOG_INFO("dir inode i_size: %llu", dir_inode->i_size);
    mark_inode_dirty(dir_inode);

    unlock_page(page);
    lite_fs_put_page(page);
out:
    return err;
}

static int lite_fs_nondir(struct dentry *dentry, struct inode *inode)
{
    int err;
    LOG_INFO();

    err = lite_fs_add_link(dentry, inode);
    if (!err) {
        d_instantiate(dentry, inode);
        unlock_new_inode(inode);
        return 0;
    }
    inode_dec_link_count(inode);
    unlock_new_inode(inode);
    iput(inode);
    return err;
}

static int lite_fs_create(struct inode *dir_inode, struct dentry *dentry,
                            int mode, struct nameidata *nd)
{
    struct inode *inode;
    int err;
    
    LOG_INFO();

    inode = lite_fs_new_inode(dir_inode, mode);
    err = PTR_ERR(inode);
    if (!IS_ERR(inode)) {
        inode->i_op = &lite_fs_file_inode_iops;
        inode->i_mapping->a_ops = &lite_fs_aops;
        inode->i_fop = &lite_fs_file_fops;
        err = lite_fs_nondir(dentry, inode);
    }
    return err;

}

static struct dentry *lite_fs_lookup(struct inode *dir_inode, struct dentry *dentry,
                            struct nameidata *nd)
{
    struct inode *inode;
    ino_t   ino;

    LOG_INFO();

    if (dentry->d_name.len > LITE_FS_NAME_LEN) {
        return ERR_PTR(-ENAMETOOLONG);
    }

    ino = lite_fs_inode_by_name(dir_inode, &dentry->d_name);
    inode = NULL;
    if (ino) {
        inode = lite_fs_iget(dir_inode->i_sb, ino);
        if (IS_ERR(inode)) {
            return ERR_CAST(inode);
        }
    }
    return d_splice_alias(inode, dentry);
}

int lite_fs_sync_inode(struct inode *inode)
{
    struct writeback_control wbc = {
        .sync_mode = WB_SYNC_ALL,
        .nr_to_write = 0,
    };
    return sync_inode(inode, &wbc);
}

/*
 * only service for directory context write
 */

int lite_fs_commit_chunk(struct page *page, loff_t pos, unsigned len)
{
    struct address_space *mapping = page->mapping;
    struct inode *dir = mapping->host;
    int err = 0;
    
    LOG_INFO();

    block_write_end(NULL, mapping, pos, len, len, page, NULL);

    if (pos + len > dir->i_size) {
        i_size_write(dir, pos + len);
        mark_inode_dirty(dir);
    }
    if (IS_DIRSYNC(dir)) {
        err = write_one_page(page, 1);
        if (!err) {
            lite_fs_sync_inode(dir);
        }
    } else {
        unlock_page(page);
    }
    return err;
}

int lite_fs_is_empty_dir(struct inode *dir_inode)
{
    struct page *page = NULL;
    unsigned long i, npages;
    LOG_INFO();

    npages = (dir_inode->i_size + PAGE_CACHE_SIZE - 1) >> PAGE_CACHE_SHIFT;
    for (i = 0; i < npages; i ++) {
        char *kaddr;
        struct lite_fs_dirent *dirent;
        page = lite_fs_get_page(dir_inode, i);
        if (IS_ERR(page)) {
            continue;
        }
        
        kaddr = kmap(page);
        dirent = (struct lite_fs_dirent *)kaddr;
        kaddr += lite_fs_last_byte(dir_inode, i);

        if ((char *)dirent < kaddr) {
            if (dirent->inode) {
                if (dirent->name[0] != '.') {
                    goto not_empty;
                }
                if (dirent->name_len > 2) {
                    goto not_empty;
                }
                if (dirent->name_len < 2) {
                    if (dirent->inode != dir_inode->i_ino) {
                        goto not_empty;
                    }
                } else if (dirent->name[1] != '.') {
                    goto not_empty;
                }
            }
            dirent = lite_fs_next_dentry(dirent);
        }
        kunmap(page);
        lite_fs_put_page(page);
    }
    return 1;

not_empty:
    kunmap(page);
    lite_fs_put_page(page);
    return 0;
}

int lite_fs_make_empty_dir(struct inode *inode, struct inode *parent)
{
    struct address_space *mapping = inode->i_mapping;
    struct page *page = grab_cache_page(mapping, 0);
    struct lite_fs_dirent *dirent;
    int err;
    char *kaddr;

    LOG_INFO();

    if (page == NULL) {
        return -ENOMEM;
    }
    err = __lite_fs_write_begin(NULL, page->mapping, 0, inode->i_sb->s_blocksize,
                                0, &page, NULL);
    if (err) {
        unlock_page(page);
        page_cache_release(page);
        return err;
    }
    kaddr = kmap(page);
    memset(kaddr, 0, inode->i_sb->s_blocksize);
    dirent = (struct lite_fs_dirent *)kaddr;
    dirent->name_len = 1;
    dirent->rec_len = LITE_FS_DIRENT_SIZE;
    memcpy(dirent->name, ".\0\0", 4);
    dirent->inode = inode->i_ino;
    lite_fs_set_dirent_type(dirent, inode);

    dirent = lite_fs_next_dentry(dirent);
    dirent->name_len = 2;
    dirent->rec_len = LITE_FS_DIRENT_SIZE;
    memcpy(dirent->name, "..\0\0", 4);
    dirent->inode = parent->i_ino;
    lite_fs_set_dirent_type(dirent, parent);

    kunmap(page);

    err = lite_fs_commit_chunk(page, 0, inode->i_sb->s_blocksize);

    page_cache_release(page);
    return err;
}

static int lite_fs_mkdir(struct inode *dir_inode, struct dentry *dentry, int mode)
{
    struct inode *inode;
    int err = -EMLINK;

    LOG_INFO();
    if (dir_inode->i_nlink >= LITE_FS_LINK_MAX) {
        return err;
    }

    inode_inc_link_count(dir_inode);
    inode = lite_fs_new_inode(dir_inode, S_IFDIR | mode);
    err = PTR_ERR(inode);
    if (IS_ERR(inode)) {
        goto out_dir;
    }

    inode->i_op = &lite_fs_dir_inode_iops;
    inode->i_fop = &lite_fs_dir_fops;
    inode->i_mapping->a_ops = &lite_fs_aops;
    inode_inc_link_count(inode);

    err = lite_fs_make_empty_dir(inode, dir_inode);
    if (err) {
        goto out_fail;
    }

    err = lite_fs_add_link(dentry, inode);
    if (err) {
        goto out_fail;
    }

    d_instantiate(dentry, inode);
    unlock_new_inode(inode);

    return err;

out_fail:
    inode_dec_link_count(inode);
    inode_dec_link_count(inode);
    unlock_new_inode(inode);
    iput(inode);
out_dir:
    inode_dec_link_count(dir_inode);
    return err;
}

static int lite_fs_unlink(struct inode *dir_inode, struct dentry *dentry)
{
    struct inode *child_inode = dentry->d_inode;
    struct lite_fs_dirent *dirent;
    struct page *page;
    int err = -ENOMEM;

    dirent = lite_fs_find_entry(dir_inode, &dentry->d_name, &page);
    if (dirent == NULL) {
        goto out;
    }
    err = lite_fs_delete_entry(dirent, page);
    if (err) {
        goto out;
    }
    child_inode->i_ctime = dir_inode->i_ctime;
    inode_dec_link_count(child_inode);
    err = 0;
out:
    return err;
}

static int lite_fs_rm_dir(struct inode *dir_inode, struct dentry *dentry)
{
    struct inode *inode = dentry->d_inode;
    int err = -ENOTEMPTY;
    LOG_INFO();

    if (lite_fs_is_empty_dir(inode)) {
        err = lite_fs_unlink(dir_inode, dentry);
        if (!err) {
            inode->i_size = 0;
            inode_dec_link_count(inode);
            inode_dec_link_count(dir_inode);
        }
    }
    return err;
}

struct inode_operations lite_fs_dir_inode_iops = {
    .create     = lite_fs_create,
    .lookup     = lite_fs_lookup,
    .mkdir      = lite_fs_mkdir,
    .unlink     = lite_fs_unlink,
    .rmdir      = lite_fs_rm_dir,
};    

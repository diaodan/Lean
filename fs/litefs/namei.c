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
    struct buffer_head *bh2;
    struct inode *inode;
    struct lite_fs_inode_info *inode_info;
    __u32   ino = 0;
    int err;
    char *kaddr;

    LOG_INFO();

    sb = dir_inode->i_sb;
    vfs_sbi = (struct lite_fs_vfs_super_info *)sb->s_fs_info;
    lsb = &vfs_sbi->disk_info;
    inode = new_inode(sb);
    inode_info = container_of(inode, struct lite_fs_inode_info, vfs_inode);

    bitmap_block = 0;
    for ( ; bitmap_block < lsb->s_inode_bitmap_blocks; bitmap_block ++) {
        bitmap_bh = sb_bread(sb, lsb->s_first_inode_bitmap_block + bitmap_block);
        kaddr = kmap(bitmap_bh->b_page);
        ino = lite_fs_find_next_zero_bit((unsigned long *)(kaddr + bh_offset(bitmap_bh)), lsb->s_inode_count, 0);
        if (ino >= lsb->s_inode_count) {
            kunmap(bitmap_bh->b_page);
            brelse(bitmap_bh);
            continue;
        }
        if (lite_fs_set_bit_atomic(&vfs_sbi->inode_bitmap_lock, ino, kaddr + bh_offset(bitmap_bh))) {
            kunmap(bitmap_bh->b_page);
            brelse(bitmap_bh);
            continue;
        }
        kunmap(bitmap_bh);
        break;
    }
    if (bitmap_block >= lsb->s_inode_bitmap_blocks) {
        err = -ENOSPC;
        goto create_err;
    }

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
    memset(vfs_inode->i_block, 0, sizeof(vfs_inode->i_block));
    mark_inode_dirty(inode);
    return inode;

create_err:
    if (inode) {
        make_bad_inode(inode);
        iput(inode);
    }
    return ERR_PTR(err);
}

static int lite_fs_add_link(struct dentry *dentry, struct inode *inode)
{
    struct inode *dir_inode = dentry->d_parent->d_inode;
    const char *name = dentry->d_name.name;
    int namelen = dentry->d_name.len;
    struct page *page = NULL;
    unsigned long npages = (dir_inode->i_size + PAGE_CAHCE_SIZE - 1) >> PAGE_CACHE_SHIFT;
    unsigned long n;
    struct lite_fs_dirent *dirent;
    struct lite_fs_dirent *empty_dirent;
    char *kaddr;
    loff_t pos;
    int err;

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
            if (dirent->inode) {
                if (lite_fs_match(namelen, name, dirent)) {
                    err = -EEXIST;
                    goto unlock;
                }
            }
            dirent ++;
        }
        kunmap(page);
        unlock_page(page);
        lite_fs_put_page(page);
    }
unlock:
    kunamp(page);
    unlock_page(page);
    lite_fs_put_page(page);

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
    LOG_INFO();
    return ERR_PTR(-EINVAL);;
}

static int lite_fs_mkdir(struct inode *dir_inode, struct dentry *dentry, int mode)
{
    LOG_INFO();
    return 0;
}

static int lite_fs_rm_dir(struct inode *dir_inode, struct dentry *dentry)
{
    LOG_INFO();
    return 0;
}

struct inode_operations lite_fs_dir_inode_iops = {
    .create     = lite_fs_create,
    .lookup     = lite_fs_lookup,
    .mkdir      = lite_fs_mkdir,
    .rmdir      = lite_fs_rm_dir,
};    

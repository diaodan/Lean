#include "litefs.h"

struct lite_fs_inode *lite_fs_get_raw_inode(struct super_block *sb, unsigned long ino, struct buffer_head **buffer)
{
    struct buffer_head *bh = NULL;
    __u64   block_nr;
    unsigned int ino_offset_in_block = 0;
    struct lite_fs_super_info *lsb = NULL;
    char *kaddr;

    LOG_INFO();

    lsb = (struct lite_fs_super_info *)sb->s_fs_info;

    block_nr = lsb->s_first_inode_block + (ino / LITE_FS_PER_BLOCK_INODE);
    ino_offset_in_block = (ino & (LITE_FS_PER_BLOCK_INODE - 1)) << LITE_FS_INODE_SIZEBITS;
    LOG_INFO("ino  %lu", ino);
    LOG_INFO("block nr %llu, ino offset in block %u", block_nr, ino_offset_in_block);

    bh = sb_bread(sb, block_nr);
    if (bh == NULL) {
        LOG_ERR("LITE fs: get block %llu failed", block_nr);
        return ERR_PTR(-EIO);
    }

    *buffer = bh;
    kaddr = kmap(bh->b_page);
    return (struct lite_fs_inode *)(((char *)kaddr + bh_offset(bh)) + ino_offset_in_block);
}

static void lite_fs_put_raw_inode(struct buffer_head *buffer)
{
    LOG_INFO();
    kunmap(buffer->b_page);
    brelse(buffer);
}

int lite_fs_get_block(struct inode *inode, sector_t iblock,
                        struct buffer_head *bh_result, int create)
{
    struct lite_fs_vfs_inode_info *inode_info = NULL;
    struct super_block *sb = inode->i_sb;
    struct lite_fs_vfs_super_info *vfs_sb = NULL;
    struct buffer_head *bitmap_bh = NULL;
    unsigned long bitno = 0;
    unsigned long bitmap_block;
    char *kaddr;

    LOG_INFO();
    
    sb = inode->i_sb;
    vfs_sb = (struct lite_fs_vfs_super_info *)sb->s_fs_info;

    inode_info = container_of(inode, struct lite_fs_vfs_inode_info, vfs_inode);

    if (iblock < 0 || iblock >= LITE_FS_N_BLOCKS) {
        LOG_ERR("invalid block num for inode %lu, block %lu", inode->i_ino, iblock);
        return -EINVAL;
    }
    if (create) {
        if (inode_info->i_block[iblock] == 0) {
            LOG_INFO("iblock %lu", iblock);
            bitmap_block = 0;
            for ( ; bitmap_block < vfs_sb->disk_info.s_data_bitmap_blocks; bitmap_block ++) {
                bitmap_bh = sb_bread(sb, vfs_sb->disk_info.s_first_data_bitmap_block + bitmap_block);
                kaddr = kmap(bitmap_bh->b_page);
                bitno = lite_fs_find_first_zero_bit((unsigned long *)(kaddr + bh_offset(bitmap_bh)), vfs_sb->disk_info.s_bitmap_size);
                if (bitno >= vfs_sb->disk_info.s_data_blocks) {
                    kunmap(bitmap_bh->b_page);
                    brelse(bitmap_bh);
                    continue;
                }
                if (lite_fs_set_bit_atomic(&vfs_sb->inode_bitmap_lock, bitno, kaddr + bh_offset(bitmap_bh))) {
                    kunmap(bitmap_bh->b_page);
                    brelse(bitmap_bh);
                    continue;
                }
                kunmap(bitmap_bh->b_page);
                goto got_block;
            }
            return -ENOSPC;

        got_block:
            mark_buffer_dirty(bitmap_bh);
            brelse(bitmap_bh);
            inode_info->i_block[iblock] = vfs_sb->disk_info.s_first_data_block + bitno;
            inode_info->vfs_inode.i_blocks ++;
            spin_lock(&vfs_sb->vfs_super_lock);
            vfs_sb->disk_info.s_free_data_block_count --;
            sb->s_dirt = 1;
            spin_unlock(&vfs_sb->vfs_super_lock);
        }
    }

    map_bh(bh_result, inode->i_sb, inode_info->i_block[iblock]);
    if (create) {
        set_buffer_new(bh_result);
    }

    return 0;
}

static int lite_fs_readpage(struct file * filp, struct page *page)
{
    LOG_INFO();
    return mpage_readpage(page, lite_fs_get_block);
}

static int lite_fs_readpages(struct file *filp, struct address_space *mapping,
                        struct list_head *pages, unsigned nr_pages)
{
    LOG_INFO();
    return mpage_readpages(mapping, pages, nr_pages, lite_fs_get_block);
}

static int lite_fs_writepage(struct page *page, struct writeback_control *wbc)
{
    LOG_INFO();
    return block_write_full_page(page, lite_fs_get_block, wbc);
}

static int lite_fs_writepages(struct address_space *mapping, struct writeback_control *wbc)
{
    LOG_INFO();
    return mpage_writepages(mapping, wbc, lite_fs_get_block);
}

int __lite_fs_write_begin(struct file *filp, struct address_space *mapping, loff_t pos,
                            unsigned len, unsigned flags,
                            struct page **pagep, void **fsdata)
{
    LOG_INFO();
    return block_write_begin(filp, mapping, pos, len, flags, pagep, fsdata, lite_fs_get_block);
}


static int lite_fs_write_begin(struct file *filp, struct address_space *mapping, loff_t pos,
                            unsigned len, unsigned flags,
                            struct page **pagep, void **fsdata)
{
    LOG_INFO();
    *pagep = NULL;
    return __lite_fs_write_begin(filp, mapping, pos, len, flags, pagep, fsdata);
}

static int lite_fs_write_end(struct file *filp, struct address_space *mapping, loff_t pos,
                            unsigned len, unsigned copied,
                            struct page *page, void *fsdata)
{
    LOG_INFO();
    return generic_write_end(filp, mapping, pos, len, copied, page, fsdata);
}

struct address_space_operations lite_fs_aops = {
    .readpage       = lite_fs_readpage,
    .readpages      = lite_fs_readpages,
    .writepage      = lite_fs_writepage,
    .writepages     = lite_fs_writepages,
    .write_begin    = lite_fs_write_begin,
    .write_end      = lite_fs_write_end,
};

struct inode * lite_fs_iget(struct super_block *sb, unsigned long ino)
{
    struct inode *inode = NULL;
    struct lite_fs_inode *raw_inode = NULL;
    struct lite_fs_vfs_inode_info *vfs_inode = NULL;
    struct buffer_head *bh = NULL;
    int ret = -EINVAL, i;

    inode = iget_locked(sb, ino);
    if (!inode) {
        return ERR_PTR(-ENOMEM);
    }
    if (!(inode->i_state & I_NEW)) {
        return inode;
    }

    raw_inode = lite_fs_get_raw_inode(inode->i_sb, ino, &bh);
    if (IS_ERR(raw_inode)) {
        ret = PTR_ERR(raw_inode);
        goto bad_inode;
    }

    inode->i_mode = raw_inode->i_mode;
    inode->i_nlink = raw_inode->i_link_count;
    inode->i_size = raw_inode->i_size;
    inode->i_blocks = raw_inode->i_blocks;

    vfs_inode = container_of(inode, struct lite_fs_vfs_inode_info, vfs_inode);

    for (i = 0; i < LITE_FS_N_BLOCKS; i++) {
        vfs_inode->i_block[i] = raw_inode->i_block[i];
    }

    lite_fs_put_raw_inode(bh);

    if (S_ISREG(inode->i_mode)) {
        inode->i_op = &lite_fs_file_inode_iops;
        inode->i_fop = &lite_fs_file_fops;
        inode->i_mapping->a_ops = (struct address_space_operations *)&lite_fs_aops;
    } else if (S_ISDIR(inode->i_mode)) {
        inode->i_op = &lite_fs_dir_inode_iops;
        inode->i_fop = &lite_fs_dir_fops;
        inode->i_mapping->a_ops = (struct address_space_operations *)&lite_fs_aops;
    } else {
        LOG_ERR("LITE fs: unknown file type");
        unlock_new_inode(inode);
        iput(inode);
        inode = NULL;
        goto bad_inode;
    }

    unlock_new_inode(inode);
    return inode;
bad_inode:
    return ERR_PTR(ret);
}

void lite_fs_free_data_block(struct super_block *sb, __u32 block_nr)
{
    struct buffer_head *bitmap_bh;
    struct lite_fs_vfs_super_info *vfs_sb;
    int bit, bitmap_block, bit_offset, bitnum;
    char *kaddr;
    LOG_INFO();

    vfs_sb = (struct lite_fs_vfs_super_info *)sb->s_fs_info;
    bitnum = vfs_sb->disk_info.s_blocksize * 8;

    block_nr = block_nr - vfs_sb->disk_info.s_first_data_block;

    bitmap_block = block_nr / bitnum;
    bit_offset = block_nr % bitnum;
    LOG_INFO("bitmap block %u, bit_offset %u", bitmap_block, bit_offset);

    bitmap_bh = sb_bread(sb, vfs_sb->disk_info.s_first_data_bitmap_block + bitmap_block);
    if (bitmap_bh == NULL) {
        LOG_ERR("LITE fs get data bitmap block failed");
        return ;
    }
    kaddr = kmap(bitmap_bh->b_page);
    bit = lite_fs_clear_bit_atomic(&vfs_sb->inode_bitmap_lock, bit_offset, (kaddr + bh_offset(bitmap_bh)));
    if (bit >= bit_offset) {
        LOG_ERR("LITE fs clear bitmap %u bit fail", block_nr);
    }
    kunmap(bitmap_bh->b_page);
    mark_buffer_dirty(bitmap_bh);
    brelse(bitmap_bh);
    spin_lock(&vfs_sb->vfs_super_lock);
    vfs_sb->disk_info.s_free_data_block_count ++;
    sb->s_dirt = 1;
    spin_unlock(&vfs_sb->vfs_super_lock);
}

void lite_fs_truncate(struct inode *inode)
{
    struct lite_fs_vfs_inode_info *vfs_inode;
    struct super_block *sb;
    __u32 iblock = 0;
    __u32 i;
    
    sb = inode->i_sb;
    vfs_inode = container_of(inode, struct lite_fs_vfs_inode_info, vfs_inode);
    iblock = inode->i_blocks;

    LOG_INFO("iblock %u", iblock);

    for (i = 0; i < iblock; i ++) {
        if (vfs_inode->i_block[i]) {
            lite_fs_free_data_block(sb, vfs_inode->i_block[i]);
            vfs_inode->i_block[i] = 0;
        }
    }
    return;
}

void lite_fs_free_inode(struct inode * inode)
{
    struct super_block *sb;
    struct lite_fs_vfs_super_info *vfs_sb;
    int bit_block, bit_offset, bitnum;
    struct buffer_head *bitmap_bh;
    char *kaddr;

    clear_inode(inode);

    sb = inode->i_sb;
    vfs_sb = (struct lite_fs_vfs_super_info *)sb->s_fs_info;
    bitnum = vfs_sb->disk_info.s_blocksize * 8;

    bit_block = inode->i_ino / bitnum;
    bit_offset = inode->i_ino % bitnum;

    bitmap_bh = sb_bread(sb, vfs_sb->disk_info.s_first_inode_bitmap_block + bit_block);
    if (bitmap_bh == NULL) {
        LOG_ERR("LITE fs get inode bitmap block failed");
        return ;
    }
    kaddr = kmap(bitmap_bh->b_page);
    bitnum = lite_fs_clear_bit_atomic(&vfs_sb->inode_bitmap_lock, bit_offset, (kaddr + bh_offset(bitmap_bh)));
    if (bitnum >= bit_offset) {
        LOG_ERR("LITE fs clear bitmap %lu bit fail", inode->i_ino);
    }
    kunmap(bitmap_bh->b_page);
    brelse(bitmap_bh);
    spin_lock(&vfs_sb->vfs_super_lock);
    vfs_sb->disk_info.s_free_inode_count ++;
    spin_unlock(&vfs_sb->vfs_super_lock);
    sb->s_dirt = 1;

    return ;
}

void lite_fs_delete_inode(struct inode * inode)
{
    truncate_inode_pages(&inode->i_data, 0);

    if (is_bad_inode(inode)) {
        goto no_delete;
    }
    mark_inode_dirty(inode);
    lite_fs_write_inode(inode, 0);

    inode->i_size = 0;
    if (inode->i_blocks) {
        lite_fs_truncate(inode);
    }
    lite_fs_free_inode(inode);

    return ;

no_delete:
    return ;
}


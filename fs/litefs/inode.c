#include "litefs.h"

struct lite_fs_inode *lite_fs_get_raw_inode(struct super_block *sb, unsigned long ino, struct buffer_head **buffer)
{
    struct buffer_head *bh = NULL;
    __u64   block_nr;
    int ino_offset_in_block = 0;
    struct lite_fs_super_info *lsb = NULL;
    char *kaddr;

    LOG_INFO();

    lsb = (struct lite_fs_super_info *)sb->s_fs_info;

    block_nr = lsb->s_first_inode_block + (ino >> LITE_FS_PER_BLOCK_INODE);
    ino_offset_in_block = (ino & (LITE_FS_PER_BLOCK_INODE - 1)) << LITE_FS_INODE_SIZEBITS;

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

static int lite_fs_get_block(struct inode *inode, sector_t iblock,
                        struct buffer_head *bh_result, int create)
{
    struct lite_fs_inode_info *inode_info = NULL;

    LOG_INFO();
    inode_info = container_of(inode, struct lite_fs_inode_info, vfs_inode);

    if (iblock < 0 || iblock >= LITE_FS_N_BLOCKS) {
        LOG_ERR("invalid block num for inode %lu, block %lu", inode->i_ino, iblock);
        return -EINVAL;
    }
    if (create) {
        LOG_ERR("no support create");
        return -EINVAL;
    }

    map_bh(bh_result, inode->i_sb, inode_info->i_block[iblock]);

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

static int lite_fs_write_begin(struct file *filp, struct address_space *mapping, loff_t pos,
                            unsigned len, unsigned flags,
                            struct page **pagep, void **fsdata)
{
    LOG_INFO();
    return block_write_begin(filp, mapping, pos, len, flags, pagep, fsdata, lite_fs_get_block);
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
    struct lite_fs_inode_info *lite_inode = NULL;
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

    lite_inode = container_of(inode, struct lite_fs_inode_info, vfs_inode);

    for (i = 0; i < LITE_FS_N_BLOCKS; i++) {
        lite_inode->i_block[i] = raw_inode->i_block[i];
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

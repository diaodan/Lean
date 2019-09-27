#include "litefs.h"

static int lite_fs_get_block(struct inode *inode, sector_t iblock,
                        struct buffer_head *bh_result, int create)
{
    LOG_INFO();
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






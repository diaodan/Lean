#include "litefs.h"

static struct page *lite_fs_get_page(struct inode *dir, unsigned long index)
{
    struct address_space *mapping = dir->i_mapping;
    struct page *page = read_mapping_page(mapping, index, NULL);
    if (!IS_ERR(page)) {
        return page;
    }
    return page;
}

static void lite_fs_put_page(struct page *page)
{
    page_cache_release(page);
}

struct lite_fs_dirent *lite_fs_next_dentry(struct lite_fs_dirent *de)
{
    return (struct lite_fs_dirent *)((char *)de + de->rec_len);
}

unsigned lite_last_byte(struct inode *inode, unsigned long n)
{
    unsigned last_byte = inode->i_size;

    last_byte -= n << PAGE_CACHE_SHIFT;
    if (last_byte > PAGE_CACHE_SIZE) {
        last_byte = PAGE_CACHE_SIZE;
    }
    return last_byte;
}


//disk type to vfs type translate
static unsigned char lite_fs_type_table[LINK_MAX] = {
    [LITE_FT_UNKNOWN]   = DT_UNKNOWN,
    [LITE_FT_REG_FILE]  = DT_REG,
    [LITE_FT_DIR]       = DT_DIR,
    [LITE_FT_CHRDEV]    = DT_CHR,
    [LITE_FT_BKLDEV]    = DT_BLK,
    [LITE_FT_FIFO]      = DT_FIFO,
    [LITE_FT_SOCK]      = DT_SOCK,
    [LITE_FT_SYMLINK]   = DT_LNK,
};

static int lite_fs_readdir(struct file *filp, void *dirent, filldir_t filldir)
{
    loff_t pos = filp->f_pos;
    struct inode *inode = filp->f_path.dentry->d_inode;
    //struct super_block *sb = inode->i_sb;
    unsigned int offset = pos & ~PAGE_CACHE_MASK;
    unsigned long n = pos >> PAGE_CACHE_SHIFT;
    unsigned long npages = (inode->i_size + PAGE_CACHE_SIZE - 1) >> PAGE_CACHE_SHIFT;

    LOG_INFO();

    if (pos > inode->i_size - LITE_FS_DIRENT_SIZE) {
        return 0;
    }

    for ( ; n < npages; n ++, offset = 0) {
        char *kaddr, *limit;
        struct lite_fs_dirent  *de;
        struct page *page = NULL;

        page = lite_fs_get_page(inode, n);
        if (IS_ERR(page)) {
            LOG_ERR("LITE fs: bad page in %lu", inode->i_ino);
            filp->f_pos += PAGE_CACHE_SIZE - offset;
            return PTR_ERR(page);
        }

        kaddr = (char *)kmap(page);
        de = (struct lite_fs_dirent *)(kaddr + offset);

        limit = kaddr + lite_last_byte(inode, n);

        for ( ; (char*)de < limit; de = lite_fs_next_dentry(de)) {
            if (de->rec_len == 0) {
                LOG_ERR("LITE fs zero-length direntry entry");
                kunmap(page);
                lite_fs_put_page(page);
                return -EIO;
            }

            if (de->inode) {
                unsigned char d_type = lite_fs_type_table[de->file_type];
                int over;

                offset = (char *)de - kaddr;
                over = filldir(dirent, de->name, de->name_len, n << PAGE_CACHE_SHIFT | offset,
                                de->inode, d_type);
                if (over) {
                    kunmap(page);
                    lite_fs_put_page(page);
                    return 0;
                }
            }
            filp->f_pos += de->rec_len;
        }
        kunmap(page);
        lite_fs_put_page(page);
    }
    return 0;
}

struct file_operations lite_fs_dir_fops = {
    .llseek     = generic_file_llseek,
    .read       = generic_read_dir,
    .readdir    = lite_fs_readdir,
};


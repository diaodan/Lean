#include "litefs.h"

struct lite_fs_dirent *lite_fs_find_entry(struct inode *dir_inode, struct qstr *child,
                                    struct page **res_page)
{
    const char *name = child->name;
    int namelen = child->len;
    unsigned long npages = (dir_inode->i_size + PAGE_CACHE_SIZE - 1) >> PAGE_CACHE_SHIFT;
    struct page *page = NULL;
    struct lite_fs_dirent *dirent;
    unsigned long n;


    if (npages == 0) {
        goto out;
    }

    *res_page = NULL;

    for (n = 0; n < npages; n ++) {
        char *kaddr, *limit;
        page = lite_fs_get_page(dir_inode, n);
        if (!IS_ERR(page)) {
            kaddr = kmap(page);
            dirent = (struct lite_fs_dirent *)kaddr;
            limit = kaddr + lite_fs_last_byte(dir_inode, n);
            while ((char *)dirent < limit) {
                if (lite_fs_match(namelen, name, dirent)) {
                    goto found;
                }
                dirent = lite_fs_next_dentry(dirent);
            }
            kunmap(page);
            lite_fs_put_page(page);
        }
    }
out:
    return NULL;

found:
    *res_page = page;
    return dirent;
}

int lite_fs_delete_entry(struct lite_fs_dirent *dirent, struct page *page)
{
    loff_t pos = 0;
    int err = 0;
    char *kaddr = page_address(page);
    int offset = ((char *)dirent - kaddr);
    struct address_space *mapping = page->mapping;
    struct inode *dir_inode = mapping->host;

    pos = page_offset(page) + offset;
    lock_page(page);
    err = __lite_fs_write_begin(NULL, mapping, pos, LITE_FS_DIRENT_SIZE, 0, &page, NULL);
    if (err) {
        goto out;
    }
    dirent->inode = 0;
    dirent->rec_len = 0;
    dirent->name_len = 0;
    dirent->file_type = LITE_FT_UNKNOWN;
    kunmap(page);

    err = lite_fs_commit_chunk(page, pos, LITE_FS_DIRENT_SIZE);
    dir_inode->i_ctime = dir_inode->i_mtime = CURRENT_TIME_SEC;
    mark_inode_dirty(dir_inode);
out:

    lite_fs_put_page(page);
    return err;
}


ino_t lite_fs_inode_by_name(struct inode *dir, struct qstr *child)
{
    ino_t res = 0;
    struct lite_fs_dirent *dirent;
    struct page *page = NULL;
    
    dirent = lite_fs_find_entry(dir, child, &page);
    if (dirent) {
        res = dirent->inode;
        kunmap(page);
        lite_fs_put_page(page);
    }
    return res;
}

struct page *lite_fs_get_page(struct inode *dir, unsigned long index)
{
    struct address_space *mapping = dir->i_mapping;
    struct page *page = read_mapping_page(mapping, index, NULL);
    if (!IS_ERR(page)) {
        return page;
    }
    return page;
}

void lite_fs_put_page(struct page *page)
{
    page_cache_release(page);
}

struct lite_fs_dirent *lite_fs_next_dentry(struct lite_fs_dirent *de)
{
    LOG_INFO();
    return (struct lite_fs_dirent *)((char *) de + LITE_FS_DIRENT_SIZE);
}

unsigned lite_fs_last_byte(struct inode *inode, unsigned long n)
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

#define S_SHIFT 12
unsigned char lite_fs_type_by_mode[S_IFMT >> S_SHIFT] = {
    [S_IFREG >> S_SHIFT]    = LITE_FT_REG_FILE,
    [S_IFDIR >> S_SHIFT]    = LITE_FT_DIR,
    [S_IFCHR >> S_SHIFT]    = LITE_FT_CHRDEV,
    [S_IFBLK >> S_SHIFT]    = LITE_FT_BKLDEV,
    [S_IFIFO >> S_SHIFT]    = LITE_FT_FIFO,
    [S_IFSOCK >> S_SHIFT]   = LITE_FT_SOCK,
    [S_IFLNK >> S_SHIFT]    = LITE_FT_SYMLINK,
};

void lite_fs_set_dirent_type(struct lite_fs_dirent *dirent, struct inode *inode)
{
    mode_t mode = inode->i_mode;
    dirent->file_type = lite_fs_type_by_mode[(mode & S_IFMT) >> S_SHIFT];
    return ;
}

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

        limit = kaddr + lite_fs_last_byte(inode, n);

        for ( ; (char*)de < limit; de = lite_fs_next_dentry(de)) {
            if (de->inode) {
                unsigned char d_type = lite_fs_type_table[de->file_type];
                int over;
                LOG_INFO();

                offset = (char *)de - kaddr;
                over = filldir(dirent, de->name, de->name_len, n << PAGE_CACHE_SHIFT | offset,
                                de->inode, d_type);
                if (over) {
                    kunmap(page);
                    lite_fs_put_page(page);
                    return 0;
                }
            }
            filp->f_pos += LITE_FS_DIRENT_SIZE;
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


#include "litefs.h"

MODULE_LICENSE("Dual BSD/GPL");
int test_blocknr=50;
module_param(test_blocknr, int, 0);


sector_t start_sect = 0;


struct inode_operations lite_fs_file_inode_iops = {

};

struct file_operations lite_fs_file_fops = {
    .read   = do_sync_read,
    .write  = do_sync_write,
    .open   = generic_file_open,
    .llseek = generic_file_llseek,
};


static int lite_fs_create(struct inode *dir_inode, struct dentry *dentry,
                            int mode, struct nameidata *nd)
{
    LOG_INFO();
    return 0;
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
    return (char *)de + de->rec_len;;
}

unsigned lite_last_byte(struct inode *inode, unsigned long n)
{
    unsigned last_byte = inode->i_size;

    last_byte -= n << PAGE_CACHE_SIZE;
    if (last_byte > PAGE_CACHE_SIZE) {
        last_byte = PAGE_CACHE_SIZE;
    }
    return last_byte;
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
        LOG_INFO();

        kaddr = (char *)kmap(page);
        LOG_INFO();
        
        de = (struct lite_fs_dirent *)(kaddr + offset);

        limit = kaddr + lite_last_byte(inode, n);

        for ( ; (char*)de < limit; de = lite_fs_next_dentry(de)) {
            if (de->rec_len == 0) {
                LOG_ERR("LITE fs zero-length direntry entry");
                kunmap(page);
                lite_fs_put_page(page);
                return -EIO;
            }

            LOG_INFO("%x", de);
            if (de->inode) {
                unsigned char d_type = DT_REG;
                int over;
                LOG_INFO("%d,%s", de->name_len, de->name);

                offset = (char *)de - kaddr;
                over = filldir(dirent, de->name, de->name_len, n << PAGE_CACHE_SHIFT | offset,
                                de->inode, d_type);
                if (over) {
                    kunmap(page);
                    lite_fs_put_page(page);
                    return 0;
                }
                LOG_INFO();
            }
            filp->f_pos += de->rec_len;
        }
        kunmap(page);
        lite_fs_put_page(page);
    }
    return 0;
}

struct inode_operations lite_fs_dir_inode_iops = {
    .create     = lite_fs_create,
    .lookup     = lite_fs_lookup,
    .mkdir      = lite_fs_mkdir,
    .rmdir      = lite_fs_rm_dir,
};    

struct file_operations lite_fs_dir_fops = {
    .llseek     = generic_file_llseek,
    .read       = generic_read_dir,
    .readdir    = lite_fs_readdir,
};

static struct inode *lite_fs_alloc_inode(struct super_block *sb)
{
    struct lite_fs_inode_info *inode = NULL;
    LOG_INFO();

    inode = (struct lite_fs_inode_info *)kmalloc(sizeof(*inode), GFP_KERNEL);
    if (inode == NULL) {
        return ERR_PTR(-ENOMEM);
    }

    inode_init_once(&inode->vfs_inode);

    return &inode->vfs_inode;
}

static void lite_fs_destroy_inode(struct inode *inode)
{
    LOG_INFO();
    kfree(container_of(inode, struct lite_fs_inode_info, vfs_inode));
}

static struct super_operations lite_fs_sops = {
    .alloc_inode    = lite_fs_alloc_inode,
    .destroy_inode  = lite_fs_destroy_inode,
//    .write_inode    = lite_fs_write_inode,
};

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


extern struct address_sapce_operations lite_fs_aops;

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
        inode->i_mapping->a_ops = &lite_fs_aops;
    } else if (S_ISDIR(inode->i_mode)) {
        inode->i_op = &lite_fs_dir_inode_iops;
        inode->i_fop = &lite_fs_dir_fops;
        inode->i_mapping->a_ops = &lite_fs_aops;
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


int lite_fs_setup_root_dir(struct super_block *sb, struct lite_fs_super_info *lsb)
{
    struct buffer_head *bh = NULL;
    struct buffer_head *dirbh = NULL;
    struct lite_fs_inode *lite_inode = NULL;
    char *kaddr = NULL;
    unsigned int ret = 0;
    unsigned int ino_block;
    struct lite_fs_dirent *dirent = NULL;
    unsigned int root_ino = LITE_FS_ROOT_INO;
    int ino_offset_in_block;
    LOG_INFO();

    bh = sb_bread(sb, lsb->s_first_inode_bitmap);
    if (bh == NULL) {
        LOG_ERR("LITE fs :get first bitmap failed");
        return -EIO;
    }

    kaddr = kmap(bh->b_page);
    ret = lite_fs_set_bit(LITE_FS_ROOT_INO, (kaddr + bh_offset(bh)));
    kunmap(bh->b_page);
    //if (ret) {
    //    LOG_INFO("LITE fs root has been setup");
    //    brelse(bh);
    //    return 0;
    //}
    LOG_INFO("LITE fs: root setup");

    get_bh(bh);
    lock_buffer(bh);
    bh->b_end_io = end_buffer_write_sync;
    submit_bh(WRITE, bh);
    wait_on_buffer(bh);
    brelse(bh);


    ino_block = root_ino >> (LITE_FS_PER_BLOCK_INODE);
    bh = sb_bread(sb, lsb->s_first_inode_block + ino_block);
    if (bh == NULL) {
        LOG_ERR("LITE fs :get inode block %llu failed", lsb->s_first_inode_block + ino_block);
        return -EIO;
    }
    ino_offset_in_block = (root_ino & (LITE_FS_PER_BLOCK_INODE - 1)) << LITE_FS_INODE_SIZEBITS;

    kaddr = kmap(bh->b_page);
    lite_inode = (struct lite_fs_inode *)(((char *)kaddr + bh_offset(bh)) 
                    + ((root_ino & (LITE_FS_PER_BLOCK_INODE -1)) << LITE_FS_INODE_SIZEBITS));
    memset((char *)lite_inode, 0, LITE_FS_INODE_SIZE);
    lite_inode->i_mode = S_IFDIR;
    lite_inode->i_link_count = 1;
    lite_inode->i_block[0] = lsb->s_first_data_block;
    lite_inode->i_size = LITE_FS_DIRENT_SIZE << 1;// 2 entry (. and ..)
    lite_inode->i_blocks = 1;
    kunmap(bh->b_page);

    get_bh(bh);
    lock_buffer(bh);
    bh->b_end_io = end_buffer_write_sync;
    submit_bh(WRITE, bh);
    wait_on_buffer(bh);
    brelse(bh);

    bh = sb_bread(sb, lsb->s_first_data_bitmap);
    if (bh == NULL) {
        LOG_ERR("LITE fs :get data bitmap block %llu failed", lsb->s_first_data_bitmap);
        return -EIO;
    }

    kaddr = kmap(bh->b_page);
    lite_fs_set_bit(0, (kaddr + bh_offset(bh)));
    kunmap(bh->b_page);

    get_bh(bh);
    lock_buffer(bh);
    bh->b_end_io = end_buffer_write_sync;
    submit_bh(WRITE, bh);
    wait_on_buffer(bh);
    brelse(bh);

    dirbh = sb_bread(sb, lsb->s_first_data_block);
    if (dirbh == NULL) {
        LOG_ERR("LITE fs :get data block %llu failed", lsb->s_first_data_block);
        return -EIO;
    }

    kaddr = kmap(dirbh->b_page);
    dirent = (struct lite_fs_dirent *)((char *)kaddr + bh_offset(dirbh));
    dirent->inode = LITE_FS_ROOT_INO;
    dirent->file_type = LITE_FS_DIR;
    memset(dirent->name, 0, LITE_FS_NAME_SIZE);
    memcpy(dirent->name, ".", strlen("."));
    dirent->name_len = strlen(".");
    dirent->rec_len = LITE_FS_DIRENT_SIZE;

    dirent ++;
    dirent->inode = LITE_FS_ROOT_INO;
    dirent->file_type = LITE_FS_DIR;

    memset(dirent->name, 0, LITE_FS_NAME_SIZE);
    memcpy(dirent->name, "..", strlen(".."));
    dirent->name_len = strlen("..");
    dirent->rec_len = LITE_FS_DIRENT_SIZE;

    kunmap(dirbh->b_page);
    
    get_bh(dirbh);
    lock_buffer(dirbh);
    dirbh->b_end_io = end_buffer_write_sync;
    submit_bh(WRITE, dirbh);
    wait_on_buffer(dirbh);
    brelse(dirbh);

    return 0;
}

static int lite_fs_fill_super(struct super_block *sb, void *data, int silent)
{
    struct buffer_head *bh = NULL;
    struct lite_fs_super_info *lsb = NULL;
    struct lite_fs_super_info *sbi = NULL;
    unsigned int blocksize;
    struct inode *root = NULL;
    int ret = 0;

    LOG_INFO();

    sbi = kzalloc(sizeof(*sbi), GFP_KERNEL);
    if (!sbi) {
        return  -ENOMEM;
    }

    sb->s_fs_info = sbi;

    blocksize = sb_set_blocksize(sb, 1024);

    if (!(bh = sb_bread(sb, 1))) {
        LOG_ERR("unable to read superblock");
        goto failed_lsb;
    }

    lsb = (struct lite_fs_super_info *)((char *)bh->b_data);

    //blocksize = 1k
    lsb->s_blocks_count = sb->s_bdev->bd_part->nr_sects >>(blocksize >> 9);
    start_sect = sb->s_bdev->bd_part->start_sect;

    lsb->s_inodes_count = 100;
    lsb->s_free_blocks_count = lsb->s_blocks_count-1;
    lsb->s_free_inodes_count = lsb->s_inodes_count-1;
    LOG_INFO("%llu  %llu", lsb->s_free_blocks_count, lsb->s_free_inodes_count);
    lsb->s_first_inode_bitmap = INODE_BITMAP_BLOCK;
    lsb->s_first_data_bitmap = DATA_BITMAP_BLOCK;
    lsb->s_first_inode_block = FIRST_INODE_BLOCK;
    lsb->s_first_data_block = FIRST_DATA_BLOCK;
    lsb->s_magic = LITE_MAGIC;

    LOG_INFO("super block  %lu", bh->b_blocknr);

    *sbi = *lsb;

    get_bh(bh);
    lock_buffer(bh);
    bh->b_end_io = end_buffer_write_sync;
    submit_bh(WRITE, bh);
    wait_on_buffer(bh);




    if ((ret = lite_fs_setup_root_dir(sb, lsb))) {
        LOG_ERR();
        return ret;
    }

    sb->s_blocksize = LITE_BLOCKSIZE;
    sb->s_maxbytes = LITE_MAX_FILE_SIZE;
    sb->s_magic = LITE_MAGIC;
    sb->s_blocksize_bits = LITE_BLOCKBITS;

    sb->s_op = &lite_fs_sops;

    root = lite_fs_iget(sb, LITE_FS_ROOT_INO);
    if (IS_ERR(root)) {
        ret = PTR_ERR(root);
        goto failed_lsb;
    }

    sb->s_root = d_alloc_root(root);
    if (!sb->s_root) {
        iput(root);
        LOG_ERR("LITE FS: get root inode failed\n");
        goto failed_lsb;
    }
    brelse(bh);
    return 0;

failed_lsb:
    LOG_ERR("LITE fs: failed fill super");
    if (bh) {
        brelse(bh);
        bh = NULL;
    }
    return ret;
}



static int lite_fs_get_sb(struct file_system_type *fs_type, int flags,
        const char *dev_name, void *data, struct vfsmount *mnt)
{
    LOG_INFO();

    return get_sb_bdev(fs_type, flags, dev_name, data, lite_fs_fill_super, mnt);
}

static void lite_fs_kill_block_super(struct super_block *sb)
{
    LOG_INFO();
    kill_block_super(sb);
}

static struct file_system_type lite_fs_type = {
    .owner      = THIS_MODULE,
    .name       = "lite_fs",
    .get_sb     = lite_fs_get_sb,
    .kill_sb    = lite_fs_kill_block_super,
};


static int init_lite_fs(void)
{
    int err;

    printk(KERN_ALERT "init\n");
    LOG_INFO();

    err = register_filesystem(&lite_fs_type);
    if (err) {
        LOG_ERR("register filesystem failed\n");
    }
    return err;
}


static void exit_lite_fs(void)
{
    printk(KERN_ALERT "exit\n");
    LOG_INFO();
    unregister_filesystem(&lite_fs_type);
}

module_init(init_lite_fs);
module_exit(exit_lite_fs);

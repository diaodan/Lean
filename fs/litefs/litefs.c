#include "litefs.h"

MODULE_LICENSE("Dual BSD/GPL");

struct inode_operations lite_fs_file_inode_iops = {

};

struct file_operations lite_fs_file_fops = {
    .read   = do_sync_read,
    .write  = do_sync_write,
    .open   = generic_file_open,
    .llseek = generic_file_llseek,
};


int lite_fs_create(struct inode *dir_inode, struct dentry *dentry,
                            int mode, struct nameidata *nd)
{
    LOG_INFO();
    return 0;
}

struct dentry *lite_fs_lookup(struct inode *dir_inode, struct dentry *dentry,
                            struct nameidata *nd)
{
    LOG_INFO();
    return NULL;
}

int lite_fs_mkdir(struct inode *dir_inode, struct dentry *dentry, int mode)
{
    LOG_INFO();
    return 0;
}

int lite_fs_rm_dir(struct inode *dir_inode, struct dentry *dentry)
{
    LOG_INFO();
    return 0;
}

int lite_fs_readdir(struct file *filp, void *dirent, filldir_t filldir)
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

struct file_operations lite_fs_dir_fops = {
    .llseek     = generic_file_llseek,
    .read       = generic_read_dir,
    .readdir    = lite_fs_readdir,
};

static struct super_operations lite_fs_sops = {
//    .alloc_inode    = lite_fs_alloc_inode,
//    .destroy_inode  = lite_fs_destroy_inode,
//    .write_inode    = lite_fs_write_inode,
};

struct inode * lite_fs_iget(struct super_block *sb, unsigned long ino)
{
    struct inode *inode = NULL;

    inode = iget_locked(sb, ino);
    if (!inode) {
        return ERR_PTR(-ENOMEM);
    }
    if (!(inode->i_state & I_NEW)) {
        return inode;
    }

    inode->i_mode = S_IFDIR;

    if (S_ISREG(inode->i_mode)) {
        inode->i_op = &lite_fs_file_inode_iops;
        inode->i_fop = &lite_fs_file_fops;
    } else if (S_ISDIR(inode->i_mode)) {
        inode->i_op = &lite_fs_dir_inode_iops;
        inode->i_fop = &lite_fs_dir_fops;
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
    return ERR_PTR(-EINVAL);
}


static int lite_fs_fill_super(struct super_block *sb, void *data, int silent)
{
    LOG_INFO();

    struct buffer_head *bh = NULL;
    struct lite_fs_super_info *lsb = NULL;
    struct lite_fs_super_info *sbi = NULL;
    unsigned int blocksize;
    struct inode *root = NULL;
    int ret = 0;

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
    lsb->s_inodes_count = 100;
    lsb->s_free_blocks_count = lsb->s_blocks_count-1;
    lsb->s_free_inodes_count = lsb->s_inodes_count-1;
    LOG_INFO("%lu  %lu", lsb->s_free_blocks_count, lsb->s_free_inodes_count);
    lsb->s_first_inode_bitmap = INODE_BITMAP_BLOCK;
    lsb->s_first_data_bitmap = DATA_BITMAP_BLOCK;
    lsb->s_first_inode_block = FIRST_INODE_BLOCK;
    lsb->s_first_data_block = FIRST_DATA_BLOCK;
    lsb->s_magic = LITE_MAGIC;


    get_bh(bh);
    lock_buffer(bh);
    bh->b_end_io = end_buffer_write_sync;
    submit_bh(WRITE, bh);
    wait_on_buffer(bh);

    sb->s_blocksize = LITE_BLOCKSIZE;
    sb->s_maxbytes = LITE_MAX_FILE_SIZE;
    sb->s_magic = LITE_MAGIC;
    sb->s_blocksize_bits = LITE_BLOCKBITS;

    sb->s_op = &lite_fs_sops;

    root = lite_fs_iget(sb, 1);
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

    LOG_INFO();

    err = register_filesystem(&lite_fs_type);
    if (err) {
        LOG_ERR("register filesystem failed\n");
    }
    return err;
}


static void exit_lite_fs(void)
{
    LOG_INFO();
    unregister_filesystem(&lite_fs_type);
}

module_init(init_lite_fs);
module_exit(exit_lite_fs);

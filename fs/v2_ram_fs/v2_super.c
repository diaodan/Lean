#include "fs_head.h"

struct file_operations  v2_ram_fs_file_operations = {
    .read       = do_sync_read,
    .write      = do_sync_write,
};

struct inode_operations v2_ram_fs_file_inode_operations = {

};



struct inode_operations v2_ram_fs_dir_inode_operations = {
    .create     = v2_ram_fs_create,
    .lookup     = v2_ram_fs_lookup,
    .mkdir      = v2_ram_fs_mkdir,
    .rmdir      = v2_ram_fs_rmdir,
    .mknod      = v2_ram_fs_mknod,
    .unlink     = v2_ram_fs_unlink,
};

struct file_operations v2_ram_fs_dir_operations = {
    .open       = v2_ram_fs_dir_open,
    .readdir    = v2_ram_fs_dir_readdir,
    .read       = v2_ram_fs_dir_read,
    .release    = v2_ram_fs_dir_close,
    .llseek     = v2_ram_fs_dir_llseek,
};

struct super_operations v2_ram_fs_super_operations = {
    .alloc_inode            = v2_ram_fs_super_alloc_inode,
    .destroy_inode          = v2_ram_fs_super_destroy_inode,
    .dirty_inode            = v2_ram_fs_super_dirty_inode,
    .write_inode            = v2_ram_fs_super_write_inode,
    .drop_inode             = NULL,
    .delete_inode           = NULL,
    .put_super              = v2_ram_fs_super_put_super,
    .write_super            = NULL,
    .sync_fs                = NULL,
    .freeze_fs              = NULL,
    .unfreeze_fs            = NULL,
    .statfs                 = NULL,
    .remount_fs             = NULL,
    .clear_inode            = NULL,
    .umount_begin           = NULL,
    .show_options           = NULL,
    .show_stats             = NULL,
};
    


struct inode *v2_ram_fs_get_inode(struct super_block *sb, int mode, dev_t dev)
{
    struct inode * inode = new_inode(sb);

    INFO();
    if (inode) {
        inode->i_mode = mode;
        inode->i_uid = current_fsuid();
        inode->i_gid = current_fsgid();
        inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;
        switch (mode & S_IFMT) {
           case S_IFREG:
               inode->i_op = &v2_ram_fs_file_inode_operations;
               inode->i_fop = &v2_ram_fs_file_operations;
               break;
            case S_IFDIR:
               inode->i_op = &v2_ram_fs_dir_inode_operations;
               inode->i_fop = &v2_ram_fs_dir_operations;
               inc_nlink(inode);
        }
    }
    return inode;
}

static int v2_ram_fs_fill_super(struct super_block *sb, void *data, int silent)
{
    struct v2_ram_fs_info *fsi = NULL;
    struct inode *inode = NULL;
    struct dentry *root = NULL;
    int err;

    INFO();
    fsi = kzalloc(sizeof(struct v2_ram_fs_info), GFP_KERNEL);
    sb->s_fs_info = fsi;
    if (!fsi) {
        err = -ENOMEM;
        goto fail;
    }
    
    sb->s_maxbytes      = V2_RAM_FS_FILE_SIZE;
    sb->s_blocksize     = PAGE_CACHE_SIZE;
    sb->s_blocksize_bits= PAGE_CACHE_SHIFT;
    sb->s_magic         = V2_RAM_FS_MAGIC;
    //sb->s_op            = &v2_ram_fs_ops;

    inode = v2_ram_fs_get_inode(sb, S_IFDIR, 0);
    if (!inode) {
        err = -ENOMEM;
        goto fail;
    }

    root = d_alloc_root(inode);
    sb->s_root = root;
    if (!root) {
        err = -ENOMEM;
        goto fail;
    }
    return 0;
fail:
    kfree(fsi);
    sb->s_fs_info = NULL;
    iput(inode);
    return err;
}

static int v2_ram_fs_get_sb(struct file_system_type *fs_type,
        int flags, const char *dev_name, void *data, struct vfsmount *mnt)
{
    INFO();
    return get_sb_nodev(fs_type, flags , data, v2_ram_fs_fill_super,
            mnt);
}

static struct file_system_type v2_ram_fs_type = {
    .name       = "v2_ram_fs",
    .get_sb     = v2_ram_fs_get_sb,
    .kill_sb    = kill_litter_super,
};

static int __init init_v2_ram_fs(void)
{
    INFO();
    return register_filesystem(&v2_ram_fs_type);
}

static void __exit exit_v2_ram_fs(void)
{
    INFO();
    unregister_filesystem(&v2_ram_fs_type);
}


module_init(init_v2_ram_fs);
module_exit(exit_v2_ram_fs);
MODULE_LICENSE("GPL");

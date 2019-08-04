#include "fs_head.h"

MODULE_LICENSE("GPL");

const struct file_operations simple_dir_operations = {

};

struct dentry * lite_ramfs_lookup(struct inode *dir, struct dentry *dentry, unsigned int flags)
{
    INFO();
    d_add(dentry, NULL);
    return NULL;
}

static int lite_ramfs_mknod(struct inode *dir, struct dentry *dentry, umode_t mode, dev_t dev)
{
    struct inode * inode = lite_ramfs_get_inode(dir->i_sb, dir, mode, dev);
    int error = -ENOSPC;
    INFO();

    if (inode) {
        d_instantiate(dentry, inode);
        dget(dentry);
        error = 0;
        dir->i_mtime = dir->i_ctime = CURRENT_TIME;
    }
    return error;
}

static int lite_ramfs_mkdir(struct inode * dir, struct dentry * dentry, umode_t mode)
{
    int retval;

    INFO();
    retval = lite_ramfs_mknod(dir, dentry, mode | S_IFDIR, 0);
    if (!retval) {
        inc_nlink(dir);
    }
    return retval;
}

static int lite_ramfs_create(struct inode *dir, struct dentry *dentry, umode_t mode, bool excl)
{
    INFO();
    return lite_ramfs_mknod(dir, dentry, mode | S_IFREG, 0);
}

static const struct inode_operations lite_ramfs_dir_inode_operations = {
    .create     = lite_ramfs_create,
    .lookup     = lite_ramfs_lookup,
    .mkdir      = lite_ramfs_mkdir,
    .mknod      = lite_ramfs_mknod,
};


struct inode *lite_ramfs_get_inode(struct super_block *sb,
                    const struct inode *dir, umode_t mode, dev_t dev)
{
    struct inode * inode = new_inode(sb);
    INFO();

    if (inode) {
        inode->i_ino = get_next_ino();
        inode_init_owner(inode, dir, mode);
        inode->i_mapping->a_ops = NULL;
        inode->i_mapping->backing_dev_info = NULL;
        inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;
        switch (mode &S_IFMT) {
            default:
                init_special_inode(inode, mode, dev);
                break;
            case S_IFREG:
                //inode->i_op = &lite_ramfs_file_inode_operations;
                //inode->i_fop = &lite_ramfs_file_operations;
                break;
            case S_IFDIR:
                inode->i_op = &lite_ramfs_dir_inode_operations;
                inode->i_fop = &simple_dir_operations;
                inc_nlink(inode);
                break;
            case S_IFLNK:
                break;
        }
    }
    return inode;
}

static const struct super_operations lite_ramfs_ops = {

};

int lite_ramfs_fill_super(struct super_block *sb, void *data, int silent)
{
    struct lite_ramfs_info *fsi;
    struct inode *inode;
    INFO();
    
    fsi = kzalloc(sizeof(struct lite_ramfs_info), GFP_KERNEL);
    sb->s_fs_info = fsi;
    if (!fsi) {
        return -ENOMEM;
    }

    sb->s_maxbytes      = LITE_RAMFS_FILE_SIZE;
    sb->s_blocksize     = PAGE_CACHE_SIZE;
    sb->s_magic         = LITE_RAMFS_MAGIC;
    sb->s_op            = &lite_ramfs_ops;
    sb->s_time_gran     = 1;

    inode = lite_ramfs_get_inode(sb, NULL, S_IFDIR, 0);
    sb->s_root = d_make_root(inode);
    if (!sb->s_root) {
        return -ENOMEM;
    }

    return 0;
}


static struct dentry *lite_ramfs_mount(struct file_system_type *fs_type,
        int flags, const char *dev_name, void *data)
{
    INFO();
    return mount_nodev(fs_type, flags, data, lite_ramfs_fill_super);
}

static struct file_system_type ram_fs_type = {
    .name       = "ram_fs",
    .mount      = lite_ramfs_mount,
    .kill_sb    = kill_litter_super,
};

int __init init_lite_ramfs(void)
{
    int err;
    INFO();
    err = register_filesystem(&ram_fs_type);
    return err;
}

void exit_lite_ramfs(void)
{
    INFO();
    unregister_filesystem(&ram_fs_type);
}

module_init(init_lite_ramfs);
module_exit(exit_lite_ramfs);

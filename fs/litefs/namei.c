#include "litefs.h"

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

struct inode_operations lite_fs_dir_inode_iops = {
    .create     = lite_fs_create,
    .lookup     = lite_fs_lookup,
    .mkdir      = lite_fs_mkdir,
    .rmdir      = lite_fs_rm_dir,
};    

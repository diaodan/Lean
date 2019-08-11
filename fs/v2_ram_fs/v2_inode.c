#include "fs_head.h"

int v2_ram_fs_dir_open(struct inode *inode, struct file *file)
{
    INFO();
    return dcache_dir_open(inode, file);
}

int v2_ram_fs_dir_close(struct inode *inode, struct file *file)
{
    INFO();
    return dcache_dir_close(inode, file);
}

loff_t v2_ram_fs_dir_llseek(struct file *file, loff_t offset, int origin)
{
    INFO();
    return dcache_dir_lseek(file, offset, origin);
}

ssize_t v2_ram_fs_dir_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
    INFO();
    return generic_read_dir(filp, buf, size, ppos);
}

int v2_ram_fs_dir_readdir(struct file *filp, void *dirent, filldir_t filldir)
{
    INFO();
    return dcache_readdir(filp, dirent, filldir);
}



static int simple_delete_dentry(struct dentry *dentry)
{
    INFO();
    return 1;
}

struct dentry *v2_ram_fs_lookup(struct inode *dir, struct dentry *dentry, struct nameidata *nd)
{
    static const struct dentry_operations simple_dentry_operations = {
        .d_delete = simple_delete_dentry,
    };
    INFO();
    dentry->d_op = &simple_dentry_operations;
    d_add(dentry, NULL);
    return NULL;
}

static int v2_simple_unlink(struct inode *dir, struct dentry *dentry)
{
    struct inode *inode = dentry->d_inode;

    inode->i_ctime = dir->i_ctime = dir->i_mtime = CURRENT_TIME;
    drop_nlink(inode);
    dput(dentry);
    return 0;
}

int v2_ram_fs_rmdir(struct inode *dir, struct dentry *dentry)
{
    INFO();
    drop_nlink(dentry->d_inode);
    v2_simple_unlink(dir, dentry);
    drop_nlink(dir);
    return 0;
}

int v2_ram_fs_mknod(struct inode *dir, struct dentry *dentry, int mode, dev_t dev)
{
    struct inode *inode = NULL;
    int error = -ENOSPC;

    INFO();
    inode = v2_ram_fs_get_inode(dir->i_sb, mode, dev);
    if (inode) {
        if (dir->i_mode & S_ISGID) {
            inode->i_gid = dir->i_gid;
            if (S_ISDIR(mode)) {
                inode->i_mode |= S_ISGID;
            }
        }
        d_instantiate(dentry, inode);
        dget(dentry);
        error = 0;
        dir->i_mtime = dir->i_ctime = CURRENT_TIME;
    }
    return error;
}

int v2_ram_fs_mkdir(struct inode *dir, struct dentry *dentry, int mode)
{
    int retval;

    INFO();
    retval = v2_ram_fs_mknod(dir, dentry, mode | S_IFDIR, 0);
    if (!retval) {
        inc_nlink(dir);
    }
    return retval;
}

int v2_ram_fs_create(struct inode *dir, struct dentry *dentry, int mode, struct nameidata *nd)
{
    INFO();
    return v2_ram_fs_mknod(dir, dentry, mode | S_IFREG, 0);
}

int v2_ram_fs_unlink(struct inode *dir, struct dentry *dentry)
{
    struct inode *inode = dentry->d_inode;
    INFO();

    inode->i_ctime = dir->i_ctime = dir->i_mtime = CURRENT_TIME;
    drop_nlink(inode);
    dput(dentry);
    return 0;
}

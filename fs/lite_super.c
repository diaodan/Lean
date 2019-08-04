#include "fs_head.h"
#include <linux/module.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/blkdev.h>
#include <linux/buffer_head.h>
#include <linux/fs.h>
#include <linux/vfs.h>
#include <asm/uaccess.h>

MODULE_LICENSE("DUAL BSD/GPL");

struct inode *lite_alloc_inode(struct super_block *sb)
{
    struct inode *in = NULL;
    INFO();
    in = (struct inode *)kmalloc(sizeof(struct inode), GFP_KERNEL);
    if (in == NULL) {
        return NULL;
    }
    return in;
}

void lite_destroy_inode(struct inode *inode)
{
    INFO();
    kfree(inode);
}

static const struct super_operations lite_sop = {
    .alloc_inode        = lite_alloc_inode,
    .destroy_inode      = lite_destroy_inode,
};

static int lite_fs_fill_super(struct super_block *sb, void *data, int silent)
{
    struct dentry *root = NULL;
    struct inode *inode = NULL;
    INFO();
    return 0;
}
static struct dentry * lite_fs_mount(struct file_system_type *fs_type,
        int flags, const char *dev_name, void *data)
{
    INFO();
    return mount_bdev(fs_type, flags, dev_name, data, lite_fs_fill_super);
}


static struct file_system_type lite_fs_type = {
    .owner      = THIS_MODULE,
    .name       = "lite_fs",
    .mount     = lite_fs_mount,
    .kill_sb    = kill_block_super,
    .fs_flags   = FS_REQUIRES_DEV,
};


static int __init init_lite_fs(void)
{
    int err;
    INFO();
    err = register_filesystem(&lite_fs_type);
    if (err) {
        goto out;
    }

out:
    return err;
}

static void __exit exit_lite_fs(void)
{
    INFO();
    unregister_filesystem(&lite_fs_type);
}

module_init(init_lite_fs);
module_exit(exit_lite_fs);



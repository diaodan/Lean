#ifndef __LITE_FS_HEAD__
#define __LITE_FS_HEAD__

#include <linux/module.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/blkdev.h>
#include <linux/buffer_head.h>
#include <linux/fs.h>
#include <linux/vfs.h>
#include <asm/uaccess.h>
#include <linux/gfp.h>
#include <linux/quotaops.h>

#define INFO(fmt, args...) printk(KERN_INFO "func: %s line: %d " fmt"\n", \
                                __func__, __LINE__, ##args)

#define LITE_RAMFS_FILE_SIZE    (1<<20)
#define LITE_RAMFS_MAGIC        (0X123456789)

struct lite_ramfs_info {
    int i;
};

struct inode *lite_ramfs_get_inode(struct super_block *sb, const struct inode *dir, umode_t mode, dev_t dev);


#endif //__FS_HEAD__

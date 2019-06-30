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

#define INFO(fmt, args...) printk(KERN_INFO "func: %s line: %d " fmt"\n", \
                                __func__, __LINE__, ##args)




#endif //__FS_HEAD__

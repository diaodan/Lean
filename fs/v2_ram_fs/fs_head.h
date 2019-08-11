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
#include <linux/pagemap.h>

#define INFO(fmt, args...) printk(KERN_INFO "func: %s line: %d " fmt"\n", \
                                __func__, __LINE__, ##args)

#define V2_RAM_FS_FILE_SIZE    (1<<20)
#define V2_RAM_FS_MAGIC        (0X123456789)

struct v2_ram_fs_info {
    int i;
};

struct inode *v2_ram_fs_get_inode(struct super_block *sb, int mode, dev_t dev);


int v2_ram_fs_dir_open(struct inode *inode, struct file *file);
int v2_ram_fs_dir_close(struct inode *inode, struct file *file);
loff_t v2_ram_fs_dir_llseek(struct file *file, loff_t offset, int origin);
ssize_t v2_ram_fs_dir_read(struct file *filep, char __user *buff, size_t size, loff_t *ppos);
int v2_ram_fs_dir_readdir(struct file *filp, void *dirent, filldir_t filldir);


struct dentry *v2_ram_fs_lookup(struct inode *dir, struct dentry *dentry, struct nameidata *nd);
int v2_ram_fs_rmdir(struct inode *dir, struct dentry *dentry);
int v2_ram_fs_mknod(struct inode *dir, struct dentry *dentry, int mode, dev_t dev);
int v2_ram_fs_mkdir(struct inode *dir, struct dentry *dentry, int mode);
int v2_ram_fs_create(struct inode *dir, struct dentry *dentry, int mode, struct nameidata *nd);
int v2_ram_fs_unlink(struct inode *dir, struct dentry *dentry);


/*
 * super block operations
 */

struct inode *v2_ram_fs_super_alloc_inode(struct super_block *sb);
void v2_ram_fs_super_destroy_inode(struct inode *inode);
void v2_ram_fs_super_dirty_inode(struct inode *inode);
int v2_ram_fs_super_write_inode(struct inode *inode, int flag);
void v2_ram_fs_super_put_super(struct super_block *sb);
#endif //__FS_HEAD__

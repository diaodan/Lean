#include "fs_head.h"

struct inode *v2_ram_fs_super_alloc_inode(struct super_block *sb)
{
    INFO();
    return kmalloc(sizeof(struct inode), GFP_KERNEL);
}

void v2_ram_fs_super_destroy_inode(struct inode *inode)
{
    INFO();
    kfree(inode);
}

void v2_ram_fs_super_dirty_inode(struct inode *inode)
{
    INFO();
}

int v2_ram_fs_super_write_inode(struct inode *inode, int flag)
{
    INFO();
    return 0;
}

void v2_ram_fs_super_put_super(struct super_block *sb)
{
    INFO();
}



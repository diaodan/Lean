#include "litefs.h"

MODULE_LICENSE("Dual BSD/GPL");

static int lite_fs_fill_super(struct super_block *sb, void *data, int silent)
{
    LOG_INFO();

    struct buffer_head *bh = NULL;
    struct lite_fs_super_info *lsb = NULL;

    struct lite_fs_super_info *sbi = NULL;

    sbi = kzalloc(sizeof(*sbi), GFP_KERENL);
    if (!sbi) {
        return = -ENOMEM;
    }

    sb->s_fs_info = sbi;

    blocksize = sb_set_blocksize(sb, 1024);
    


    if (!(bh = sb_bread(sb, 1))) {
        LOG_ERR("unable to read superblock");
        goto failed_lsb;
    }

    lsb = (struct lite_fs_super_info *)((char *)bh->b_data);

    lsb->s_blocks_count = sb->s_bdev->bd_part->nr_sects;



static int lite_fs_get_sb(struct file_system_type *fs_type, int flags,
        const char *dev_name, void *data, struct vfsmount *mnt)
{
    LOG_INFO();

    return get_sb_bdev(fs_type, flags, dev_name, data, lite_fs_fill_super, mnt);
}

static void lite_fs_kill_block_super(struct super_block *sb)
{
    LOG_INFO();
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

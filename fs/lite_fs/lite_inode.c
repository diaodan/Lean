#include "fs_head.h"
#include <linux/quotaops.h>

static int lite_release_file(struct inode *inode, struct file *filp)
{
    INFO();
    return 0;
}

const struct file_operations lite_file_operations = {
    .llseek     = generic_file_llseek,
    .read       = do_sync_read,
    .write      = do_sync_write,
    .open       = generic_file_open,
    .release    = lite_release_file,
};

const struct inode_operations lite_file_inode_operations = {

};





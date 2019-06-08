#ifndef __SBULL_H__

#define INFO(fmt, args...) printk(KERN_INFO "func: %s line: %d " fmt"\n", \
                                __func__, __LINE__, ##args)

#define SBULL_MAJOR     0
#define SBULL_DEVS      2
#define SBULL_RAHEAD    2
#define SBULL_SIZE      2048
#define SBULL_BLKSIZE   1024
#define SBULL_HARDSECT  512


static int sbull_major = 0;
static int hardsect_size = 512;
static int nsectors = 1024;
static int ndevices = 4;

enum {
    RM_SIMPLE   = 0,
    RM_FULL     = 1,
    RM_NOQUEUE  = 2,
};

static int request_mode = RM_NOQUEUE;

#define SBULL_MINORS    16

#define KERNEL_SECTOR_SIZE  512

struct sbull_dev {
    int size;
    u8 *data;
    short users;
    short media_change;
    spinlock_t lock;
    struct request_queue *queue;
    struct gendisk *gd;
};

static struct sbull_dev *Devices = NULL;



#endif  //__SBULL_H__

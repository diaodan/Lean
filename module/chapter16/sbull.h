#ifndef __SBULL_H__

#define INFO(fmt, args...) printk(KERN_INFO "file: %s func: %s line %d " fmt, \
                                __FILE__, __func__, __LINE__, ##args)




#endif  //__SBULL_H__

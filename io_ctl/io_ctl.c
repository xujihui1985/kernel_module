#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include "ioctl_test.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sean");
MODULE_DESCRIPTION("io_ctl example");

static int open(struct inode *dev_file, struct file *f) {
    printk("io_ctl open was called \n");
    return 0;
}

static int close(struct inode *dev_file, struct file *f) {
    printk("io_ctl close was called \n");
    return 0;
}

static int32_t answer = 42;

static long int my_ioctl(struct file *file, unsigned cmd, unsigned long arg) {
    struct mystruct test;
    switch(cmd) {
        case WR_VALUE:
            if(copy_from_user(&answer, (int32_t *) arg, sizeof(answer)))
                printk("ioctl - error copy data from user \n");
            else
                printk("update the answer to %d\n", answer);
            break;
        case RD_VALUE:
            if(copy_to_user((int32_t *) arg, &answer, sizeof(answer))) {
                printk("failed to copy to user \n");
            } else {
                printk("the answer was copied \n");
            }
            break;
        case GREETER:
            if(copy_from_user(&test, (struct mystruct*) arg, sizeof(struct mystruct))) {
                printk("failed to copy from user \n");
            } else {
                printk("ioctl %d greetts to %s \n", test.repeat, test.name);
            }
    }
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = open,
    .release = close,
    .unlocked_ioctl = my_ioctl
};

#define MYMAJOR 91

static int __init ModuleInit(void) {
    int retval;
    // register device nr
    retval = register_chrdev(MYMAJOR, "my_dev_nr", &fops);
    if (retval == 0) {
        printk("dev_nr - registered number Major %d, Minor %d \n", MYMAJOR, 0);
    }
    else if (retval > 0) {
        printk("dev_nr - registered number Major %d, Minor %d \n", retval>>20, retval&0xfffff); //0xfffff 20bit
    }
    else {
        printk("something bad happened\n");
        return -1;
    }
    return 0;
}

static void __exit ModuleExit(void) {
    unregister_chrdev(MYMAJOR, "my_dev_nr");
    printk("good bye hello world\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);


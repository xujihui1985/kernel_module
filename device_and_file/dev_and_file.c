#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sean");
MODULE_DESCRIPTION("register a device nr");

static int open(struct inode *dev_file, struct file *f) {
    printk("dev_nr open was called \n");
    return 0;
}

static int close(struct inode *dev_file, struct file *f) {
    printk("dev_nr close was called \n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = open,
    .release = close
};

#define MYMAJOR 90

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


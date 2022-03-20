#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sean");
MODULE_DESCRIPTION("register a device nr read and write");

static char buffer[255];
static int buffer_pointer;

static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

#define DRIVER_NAME "dummydriver"
#define DRIVER_CLASS "dummydriverclass"

static ssize_t driver_read(struct file *f, char *user_buffer, size_t count, loff_t *offset) {
    int to_copy, not_copied, delta;

    // get amount of data to copy
    to_copy = min(count, buffer_pointer);

    // copy to user
    not_copied = copy_to_user(user_buffer, buffer, to_copy);

    // calculate actual data copied to user
    delta = to_copy - not_copied;

    return delta;
}

static ssize_t driver_write(struct file *f, const char *user_buffer, size_t count, loff_t *offset) {
    int to_copy, not_copied, delta;

    // get amount of data to copy
    to_copy = min(count, sizeof(buffer));

    // copy to user
    not_copied = copy_from_user(buffer, user_buffer, to_copy);
    buffer_pointer = to_copy;

    // calculate actual data copied to user
    delta = to_copy - not_copied;
    return delta;
}


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
    .release = close,
    .read = driver_read,
    .write = driver_write
};

#define MYMAJOR 91

static int __init ModuleInit(void) {
    int retval;

    // allocate a device nr
    if (alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
        printk("could not allocate dev nr\n");
        return -1;
    }

    printk("device nr: Major: %d, Minor: %d was registered \n", my_device_nr >> 20, my_device_nr&0xfffff);

    // create device class
    if ((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
        printk("can not create device class\n");
        goto ClassError;
    }

    if (device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL) {
        printk("can not create device file \n");
        goto FileError;
    }

    // init device file
    cdev_init(&my_device, &fops);
    // register device to kernel
    if (cdev_add(&my_device, my_device_nr, 1) == -1) {
        printk("register device to kernel failed\n");
        goto AddError;
    }

    return 0;

AddError:
    device_destroy(my_class, my_device_nr);
FileError:
    class_destroy(my_class);
ClassError:
    unregister_chrdev(my_device_nr, DRIVER_NAME);
    return -1;

    // register device nr
    /*retval = register_chrdev(MYMAJOR, "my_dev_nr", &fops);*/
    /*if (retval == 0) {*/
        /*printk("dev_nr - registered number Major %d, Minor %d \n", MYMAJOR, 0);*/
    /*}*/
    /*else if (retval > 0) {*/
        /*printk("dev_nr - registered number Major %d, Minor %d \n", retval>>20, retval&0xfffff); //0xfffff 20bit*/
    /*}*/
    /*else {*/
        /*printk("something bad happened\n");*/
        /*return -1;*/
    /*}*/
    return 0;
}

static void __exit ModuleExit(void) {
    cdev_del(&my_device);
    device_destroy(my_class, my_device_nr);
    class_destroy(my_class);
    unregister_chrdev(my_device_nr, DRIVER_NAME);
    printk("unregister %s\n", DRIVER_NAME);
}

module_init(ModuleInit);
module_exit(ModuleExit);


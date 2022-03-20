#include <linux/module.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/wait.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sean");
MODULE_DESCRIPTION("waitqueue demo");

/* Global variables for the threads */
static struct task_struct *kthread_1;
static struct task_struct *kthread_2;

static long int watch_var = 0;

/* static declaration of waitqueue */
DECLARE_WAIT_QUEUE_HEAD(wq1);

/*dynamic declaration of waitqueue */
static wait_queue_head_t wq2;

int thread_function(void *wait_sel) {
    int selection = *(int*) wait_sel;
    switch(selection) {
        case 1:
            wait_event(wq1, watch_var == 11);
            printk("waitqueue watch_var is now 11\n");
            break;
        case 2:
            while(wait_event_timeout(wq2, watch_var == 22, msecs_to_jiffies(5000)) == 0) {
                printk("watch_var is still not 22, but timeout elapsed \n");
            
            printk("watch_var is 22 now\n");
            break;
        default:
            break;
    }
    printk("waitqueue thread monitoring wq %d finished execution \n", selection);
    return 0;
}

static ssize_t driver_write(struct file *f, const char *user_buffer, size_t count, loff_t *offset) {
    int to_copy, not_copied, delta;
    char buffer[16];
    printk("waitqueue write callback called\n");
    memset(buffer, 0, sizeof(buffer));
    // get amount of data to copy
    to_copy = min(count, sizeof(buffer));
    // copy to user
    not_copied = copy_from_user(buffer, user_buffer, to_copy);
    // calculate actual data copied to user
    delta = to_copy - not_copied;

    // convert string to long int
    if(kstrtol(buffer, 10, &watch_var) == -EINVAL) {
        printk("waitqueue error converting input\n");
    }
    printk("watch_var is now %ld\n", watch_var);
    // wake up wq1
    wake_up(&wq1);
    // wake up wq2
    wake_up(&wq2);
    return delta;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = driver_write
};

#define MYMAJOR 93

static int __init ModuleInit(void) {
    int t1 = 1, t2 = 2;
    printk("kthread init threads\n");
    // init waitqueue dynamicly
    init_waitqueue_head(&wq2);

    if(register_chrdev(MYMAJOR, "waitqueue_nr", &fops) != 0) {
        printk("can not register device number\n");
        return -1;
    }

    // create thread
    kthread_1 = kthread_run(thread_function, &t1, "kthread_1");
    if(kthread_1 != NULL) {
        // start the thread
        printk("kthread 1 was created and running\n");
    } else {
        printk("kthread 1 could not be created\n");
        unregister_chrdev(MYMAJOR, "waitqueue_nr");
        return -1;
    }

    kthread_2 = kthread_run(thread_function, &t2, "kthread_2");
    if (kthread_2 != NULL) {
        printk("kthread 2 is running now\n");
    } else {
        printk("kthread 2 could not be created\n");
        watch_var = 11;
        wake_up(&wq1);
        mdelay(10);
        unregister_chrdev(MYMAJOR, "waitqueue_nr");
        return -1;
    }
    printk("kthread both threads are running now");
    return 0;
}

static void __exit ModuleExit(void) {
    printk("kthread stop both\n");
    watch_var = 11;
    wake_up(&wq1);
    mdelay(10);
    watch_var = 22;
    wake_up(&wq2);
    mdelay(10);
    unregister_chrdev(MYMAJOR, "waitqueue_nr");
    kthread_stop(kthread_1);
    kthread_stop(kthread_2);
}

module_init(ModuleInit);
module_exit(ModuleExit);


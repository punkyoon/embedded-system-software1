#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/fs.h>
#include <linux/cdev.h>

#define DEV_NAME "ch1_mod"

#define IOCTL_START_NUM 0x80
#define IOCTL_READ IOCTL_START_NUM+1
#define IOCTL_WRITE IOCTL_START_NUM+2

#define CH1_IOCTL_NUM 'z'
#define CH1_IOCTL_READ _IOWR(CH1_IOCTL_NUM, IOCTL_READ, unsigned long *)
#define CH1_IOCTL_WRITE _IOWR(CH1_IOCTL_NUM, IOCTL_WRITE, unsigned long *)

MODULE_LICENSE("GPL");

int global_value;
EXPORT_SYMBOL(global_value);

static long ch1_mod(struct file *file, unsigned int cmd, unsigned long arg)
{
    switch(cmd)
    {
        // Print global_value
        case CH1_IOCTL_READ:
            printk("global_value = %d\n", global_value);
            break;
        // Increse global_value
        case CH1_IOCTL_WRITE:
            printk("Increased global_value = %d\n", ++global_value);
            break;
        default:
            return -1;
    }

    return 0;
}

static int ch1_mod_open(struct inode *inode, struct file *file)
{
    printk("ch1_mod open\n");
    return 0;
}

static int ch1_mod_release(struct inode *inode, struct file *file)
{
    printk("ch1_mod release\n");
    return 0;
}

struct file_operations ch1_mod_fops =
{
    .unlocked_ioctl = ch1_mod,
    .open = ch1_mod_open,
    .release = ch1_mod_release,
};

static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init ch1_mod_init(void)
{
    printk("ch1_mod init\n");

    alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
    cd_cdev = cdev_alloc();
    cdev_init(cd_cdev, &ch1_mod_fops);
    cdev_add(cd_cdev, dev_num, 1);

    return 0;
}

static void __exit ch1_mod_exit(void)
{
    printk("ch1_mod exit\n");

    cdev_del(cd_cdev);
    unregister_chrdev_region(dev_num, 1);
}

module_init(ch1_mod_init);
module_exit(ch1_mod_exit);

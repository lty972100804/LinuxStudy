#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>

#define GPIOLED_COUNT  1
#define GPIOLED_NAME   "gpioled"

struct gpioled_dev {
	dev_t 			devid;
	struct cdev 	cdev;
	int 			major;
	int 			minor;
};

struct gpioled_dev gpioled;

static const struct file_operations gpioled_fops = {
	.owner = THIS_MODULE,
};

static int __init led_init(void)
{
	int ret = 0;
	printk("led init\n");

	gpioled.major = 0;
	if(gpioled.major) {
		gpioled.devid = MKDEV(gpioled.major, 0);
		register_chrdev_region(gpioled.devid, GPIOLED_COUNT, GPIOLED_NAME);
	} else {
		alloc_chrdev_region(&gpioled.devid, 0, GPIOLED_COUNT, GPIOLED_NAME);
		gpioled.major = MAJOR(gpioled.devid);
		gpioled.minor = MINOR(gpioled.devid);
	}
	printk("gpioled.major = %d, gpioled.minor = %d\n", gpioled.major, gpioled.minor);

	gpioled.cdev.owner = THIS_MODULE;
	cdev_init(&gpioled.cdev, &gpioled_fops);
	cdev_add(&gpioled.cdev, gpioled.devid, GPIOLED_COUNT);

	return 0;
}

static void __exit led_exit(void)
{
	printk("led exit\n");
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("liutianyu");

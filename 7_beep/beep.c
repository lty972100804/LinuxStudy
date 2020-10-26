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
#include <linux/gpio.h>
#include <linux/of_gpio.h>

#define BEEP_COUNT 		1
#define BEEP_NAME    "beep"
#define BEEP_ON 	1
#define BEEP_OFF 	0

struct beep_dev{
	dev_t devid;
	struct cdev cdev;
	int major;
	int minor;
	struct class *class;
	struct device *device;
	struct device_node *nd;
	int beep_gpio;
};

struct beep_dev beep;

static int beep_open(struct inode *inode, struct file *filp)
{
	filp->private_data = &beep;
	printk("beep open\n");
	return 0;
}

static int beep_release(struct inode *inode, struct file *filp)
{
	struct beep_dev *dev = filp->private_data;
	return 0;	
}

static ssize_t beep_write(struct file *filp, const char __user *buf,
													size_t count, loff_t *ppos)
{
	int ret;
	struct beep_dev *dev = filp->private_data;
	unsigned char databuf[1];
	
	ret = copy_from_user(databuf, buf, count);
	if (ret < 0) {
		printk("copy from user fail\n");
		return ret;
	}
	if (databuf[0] == BEEP_ON) {
		gpio_set_value(dev->beep_gpio, 0);
	} else if (databuf[0] == BEEP_OFF) {
		gpio_set_value(dev->beep_gpio, 1);
	}

	return 0;
}

static const struct file_operations beep_fops = {
	.owner = THIS_MODULE,
	.open = beep_open,
	.release = beep_release,
	.write = beep_write,
};

static int __init beep_init(void)
{
	int ret = 0;

	printk("beep init\n");
	beep.major = 0;
	if (beep.major) {
		beep.devid = MKDEV(beep.major, 0);
		ret = register_chrdev_region(beep.devid, BEEP_COUNT, BEEP_NAME);
	} else {
		ret = alloc_chrdev_region(&beep.devid, 0, BEEP_COUNT, BEEP_NAME);
	}
	if (ret < 0) {
		goto fail_devid;
	}
	printk("devid success\n");

	beep.cdev.owner = THIS_MODULE;
	cdev_init(&beep.cdev, &beep_fops);
	ret = cdev_add(&beep.cdev, beep.devid, BEEP_COUNT);
	if (ret < 0) {
		goto fail_cdev;
	}
	printk("cdev success\n");

	beep.class = class_create(THIS_MODULE, BEEP_NAME);
	if (IS_ERR(beep.class)) {
		ret = PTR_ERR(beep.class);
		goto fail_class;
	}
	printk("class success\n");

	beep.device = device_create(beep.class, NULL, beep.devid, NULL, BEEP_NAME);
	if (IS_ERR(beep.device)) {
		ret = PTR_ERR(beep.device);
		goto fail_device;
	}
		printk("device success\n");

	/* 初始化BEEP */
	beep.nd = of_find_node_by_path("/beep");
	if (beep.nd == NULL) {
		ret = -EINVAL;
		goto fail_nd;
	}
	printk("find node success\n");

	beep.beep_gpio = of_get_named_gpio(beep.nd, "beep-gpios", 0);
	if (beep.beep_gpio < 0) {
		ret = -EINVAL;
		goto fail_nd;
	}
	printk("gpio get success\n");

	ret = gpio_request(beep.beep_gpio, "beep-gpio");
	if (ret ) {
		printk("can't request beep gpio");
		goto fail_nd;
	}
	printk("request gpio success\n");

	ret = gpio_direction_output(beep.beep_gpio, 0);
	if (ret < 0) {
		goto fail_set;
	}
	printk("gpio set output success\n");
	gpio_set_value(beep.beep_gpio, 0);
	printk("gpio set success\n");

	return 0;
fail_set:
	gpio_free(beep.beep_gpio);
fail_nd:
	device_destroy(beep.class, beep.devid);
fail_device:
	class_destroy(beep.class);
fail_class:
	cdev_del(&beep.cdev);
fail_cdev:
	unregister_chrdev_region(beep.devid, BEEP_COUNT);
fail_devid:
	return ret;
}

static void __exit beep_exit(void)
{
	printk("beep exit\n");
	gpio_free(beep.beep_gpio);
	printk("gpio free \n");
	device_destroy(beep.class, beep.devid);
	printk("device destroy\n");
	class_destroy(beep.class);
	printk("class destroy\n");
	cdev_del(&beep.cdev);
	printk("cdev delete\n");
	unregister_chrdev_region(beep.devid, BEEP_COUNT);
	printk("unregister chrdev\n");
}

module_init(beep_init);
module_exit(beep_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("liutianyu");
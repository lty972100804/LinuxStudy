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

#define DTSLED_COUNT  1
#define DTSLED_NAME   "dtsled"

struct dtsled_dev {
	dev_t				devid;
	struct 	cdev 		cdev;
	struct class 		*class;
	struct device 		*device;
	int					major;
	int					minor;
	struct device_node 	*nd;
};

struct dtsled_dev dtsled;

static int dtsled_open(struct inode *inode, struct file *filp)
{
	filp->private_data = &dtsled;
	return 0;
}

static int dtsled_release(struct inode *inode, struct file *filp)
{
	struct dtsled_dev *dev = (struct dtsled_dev*)filp->private_data;
	return 0;
}

static ssize_t dtsled_write(struct file *filp, const char __user *buf, 
							size_t count, loff_t *ppos)
{
	struct dtsled_dev *dev = (struct dtsled_dev*)filp->private_data;
	return 0;
}

static const struct file_operations dtsled_fops = {
	.owner = THIS_MODULE,
	.write = dtsled_write,
	.open = dtsled_open,
	.release = dtsled_release,
};

static int __init dtsled_init(void)
{
	int ret = 0;
	const char *str;
	u32 regdata[10];
	u8 i = 0;

	printk("dtsled init\n");

	/* 1.regist dev_t */
	dtsled.major = 0;
	if (dtsled.major) {
		dtsled.devid = MKDEV(dtsled.major, 0);
		ret = register_chrdev_region(dtsled.devid, DTSLED_COUNT, DTSLED_NAME);
	} else {
		ret = alloc_chrdev_region(&dtsled.devid, 0, DTSLED_COUNT, DTSLED_NAME);
		dtsled.major = MAJOR(dtsled.devid);
		dtsled.minor = MINOR(dtsled.devid);
	}

	if (ret < 0) {
		goto fail_devid;
	}

	/* 2.add cdev */
	dtsled.cdev.owner = THIS_MODULE;
	cdev_init(&dtsled.cdev, &dtsled_fops);
	ret = cdev_add(&dtsled.cdev, dtsled.devid, DTSLED_COUNT);
	if (ret < 0) {
		goto fail_cdev;
	}

	/* 3.create class */
	dtsled.class = class_create(THIS_MODULE, DTSLED_NAME);
	if(IS_ERR(dtsled.class)) {
		ret = PTR_ERR(dtsled.class);
		goto fail_class;
	}
	/* 4.create device */
	dtsled.device = device_create(dtsled.class, NULL, dtsled.devid, NULL, DTSLED_NAME);
	if (IS_ERR(dtsled.device)) {
		ret = PTR_ERR(dtsled.device);
		goto fail_device;
	}

	/* 5.get the device node */
	dtsled.nd = of_find_node_by_path("/alphaled");
	if (dtsled.nd == NULL) {
		ret = -EINVAL;
		goto fail_findnd;
	}

	ret = of_property_read_string(dtsled.nd, "status", &str);
	if (ret < 0) {
		goto fail_rs;
	} else {
		printk("status = %s\n", str);
	}

	ret = of_property_read_string(dtsled.nd, "compatible", &str);
	if (ret < 0) {
		goto fail_rs;
	} else {
		printk("compatible = %s\n", str);
	}

	ret = of_property_read_u32_array(dtsled.nd, "reg", regdata, 10);
	if (ret < 0) {
		goto fail_rs;
	} else {
		for (i = 0; i < 10; ++i) {
			printk("regdata[%d] = %d\n", i, regdata[i]);
		}
	}
	/* of_iomap:get the "reg" property value and remap */

	return 0;

fail_rs:
fail_findnd:
	device_destroy(dtsled.class, dtsled.devid);
fail_device:
	class_destroy(dtsled.class);
fail_class:
	cdev_del(&dtsled.cdev);
fail_cdev:
	unregister_chrdev_region(dtsled.devid, DTSLED_COUNT);
fail_devid:
	return ret;

}

static void __exit dtsled_exit(void)
{
	printk("dtsled exit\n");
	device_destroy(dtsled.class, dtsled.devid);
	class_destroy(dtsled.class);
	cdev_del(&dtsled.cdev);
	unregister_chrdev_region(dtsled.devid, DTSLED_COUNT);
}

module_init(dtsled_init);
module_exit(dtsled_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("liutianyu");


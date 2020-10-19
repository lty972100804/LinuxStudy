#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define NEWCHRLED_NAME      "newchrled"
#define NEWCHRLED_COUNT          1

#if 0
/* define register physcial addr */
#define CCM_CCGR1_BASE               (0x020C406C)
#define SW_MUX_GPIO1_IO03_BASE       (0x020E0068)
#define SW_PAD_FPIO1_IO03_BASE       (0x020E02F4)
#define GPIO1_DR_BASE                (0x0209C000)
#define GPIO1_GDIR_BASE              (0x0209C004)

#define LEDOFF   0
#define LEDON    1

/* after addr remap, virtual addr */
static void __iomem *IMX6U_CCM_CCGR1;
static void __iomem *SW_MUX_GPIO1_IO03;
static void __iomem *SW_PAD_FPIO1_IO03;
static void __iomem *GPIO1_DR;
static void __iomem *GPIO1_GDIR;
#endif

/* LED设备结构体 */
struct newchrled_dev {
	struct cdev    cdev;
	dev_t          devid;
	struct class   *class;
	struct device  *device;
	int            major;
	int            minor;
};

struct newchrled_dev newchrled;

static int newchrled_open(struct inode *inode, struct file *filp)
{
	printk("newchrled open\n");
	return 0;
}

static int newchrled_release(struct inode *inode, struct file *filp)
{
	printk("newchrled release\n");
	return 0;
}

static ssize_t newchrled_write(struct file *filp, const char __user *buf, 
								size_t count, loff_t *ppos)
{
	return 0;
}

static const struct file_operations newchrled_fops = {
	.owner = THIS_MODULE,
	.open = newchrled_open,
	.release = newchrled_release,
	.write = newchrled_write,
};

static int __init newchrled_init(void)
{
	int ret = 0;
	printk("newchrled init\n");

	if(newchrled.major) {
		newchrled.devid = MKDEV(newchrled.major, 0);
		ret = register_chrdev_region(newchrled.devid, NEWCHRLED_COUNT, NEWCHRLED_NAME);
	} else {
		ret = alloc_chrdev_region(&newchrled.devid, 0, NEWCHRLED_COUNT, NEWCHRLED_NAME);
		newchrled.major = MAJOR(newchrled.devid);
		newchrled.minor = MINOR(newchrled.devid);
	}
	if(ret < 0) {
		printk("newchrled chrdev_region err!\n");
		return -1;
	}
	printk("newchrled major = %d, minor = %d\n", newchrled.major, newchrled.minor);

	newchrled.cdev.owner = THIS_MODULE;
	cdev_init(&newchrled.cdev, &newchrled_fops);
	ret = cdev_add(&newchrled.cdev, newchrled.devid, NEWCHRLED_COUNT);

	newchrled.class = class_create(THIS_MODULE, NEWCHRLED_NAME);
	if (IS_ERR(newchrled.class)) {
		return PTR_ERR(newchrled.class);
	}

	newchrled.device = device_create(newchrled.class, NULL, newchrled.devid, 
										NULL, NEWCHRLED_NAME);
	if (IS_ERR(newchrled.device)) {
		return PTR_ERR(newchrled.device);
	}
	return 0;
}

static void __exit newchrled_exit(void)
{
	printk("newchrled exit\n");

	cdev_del(&newchrled.cdev);
	unregister_chrdev_region(newchrled.devid, NEWCHRLED_COUNT);
	device_destroy(newchrled.class, newchrled.devid);
	class_destroy(newchrled.class);
}

module_init(newchrled_init);
module_exit(newchrled_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("liutianyu");

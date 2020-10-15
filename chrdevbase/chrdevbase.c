#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define CHRDEVBASE_MAJOR  200
#define CHRDEVBASE_NAME   "chrdevbase"

static char readbuf[100]; /* read buf */
static char writebuf[100]; /* write buf */
static char kerneldata[] = {"kernel data!"};

static int chrdevbase_open(struct inode *inode, struct file *filp)
{
	printk("chrdevbase open\n");
	return 0;
}

static int chrdevbase_release(struct inode *inode, struct file *filp)
{
	printk("chrdevbase release\n");
	return 0;
}

ssize_t chrdevbase_read(struct file *filp, char __user *buf, size_t count, loff_t *ppos)
{
	int ret = 0;

	printk("chrdevbase read\n");

	memcpy(readbuf, kerneldata, sizeof(kerneldata));
	ret = copy_to_user(buf, readbuf, count);
	if (ret != 0) {
		printk("chrdevbase read failed!\n");
	}
	
	return 0;
}

ssize_t chrdevbase_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos)
{
	int ret = 0;
	printk("chrdevbase write\n");

	ret = copy_from_user(writebuf, buf, count);
	if (ret != 0) {
		printk("write failed\n");
	} else {
		printk("write %s in kernel\n", writebuf);	
	}
	return 0;
}

static struct file_operations chrdevbase_fops = {
	.owner = THIS_MODULE,
	.open = chrdevbase_open,
	.release = chrdevbase_release,
	.write = chrdevbase_write,
	.read = chrdevbase_read,
};

static int __init chrdevbase_init(void)
{
	printk("chrdevbase_init\n");

	register_chrdev(CHRDEVBASE_MAJOR, CHRDEVBASE_NAME, &chrdevbase_fops);

	return 0;
}

static void __exit chrdevbase_exit(void)
{
	printk("chrdevbase_exit\n");

	unregister_chrdev(CHRDEVBASE_MAJOR, CHRDEVBASE_NAME);
}


module_init(chrdevbase_init);
module_exit(chrdevbase_exit);

MODULE_LICENSE("GPL");

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/fs.h>

#define CHRDEVBASE_MAJOR   200
#define CHRDEVBASE_NAME    "chrdevbase"

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

static ssize_t chrdevbase_read(struct file *filp, __user char *buf, size_t count, loff_t *ppos)
{
	printk("chrdevbase read\n");
	return 0;
}

static ssize_t chrdevbase_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos)
{
	printk("chrdevbase write\n");
	return 0;
}

static struct file_operations chrdevbase_fops = {
	.owner = THIS_MODULE,
	.open = chrdevbase_open,
	.release = chrdevbase_release,
	.read = chrdevbase_read,
	.write = chrdevbase_write,
};

static int __init chrdevbase_init(void)
{
	int ret = 0;

	printk("chedevbase init\n");


	/* register char device */
	ret = register_chrdev(CHRDEVBASE_MAJOR, CHRDEVBASE_NAME, &chrdevbase_fops);
	if (ret < 0) {
		printk("chrdevbase init failed!\n");
	}

	return 0;	
}

static void __exit chrdevbase_exit(void)
{
	printk("chedevbase exit\n");

	/* unregister char device */
	unregister_chrdev(CHRDEVBASE_MAJOR, CHRDEVBASE_NAME);
}

module_init(chrdevbase_init);
module_exit(chrdevbase_exit);


MODULE_LICENSE("GPL");

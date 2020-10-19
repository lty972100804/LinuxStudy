#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define LED_MAJOR    200
#define LED_NAME    "led"

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

void led_switch(u8 sta)
{
	u32 val = 0;

	if (sta == LEDON) {
		val = readl(GPIO1_DR);
		val &= ~(1 << 3);
		writel(val, GPIO1_DR);
	} else if (sta == LEDOFF) {
		val = readl(GPIO1_DR);
		val |= 1 << 3;
		writel(val, GPIO1_DR);
	}
}

static int led_open(struct inode *inode, struct file *filp)
{
	printk("led_open\n");
	return 0;
}

static int led_release(struct inode *inode, struct file *filp)
{
	printk("led_release\n");
	return 0;
}

static ssize_t led_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos)
{
	unsigned int retvalue;
	unsigned char databuf[1];

	printk("led_write\n");

	retvalue = copy_from_user(databuf, buf, count);
	if(retvalue < 0) {
		printk("kernel write failed!\n");
		return -EFAULT;
	}

	/* 判断开灯还是关灯 */
	led_switch(databuf[0]);

	return 0;
}

static const struct file_operations led_fops = {
	.owner = THIS_MODULE,
	.open = led_open,
	.write = led_write,
	.release = led_release,
};

static int __init led_init(void)
{

	int ret = 0;
	unsigned int val = 0;

	printk("led_init\n");

	/* 1.init led addr remap */
	IMX6U_CCM_CCGR1 = ioremap(CCM_CCGR1_BASE, 4);
	SW_MUX_GPIO1_IO03 = ioremap(SW_MUX_GPIO1_IO03_BASE, 4);
	SW_PAD_FPIO1_IO03 = ioremap(SW_PAD_FPIO1_IO03_BASE, 4);
	GPIO1_DR = ioremap(GPIO1_DR_BASE, 4);
	GPIO1_GDIR = ioremap(GPIO1_GDIR_BASE, 4);

	/* 2.init */
	val = readl(IMX6U_CCM_CCGR1);
	val &= ~(3 << 26);
	val |= 3 << 26;
	writel(val, IMX6U_CCM_CCGR1);

	writel(0x5, SW_MUX_GPIO1_IO03);
	writel(0x10B0, SW_PAD_FPIO1_IO03);

	val = readl(GPIO1_GDIR);
	val |= 1 << 3; /* bit 3 set 1 */
	writel(val, GPIO1_GDIR);

	val = readl(GPIO1_DR);
	val &= ~(1 << 3); /* bit3清零，打开LED灯 */
	writel(val, GPIO1_DR);

	ret = register_chrdev(LED_MAJOR, LED_NAME, &led_fops);
	if (ret < 0) {
		printk("register failed!\n");
		return -EIO;
	}

	return 0;
}

static void __exit led_exit(void)
{
	unsigned int val;
	printk("led_exit\n");

	val = readl(GPIO1_DR);
	val |= (1 << 3);
	writel(val, GPIO1_DR);

	/* unmap */
	iounmap(IMX6U_CCM_CCGR1);
	iounmap(SW_MUX_GPIO1_IO03);
	iounmap(SW_PAD_FPIO1_IO03);
	iounmap(GPIO1_DR);
	iounmap(GPIO1_GDIR);


	unregister_chrdev(LED_MAJOR, LED_NAME);
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("liutianyu");

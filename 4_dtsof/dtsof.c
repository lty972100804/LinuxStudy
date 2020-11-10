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

#if 0
	backlight {
		compatible = "pwm-backlight";
		pwms = <&pwm1 0 5000000>;
		brightness-levels = <0 4 8 16 32 64 128 255>;
		default-brightness-level = <6>;
		status = "okay";
	};
#endif

static int __init dtsof_init(void)
{
	int ret = 0;
	struct device_node *bl_nd = NULL;
	struct property *comppro = NULL;
	const char *str;
	u32 def_value = 0;
	u32 *brival;
	u32 elemsize = 0;
	u8 i =0;

	printk("dtsof init\n");

	/* 1.find the device node path is /backlight */
	bl_nd = of_find_node_by_path("/backlight");
	if (bl_nd == NULL) {
		ret = -EINVAL;
		goto fail_find_nd;
	}

	/* 2. get the property */
	comppro = of_find_property(bl_nd, "compatible", NULL);
	if (comppro == NULL) {
		ret = -EINVAL;
		goto fail_find_pro;
	} else {
		printk("compatible = %s\n", (char *)comppro->value);
	}

	ret = of_property_read_string(bl_nd, "status", &str);
	if (ret < 0) {
		goto fail_read_str;
	} else {
		printk("status = %s\n", str);
	}

	/* 3.read numbers */
	ret = of_property_read_u32(bl_nd, "default-brightness-level", &def_value);
	if (ret < 0) {
		goto fail_read32;
	} else {
		printk("default-brightness-level = %d\n", def_value);
	}

	/* 4. read num array */
	elemsize = of_property_count_elems_of_size(bl_nd, "brightness-levels", sizeof(u32));
	if (ret < 0) {
		ret = -EINVAL;
		goto fail_readele;
	} else {
		printk("brightness-levels elems size = %d\n", elemsize);
	}

	/* malloc memory */
	brival = kmalloc(ret * sizeof(u32), GFP_KERNEL);
	if (!brival) {
		ret = -EINVAL;
		goto fail_mem;
	}

	/* get array */
	ret = of_property_read_u32_array(bl_nd, "brightness-levels", brival, elemsize);
	if (ret < 0) {
		goto fail_read32_array;
	} else {
		for (i = 0; i < elemsize; ++i) {
			printk("brightness-levels[%d] = %d\n", i, *(brival + i));
		}
	}

	kfree(brival);
	return 0;

fail_read32_array:
	kfree(brival);
fail_mem:
fail_readele:
fail_read32:
fail_read_str:
fail_find_pro:
fail_find_nd:
	return ret;
}

static void __exit dtsof_exit(void)
{
	printk("dtsof exit\n");
}

module_init(dtsof_init);
module_exit(dtsof_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("liutianyu");

/*
 * Custom platform driver
 *
 * After compiling and loading this driver, we should be able to see it
 * under /sys/bus/platform.
 *
 * $ make
 * $ scp custom_platform_driver.ko pi@192.168.2.2:~/Development
 * custom_platform_driver.ko                  100% 7136     7.0KB/s   00:00
 *
 * In the RPi3
 *
 * pi@raspberrypi: $ sudo insmod custom_platform_driver.ko
 * pi@raspberrypi: $ ls /sys/bus/platform/drivers/custom_platform_driver/
 * bind  module  uevent  unbind
 */


#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>


static int custom_platform_probe(struct platform_device *pdev)
{
	pr_info("%s: Probing the platform device driver", __func__);

	return 0;
}

static int custom_platform_remove(struct platform_device *pdev)
{
	pr_info("%s: Removing the platform device driver", __func__);

	return 0;
}


/*
 * Defines the platform device id table
 *
 * https://elixir.bootlin.com/linux/v5.17.4/source/include/linux/mod_devicetable.h#L593
 *
 * The last entry in the table has to be NULL, otherwise we get:
 *
 *     FATAL: modpost: custom_platform_driver: struct platform_device_id is not
 *     terminated with a NULL entry!
 */
static const struct platform_device_id custom_platform_id_table[] = {
	{"custom_plat_dev_1", 23},
	{"custom_plat_dev_2", 33},
	{}, // Needs to finish with a NULL entry.
};

MODULE_DEVICE_TABLE(platform, custom_platform_id_table);

static struct platform_driver custom_platform_driver = {
	.driver = {
		.name = "custom_platform_driver",
		.owner = THIS_MODULE,
	},
	.probe = custom_platform_probe,
	.remove = custom_platform_remove,
	.id_table = custom_platform_id_table
};

module_platform_driver(custom_platform_driver);

MODULE_LICENSE("GPL v2");


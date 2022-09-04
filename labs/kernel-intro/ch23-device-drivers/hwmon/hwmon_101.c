// SPDX-License-Identifier: GPL-2.0
/**
 * Hardware Monitor driver for the temperature sensors.
 *
 * This driver requires a new entry in the device tree for the RPi 3 B+:
 *
 *  $ vim linux/arch/arm/boot/dts/bcm2837.dtsi
 *
 *      hwmon_101_dev {
 *          compatible = "brcm,bcm2837,hwmon_101_dev";
 *      };
 *
 * $ sudo insmod hwmon_101.ko
 * $ grep .  /sys/bus/platform/devices/hwmon_101_dev/hwmon/hwmon2/temp*
 * $ dmesg
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/device.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/hwmon.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>

#define SENSOR_BIAS 10000

static inline long extract_temp(int channel)
{
	switch (channel) {
	case 0:
		return lower_32_bits(0xABCD);
	case 1:
		return upper_32_bits(0xAABBCCDD);
	case 2:
		return lower_32_bits(0xCDab);
	case 3:
		return upper_32_bits(0xBBCCDDAA);
	case 4:
		return lower_32_bits(0xABCDAB);
	case 5:
		return upper_32_bits(0xAABBCCDDFF);
	case 6:
		return lower_32_bits(0xCDabFF);
	case 7:
		return upper_32_bits(0xBBCCDDAAFF);
	default:
		pr_err("Invalid channel %d\n", channel);
		return -EINVAL;
	}
}

/*
 * Returns the permissions of the hwmon temperature sensors. If the type
 * is not hwmon_temp, return 0.
 */
static umode_t
hwmon_101_is_visible(const void *data, enum hwmon_sensor_types type,
					u32 attr, int channel)
{
	if (type != hwmon_temp)
		return 0;

	pr_info("%s executing [hwmon_type]=%d, [channel]=%d\n",
			__func__, type, channel);

	switch (attr) {
	case hwmon_temp_input:
		return 0444;  // Permissions.
	default:
		return 0;
	}
}

static int hwmon_101_read(struct device *dev, enum hwmon_sensor_types type,
			u32 attr, int channel, long *temp)
{
	long result;

	pr_info("%s executing [channel]=%d\n", __func__, channel);

	switch (attr) {
	case hwmon_temp_input:
		result = extract_temp(channel);
		if (result < 0)
			return result;

		*temp = result - SENSOR_BIAS;
		break;
	default:
		return -EOPNOTSUPP;
	}

	return 0;
}

static const struct hwmon_ops hwmon_101_ops = {
	.is_visible = hwmon_101_is_visible,
	.read = hwmon_101_read,
};

static const struct hwmon_channel_info *hwmon_101_info[] = {
	HWMON_CHANNEL_INFO(temp,
			HWMON_T_INPUT,
			HWMON_T_INPUT,
			HWMON_T_INPUT,
			HWMON_T_INPUT,
			HWMON_T_INPUT,
			HWMON_T_INPUT,
			HWMON_T_INPUT,
			HWMON_T_INPUT),
	NULL
};

static const struct hwmon_chip_info hwmon_101_chip_info = {
	.ops = &hwmon_101_ops,
	.info = hwmon_101_info,
};

static int hwmon_101_probe(struct platform_device *pdev)
{
	struct device *hwmon_dev;

	pr_info("%s executing.", __func__);

	hwmon_dev = devm_hwmon_device_register_with_info(
			&pdev->dev, "raspberry_sensors", NULL, &hwmon_101_chip_info, NULL);
	if (IS_ERR(hwmon_dev))
		return PTR_ERR(hwmon_dev);

	dev_info(&pdev->dev, "raspberry_sensors_registered: %s\n", dev_name(hwmon_dev));

	return 0;
}

static int  __exit hwmon_101_remove(struct platform_device *pdev)
{
	pr_info("%s: Removing", __func__);

	// It is not necessary to unregistered manually.
	// https://elixir.bootlin.com/linux/latest/source/drivers/hwmon/hwmon.c#L1035
	// devm_hwmon_device_unregister(&pdev->dev);

	return 0;
}

/* Declare a list of devices supported by the driver */
static const struct of_device_id hwmon_101_ids[] = {
		{ .compatible = "brcm,bcm2837,hwmon_101_dev" },
		{}, // Needs to finish with a NULL entry.
};

MODULE_DEVICE_TABLE(of, hwmon_101_ids);

static struct platform_driver hwmon_101_driver = {
	.driver = {
		.name = "hwmon_101_driver",
		.of_match_table = hwmon_101_ids,
		.owner = THIS_MODULE,
	},
	.probe = hwmon_101_probe,
	.remove = hwmon_101_remove,
};

module_platform_driver(hwmon_101_driver);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("Simple platform driver.");



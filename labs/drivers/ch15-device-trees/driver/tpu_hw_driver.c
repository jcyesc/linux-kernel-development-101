// SPDX-License-Identifier: GPL-2.0

/*
 * Platform device driver to that reads dummy configurations
 * for a Dummy Tensor ProcesSing Unit HW.
 *
 * The expected structure of the device tree for the dummy TPU is:
 *
 * / {
 *      tpu_hw_dummy {
 *          compatible = "tpu_hw,dummy", "tpu_hw,board";
 *
 *          algos-props = "logistic regression,random forests";
 *          iterations = <44>;
 *          default-state = "off";
 *      };
 *   };
 */

#define pr_fmt(fmt)  "%s: %s: " fmt, KBUILD_MODNAME, __func__

#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <linux/of.h> // Open Firmware header.
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/printk.h>

static const struct platform_device_id tpu_hw_id_table[] = {
	{ "tpu_plat_dev_id", 23 },
	{}, // Needs to finish with a NULL entry.
};
MODULE_DEVICE_TABLE(platform, tpu_hw_id_table);

// Identifies the node in the device tree.
static const struct of_device_id tpu_dt_ids[] = {
	{ .compatible = "tpu_hw,dummy" },
	{}, // Needs to finish with a NULL entry.
};
MODULE_DEVICE_TABLE(of, tpu_dt_ids);

static int tpu_hw_probe(struct platform_device *pdev)
{
	int ret;
	const char *algos;
	const char *state;
	u32 iterations;
	const struct of_device_id *of_id;

	pr_info("Probing device");

	of_id = of_match_device(tpu_dt_ids, &pdev->dev);
	if (!of_id) {
		pr_info("The node was not found in DTB");
		return -ENODEV;
	}

	ret = of_property_read_string(pdev->dev.of_node, "algos-props", &algos);
	if (ret < 0) {
		pr_info("Failed to read 'algos-props'");
		return ret;
	}

	ret = of_property_read_string(pdev->dev.of_node, "default-state", &state);
	if (ret < 0) {
		pr_info("Failed to read 'default-state'");
		return ret;
	}

	ret = of_property_read_u32_index(pdev->dev.of_node, "iterations", 0, &iterations);
	if (ret < 0) {
		pr_info("Failed to read 'iterations'");
		return ret;
	}

	pr_info("algos-props = %s", algos);
	pr_info("default-state = %s", state);
	pr_info("iterations = %d", iterations);

	return 0;
}

static int tpu_hw_remove(struct platform_device *pdev)
{
	pr_info("Removing device");

	return 0;
}

static void tpu_hw_shutdown(struct platform_device *pdev)
{
	pr_info("Shutdown device");
}

static int tpu_hw_suspend(struct platform_device *pdev, pm_message_t state)
{
	pr_info("Suspending device");

	return 0;
}

static int tpu_hw_resume(struct platform_device *pdev)
{
	pr_info("Resuming device");

	return 0;
}

static struct platform_driver tpu_hw_driver = {
	.driver = {
		.name = "tpu_hw_driver",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(tpu_dt_ids),
	},
	.probe = tpu_hw_probe,
	.remove = tpu_hw_remove,
	.suspend = tpu_hw_suspend,
	.shutdown = tpu_hw_shutdown,
	.resume = tpu_hw_resume,
	.id_table = tpu_hw_id_table,
	.prevent_deferred_probe = false,
	.driver_managed_dma = false,
};

module_platform_driver(tpu_hw_driver);

MODULE_AUTHOR("Juan Yescas");
MODULE_DESCRIPTION("TPU HW Platform driver");
MODULE_LICENSE("GPL v2");


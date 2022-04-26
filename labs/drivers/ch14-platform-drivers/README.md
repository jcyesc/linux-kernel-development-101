
# Chapter 14 - Platform Drivers

There are devices that use the PCI and USB busses. These devices are detected
by the kernel using the `Unified Device Model`. However, there are devices
that are memory mapped and it is not possible for the Kernel to detect them.
In order to register these devices, Linux uses `platform drivers`.

The data structure that describes the `platform device` is:

```c
struct platform_device {
	const char	*name;
	int		id;
	bool		id_auto;
	struct device	dev;
	u64		platform_dma_mask;
	struct device_dma_parameters dma_parms;
	u32		num_resources;
	struct resource	*resource;

	const struct platform_device_id	*id_entry;
	char *driver_override; /* Driver name to force a match */

	/* MFD cell pointer */
	struct mfd_cell *mfd_cell;

	/* arch specific additions */
	struct pdev_archdata	archdata;
};
```

The data structure that defines the `platform driver` is:

```c
struct platform_driver {
	int (*probe)(struct platform_device *);
	int (*remove)(struct platform_device *);
	void (*shutdown)(struct platform_device *);
	int (*suspend)(struct platform_device *, pm_message_t state);
	int (*resume)(struct platform_device *);
	struct device_driver driver;
	const struct platform_device_id *id_table;
	bool prevent_deferred_probe;
};
```

> See:
> - https://elixir.bootlin.com/linux/v5.17.4/source/include/linux/platform_device.h#L23
> - https://elixir.bootlin.com/linux/v5.17.4/source/include/linux/platform_device.h#L204

With platform drivers, it is not necessary to define an init() or exit()
function. We just need to call `module_platform_driver(struct platform_driver)`.


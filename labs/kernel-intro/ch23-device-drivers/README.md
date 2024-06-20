
# Chapter 23 - Device drivers

Devices connected to a computer system expose an IO interface to communicate with
them. The IO interfaces expose IO ports (memory registers) to manipulate the device.
The Linux operating system can read/write to system registers using the IO interfaces.

The IO interfaces can be classified on:

- Generic IO interfaces:
    - PCI
    - USB
    - Serial ports
    - Parallel ports
    - SCSI

- Custom IO interfaces
	- Keyboard interface
	- Graphic interface
	- Network interface

## Device Driver Model

The Linux kernel has a framework to control and manage the different devices
connected in the system. The framework is called `Device Driver Model`. This
framework defines structures to represent drivers, devices, buses, etc. The
drivers control the devices in the system. The main tasks of the drivers are:

- Control/Manage: control the device by using CSR (Control Status Registers).
- Power Management: configure the different voltage levels in the device's power
                    lines.
- Plug and Play: register and unregister the devices automatically during booting
                 time. The allocation of resources is transparent.
- Hot plug: register and unregister the devices automatically after the system
            boots.


The kernel defines several structs to model the devices, drivers, buses, etc.
The kernel documentation is in:

- linux-stable/Documentation/driver-api/driver-model/



### [struct device](https://elixir.bootlin.com/linux/v6.9.5/source/include/linux/device.h#L703)

The `struct device` represents a device. This struct defines things like:

- device name.
- parent of the device.
- device type.
- bus type (USB, PCI, serial, parallel, etc).
- class type.
- driver binded to this device.
- device power management fields.
- power management callbacks for suspend, hibernation, resume, etc.
- dma operations if supported.
- dma ranges if supported.
- iommu data.
- the associated device tree node.
- platform data.
- private data.

It is not very clear the distinction between device_type and class type. The class
type definition will be discussed later. The description for `struct device_type`
is below:

```
/*
 * The type of device, "struct device" is embedded in. A class
 * or bus can contain devices of different types
 * like "partitions" and "disks", "mouse" and "event".
 * This identifies the device type and carries type-specific
 * information, equivalent to the kobj_type of a kobject.
 * If "name" is specified, the uevent will contain it in
 * the DEVTYPE variable.
 */
struct device_type {
	const char *name;
	const struct attribute_group **groups;
	int (*uevent)(struct device *dev, struct kobj_uevent_env *env);
	char *(*devnode)(struct device *dev, umode_t *mode,
			 kuid_t *uid, kgid_t *gid);
	void (*release)(struct device *dev);

	const struct dev_pm_ops *pm;
};
```

### [struct device_driver](https://elixir.bootlin.com/linux/v6.9.5/source/include/linux/device/driver.h#L96)

The `struct device_driver` represents a driver that manage certain types
of devices. The main fields of `device_driver` are:


- `of_match_table` field contains the devices that are compatible with the driver.
- `probe` field, which it is a pointer to a function, it is invoked to check if a
           device can be handled by this driver.
- `remove` field, which it is a pointer to a function, it is invoked when the
           device is removed from the system.
- `shutdown` field, which it is a pointer to a function, it is invoked when
           the system is shutdown and it is necessary to shutdown the device.
- `suspend` field, which it is a pointer to a function, it is invoked when the
           device is put to sleep.
 - `resume` field, which it is a pointer to a function, it is invoked to wake
           up the device after it went to sleep.

### [struct bus_type](https://elixir.bootlin.com/linux/v6.9.5/source/include/linux/device/bus.h#L77)

A bus is a channel that connects the CPU with one or more devices. From the
point of view of the device model, all devices are connected via bus. There are
different types of buses: serial, parallel, pcie, i2c, spi, usb, etc. The type
of bus is represented using `struct bus_type`. The list of bus in Linux can
be found in:

```
 $ ls /sys/bus/
amba         container               exynos-ehld     i2c       mipi-dsi  pci-epf        sdio    typec
aoc          cpu                     exynos-etr-miu  i3c       mmc       pci_express    serial  usb
cec          dax                     exynos_info     iio       mmc_rpmb  platform       serio   usb-serial
chip-id      edac                    genpd           mdio_bus  nd        rpmsg          soc     virtio
clockevents  event_source            gpio            media     nvmem     scmi_protocol  spi     workqueue
clocksource  exynos-adv-tracer-ehld  hid             memory    pci       scsi           spmi
```

The `struct bus_type` definition is below:

```
struct bus_type {
	const char		*name;
	const char		*dev_name;
	struct device		*dev_root;
	const struct attribute_group **bus_groups;
	const struct attribute_group **dev_groups;
	const struct attribute_group **drv_groups;

	int (*match)(struct device *dev, struct device_driver *drv);
	int (*uevent)(struct device *dev, struct kobj_uevent_env *env);
	int (*probe)(struct device *dev);
	void (*sync_state)(struct device *dev);
	int (*remove)(struct device *dev);
	void (*shutdown)(struct device *dev);

	int (*online)(struct device *dev);
	int (*offline)(struct device *dev);

	int (*suspend)(struct device *dev, pm_message_t state);
	int (*resume)(struct device *dev);

	int (*num_vf)(struct device *dev);

	int (*dma_configure)(struct device *dev);

	const struct dev_pm_ops *pm;

	const struct iommu_ops *iommu_ops;

	struct subsys_private *p;
	struct lock_class_key lock_key;

	bool need_parent_lock;
};
```

### [struct class](https://elixir.bootlin.com/linux/v6.9.5/source/include/linux/device/class.h#L50)

The `struct class` describes the type of device, like audio, gps, network, pci,
etc. The definition of the struct is below:

```
/*
 * A class is a higher-level view of a device that abstracts out low-level
 * implementation details. Drivers may see a SCSI disk or an ATA disk, but,
 * at the class level, they are all simply disks. Classes allow user space
 * to work with devices based on what they do, rather than how they are
 * connected or how they work.
 */
struct class {
	const char		*name;
	struct module		*owner;

	const struct attribute_group	**class_groups;
	const struct attribute_group	**dev_groups;
	struct kobject			*dev_kobj;

	int (*dev_uevent)(struct device *dev, struct kobj_uevent_env *env);
	char *(*devnode)(struct device *dev, umode_t *mode);

	void (*class_release)(struct class *class);
	void (*dev_release)(struct device *dev);

	int (*shutdown_pre)(struct device *dev);

	const struct kobj_ns_type_operations *ns_type;
	const void *(*namespace)(struct device *dev);

	void (*get_ownership)(struct device *dev, kuid_t *uid, kgid_t *gid);

	const struct dev_pm_ops *pm;

	struct subsys_private *p;
};
```


### [struct kobject](https://elixir.bootlin.com/linux/v6.9.5/source/include/linux/kobject.h#L64)

When we need to expose in the `sysfs` the devices, drivers, buses, classes, etc
available in the Linux kernel, we use `struct kobject`.

```
struct kobject {
	const char		*name;
	struct list_head	entry;
	struct kobject		*parent;
	struct kset		*kset;
	struct kobj_type	*ktype;
	struct kernfs_node	*sd; /* sysfs directory entry */
	struct kref		kref;
#ifdef CONFIG_DEBUG_KOBJECT_RELEASE
	struct delayed_work	release;
#endif
	unsigned int state_initialized:1;
	unsigned int state_in_sysfs:1;
	unsigned int state_add_uevent_sent:1;
	unsigned int state_remove_uevent_sent:1;
	unsigned int uevent_suppress:1;
};
```

See:

 - [Documentation/core-api/kobject.rst](https://elixir.bootlin.com/linux/v6.9.5/source/Documentation/core-api/kobject.rst)
 - [Samples](https://elixir.bootlin.com/linux/v6.9.5/source/samples/kobject)

### [struct kset](https://elixir.bootlin.com/linux/v6.9.5/source/include/linux/kobject.h#L168)

The `struct kset` contains a group of `kobjects`. In `sysfs`, it helps to
organize the directory structure. For example, the directory structure
`/sys/bus/i2c/devices/1-0029 `, could be represent like:


```
              kset (bus)
          /       |          \
   kset(usb)   kset (i2c)    kset (pci)
                  |
            kset (devices)
                  |
           kobject (1-0029)
                  |
        attribute_group (configs)
                  |
          attribute (name)
```

### dev_t, major and minor numbers

Every device has a unique device number stored in `dev_t`. The device
number, `dev_t`, has 2 parts, a major and manir part. The major part
consits of the 12 MSB, and the minor part consists of the 20 LSBs. The
[MAJOR and MINOR](https://elixir.bootlin.com/linux/v6.9.5/source/include/linux/kdev_t.h#L10
)
macros are provided to extract the major and minor numbers.

### udev and device nodes

`udev` is a userspace application that scans the `/sys/class` directory
to find the device numbers for the different devices. For every device
number, it creates a `device node` in `/dev`. A device node is the way
that user space communicate with devices. The device node, usually
under `/dev`, is accessed by user-space programs. The user-space programs
send requests to the device by calling `open()`, `close()`, `read()`,
`write()`, `ioctl()`, `lseek()`, etc.

There are devices that could be hot plugged later or modules that are
loaded after boot time. For this cases, the kernel generates a `uevent`
that listens `udev`. `udev` proceeds the `uevent` and creates a `device node`
in `/dev`.

### sysfs

The `sysfs` is a temporary file system that is not stored in any hard
disk. The `sysfs` contains several directories related to the system:

 - block
 - bus
 - class
 - dev
 - devices
 - firmware
 - fs
 - hypervisor
 - kernel
 - module
 - power

Next, we will show the content of directories under `/sys`.

To find the cpus in the system, we can run:

```
 $ ls /sys/bus/cpu/devices/
cpu0  cpu1  cpu2  cpu3  cpu4  cpu5  cpu6  cpu7


 # Print the arch of the cpu.
 $ for c in /sys/bus/cpu/devices/cpu*; do echo $c; cat $c/of_node/compatible; echo ; done
/sys/bus/cpu/devices/cpu0
arm,armv9
/sys/bus/cpu/devices/cpu1
arm,armv9
/sys/bus/cpu/devices/cpu2
arm,armv9
/sys/bus/cpu/devices/cpu3
arm,armv9
/sys/bus/cpu/devices/cpu4
arm,armv9
/sys/bus/cpu/devices/cpu5
arm,armv9
/sys/bus/cpu/devices/cpu6
arm,armv9
/sys/bus/cpu/devices/cpu7
arm,armv9
/sys/bus/cpu/devices/cpu8
arm,armv9
```

The memory ranges defined in the device tree in this way:

```
	memory@80000000 {
		device_type = "memory";
		reg = <0x80000000 0x900000>;
	};
```

can be found in `/sys/firmware/devicetree/base/memory@<start addr>/reg`

```
 $ xxd /sys/firmware/devicetree/base/memory@80000000/reg
 00000000: 0000 0000 8000 0000 0090 0000
```

To find the chip-id, we can run

```
# cat /sys/devices/system/chip-id/evt_ver
EVT0.1
```

To find all devices that depend on the `iommu`, run:

```
 $ for i in /sys/class/iommu/*.sysmmu;do echo $i; cat $i/device/of_node/compatible; echo; done
/sys/class/iommu/101c0000.sysmmu
samsung,sysmmu-v9
/sys/class/iommu/11040000.sysmmu
samsung,sysmmu-v9
/sys/class/iommu/16440000.sysmmu
samsung,sysmmu-v9
/sys/class/iommu/16c40000.sysmmu
samsung,sysmmu-v9
/sys/class/iommu/19090000.sysmmu
samsung,sysmmu-v9
/sys/class/iommu/19840000.sysmmu
samsung,sysmmu-v9
```

The device tree is under `/sys/firmare`. To print the device tree, we can
run:

```
 # dtc -I fs /sys/firmware/
```

### Device types

There are 3 types of device :

- `Character device drivers`: They handle hardware devices that stream bytes such
serial devices, parallel devices, etc. The basic operations of these drivers
are: `open`, `close`, `read` and `write`.

- `Block device drivers`: They handle block hardware devices such as hard disk,
ssd cards, etc. The transmission of data is done by blocks.

- `Network device drivers`: They handle hardware devices that transmit packets
of data, not stream or blocks of data. These drivers are mainly used to control
network cards.

### Direct Memory Access (DMA)

There are devices that need to transfer large amounts of data from memory
to device memory and viceversa. In order to not involve the CPU, the SoC contains
DMA devices that are in charge of this transfer. The devices that use DMA are
Network cards, sound cards, video cards, cameras, etc.

### Status of devices

The status of devices can be checked by these 2 methods:

- polling: read the Control Status Registers (CSRs).
- interrupts: configure the device to trigger an interrupt when something changes.


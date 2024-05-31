
# How to add kernel includes in device tree sources

There could be a situation where configurations or constants in
`linux-stable/linux/include` need to be used in `dts`. By default the dtc
compiler only searches for includes in `scripts/dtc/include-prefixes`.

One way to include other directories, is to modify the `DTC_INCLUDE`
variable in `linux-stable/scripts/Makefile.lib`. This variable contains the
directories that can be used in dts or dtsi files.

For example, the patch ``add-kernel-includes-in-dts.patch` makes possible to
include `<linux/kconfig.h>` in dts and dtsi files. It makes this possible by:

- creating the symlink `scripts/dtc/include-prefixes/linux/kconfig.h`
that points to `<linux/kconfig.h`.

- modifying DTC_INCLUDE variable to include the generated file
`include/generated/autoconf.h`, which it is included in `<linux/kconfig.h>`

## include/generated/autoconf.h

Once the kernel is configured and built, several files are generated, among them
is `include/generated/autoconf.h`, which it is included in `<linux/kconfig.h>`.
The `include/generated/autoconf.h` file contains macros that refers to
the selected kernel configurations. For example:

```
/*
 * Automatically generated file; DO NOT EDIT.
 * Linux/arm64 6.6.14 Kernel Configuration
 */
#define CONFIG_COMMON_CLK_SCMI 1
#define CONFIG_HAVE_ARCH_SECCOMP_FILTER 1
#define CONFIG_SND_PROC_FS 1
#define CONFIG_VFIO_PCI_MMAP 1
#define CONFIG_SCSI_DMA 1
#define CONFIG_NETFILTER_FAMILY_BRIDGE 1
#define CONFIG_SENSORS_ARM_SCMI 1
#define CONFIG_SPI_QCOM_QSPI_MODULE 1
#define CONFIG_CC_HAS_SANCOV_TRACE_PC 1
#define CONFIG_DEFAULT_INIT ""
#define CONFIG_ARM64_PAGE_SHIFT 12
#define CONFIG_ARM_MEDIATEK_CPUFREQ_HW_MODULE 1
#define CONFIG_SOUNDWIRE_MODULE 1
```

This macros can be used in the source code to verify whether a feature is enable
or not:

```
#if IS_ENABLED(CONFIG_SOUNDWIRE_MODULE)

do_something();

#endif
```

See:

- https://elixir.bootlin.com/linux/v5.17.4/source/include/linux/kconfig.h#L73

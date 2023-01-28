// SPDX-License-Identifier: GPL-2.0

#include "arm64_mm.h"

#include <linux/printk.h>
#include <linux/types.h>

// Physical Address Range, bits [3:0] in aa64mmfr0_el1
// https://developer.arm.com/documentation/ddi0601/2022-03/AArch64-Registers/ID-AA64MMFR0-EL1--AArch64-Memory-Model-Feature-Register-0
#define	PA_RANGE_32BITS 0x0 //   4GB
#define	PA_RANGE_36BITS 0x1 //  64GB
#define	PA_RANGE_40BITS 0x2 //   1TB
#define	PA_RANGE_42BITS 0x3 //   4TB
#define	PA_RANGE_44BITS 0x4 //  16TB
#define	PA_RANGE_48BITS 0x5 // 256TB
#define	PA_RANGE_52BITS 0x6 //   4PB
#define PA_RANGE(aa64mmfr0) (aa64mmfr0 & 0xF)

// TGran4, bits [31:28] in aa64mmfr0_el1
#define TGRAN4_SUPPORTED                 0x0
#define TGRAN4_SUPPORTS_52_BIT_ADDRESSES 0x1
#define TGRAN4_NOT_SUPPORTED             0xF
#define TGRAN4(aa64mmfr0) ((aa64mmfr0 >> 28) & 0xF)

// TGran64, bits [27:24] in aa64mmfr0_el1
#define TGRAN64_SUPPORTED                 0x0
#define TGRAN64_NOT_SUPPORTED             0xF
#define TGRAN64(aa64mmfr0) ((aa64mmfr0 >> 24) & 0xF)

// TGran16, bits [23:20] in aa64mmfr0_el1
#define TGRAN16_SUPPORTED                 0x1
#define TGRAN16_SUPPORTS_52_BIT_ADDRESSES 0x2
#define TGRAN16_NOT_SUPPORTED             0x0
#define TGRAN16(aa64mmfr0) ((aa64mmfr0 >> 20) & 0xF)

static void pr_tgranule4k(uint64_t aa64mmfr0_el1)
{
	int tgran4 = TGRAN4(aa64mmfr0_el1);

	switch (tgran4) {
	case TGRAN4_SUPPORTED:
		pr_info("\t\t  4k granule supported");
		break;
	case TGRAN4_SUPPORTS_52_BIT_ADDRESSES:
		pr_info("\t\t  4k granule, supports 52bit addresses");
		break;
	case TGRAN4_NOT_SUPPORTED:
		pr_info("\t\t  4k granule not supported");
		break;
	default:
		pr_info("Unknown TGRAN4k value");
	}
}

static void pr_tgranule16k(uint64_t aa64mmfr0_el1)
{
	int tgran16 = TGRAN16(aa64mmfr0_el1);

	switch (tgran16) {
	case TGRAN16_SUPPORTED:
		pr_info("\t\t 16k granule supported");
		break;
	case TGRAN16_SUPPORTS_52_BIT_ADDRESSES:
		pr_info("\t\t 16k granule supports 52bit addresses");
		break;
	case TGRAN16_NOT_SUPPORTED:
		pr_info("\t\t 16k granule not supported");
		break;
	default:
		pr_info("Unknown TGRAN16k value");
	}
}

static void pr_tgranule64k(uint64_t aa64mmfr0_el1)
{
	int tgran64 = TGRAN64(aa64mmfr0_el1);

	switch (tgran64) {
	case TGRAN64_SUPPORTED:
		pr_info("\t\t 64k granule supported");
		break;
	case TGRAN64_NOT_SUPPORTED:
		pr_info("\t\t 64k granule not supported");
		break;
	default:
		pr_info("Unknown TGRAN64k value");
	}
}

static void pr_tgranules(uint64_t aa64mmfr0_el1)
{
	pr_tgranule4k(aa64mmfr0_el1);
	pr_tgranule16k(aa64mmfr0_el1);
	pr_tgranule64k(aa64mmfr0_el1);
}

static void pr_pa_range(int pa_range)
{
	switch (pa_range) {
	case PA_RANGE_32BITS:
		pr_info("\t\t Physical Address range 32bits (4GB)");
		break;
	case PA_RANGE_36BITS:
		pr_info("\t\t Physical Address range 36bits (64GB)");
		break;
	case PA_RANGE_40BITS:
		pr_info("\t\t Physical Address range 40bits (1TB)");
		break;
	case PA_RANGE_42BITS:
		pr_info("\t\t Physical Address range 42bits (4TB)");
		break;
	case PA_RANGE_44BITS:
		pr_info("\t\t Physical Address range 44bits (16TB)");
		break;
	case PA_RANGE_48BITS:
		pr_info("\t\t Physical Address range 46bits (256TB)");
		break;
	case PA_RANGE_52BITS:
		pr_info("\t\t Physical Address range 52bits (4PB");
		break;
	default:
		pr_info("Unknown Physical Address range");
		break;
	}
}

static void pr_supported_pa_range(uint64_t aa64mmfr0_el1)
{
	int pa_range = PA_RANGE(aa64mmfr0_el1);

	pr_pa_range(pa_range);
}

/*
 * Prints the AArch64 memory model feature register 0.
 */
void mm_print_aa64mmfr0_el1_reg(void)
{
	uint64_t aa64mmfr0_el1;

	asm volatile ("mrs %0, id_aa64mmfr0_el1" : "=r" (aa64mmfr0_el1));
	pr_info("id_aa64mmfr0_el1=0x%llx\n", aa64mmfr0_el1);

	pr_supported_pa_range(aa64mmfr0_el1);
	pr_tgranules(aa64mmfr0_el1);
}

/*
 * Prints the system control register for EL1.
 */
void mm_print_sctlr_el1_reg(void)
{
	uint64_t sctlr_el1;

	asm volatile ("mrs %0, sctlr_el1" : "=r" (sctlr_el1));
	pr_info("sctlr_el1=0x%llx\n", sctlr_el1);

	if (sctlr_el1 & 0x1)
		pr_info("\t\t EL0 & EL1 stage 1 address translation enabled.");

	if (sctlr_el1 & 0x4)
		pr_info("\t\t Data accesses to Normal memory from EL0 & EL1 are cacheable.");

	if (sctlr_el1 & 0x1000)
		pr_info("\t\t Instruction accesses to Normal memory from EL0 & EL1 are cacheable.");

	if (sctlr_el1 & 0x8)
		pr_info("\t\t SP has to be aligned to 16-byte boundary in EL1.");
}

static void pr_granule_ttbr1_el1(int granule_size)
{
	switch (granule_size) {
	case 0x1:
		pr_info("\t\t\t 16k granule configured");
		break;
	case 0x2:
		pr_info("\t\t\t 4k granule configured");
		break;
	case 0x3:
		pr_info("\t\t\t 64k granule configured");
		break;
	default:
		pr_info("\t\t\t Unknown granule");
		break;
	}
}

static void pr_granule_ttbr0_el1(int granule_size)
{
	switch (granule_size) {
	case 0x0:
		pr_info("\t\t\t 4k granule configured");
		break;
	case 0x1:
		pr_info("\t\t\t 64k granule configured");
		break;
	case 0x2:
		pr_info("\t\t\t 16k granule configured");
		break;
	default:
		pr_info("\t\t\t Unknown granule");
		break;
	}
}

/*
 * Prints the table control register for EL1.
 *
 * https://developer.arm.com/documentation/ddi0595/2021-06/AArch64-Registers/TCR-EL1--Translation-Control-Register--EL1-
 */
void mm_print_tcr_el1_reg(void)
{
	int pa_range;
	int granule_ttbr0_el1;
	int granule_ttbr1_el1;
	int t1sz;
	int t0sz;
	uint64_t tcr_el1;

	asm volatile ("mrs %0, tcr_el1" : "=r" (tcr_el1));
	pr_info("tcr_el1=0x%llx\n", tcr_el1);

	pa_range = (tcr_el1 >> 32) & 0x7;
	pr_info("\t\t Intermediate Physical Address (max output address size):");
	pr_pa_range(pa_range);

	granule_ttbr1_el1 = (tcr_el1 >> 30) & 0x3;
	pr_info("\t\t Granule Size for ttbr1_el1");
	pr_granule_ttbr1_el1(granule_ttbr1_el1);

	// The region size is 2(64-T1SZ) bytes.
	t1sz = (tcr_el1 >> 16) & 0x1F;
	pr_info("\t\t The region size for ttbr1_el1 is 2^%d", (64 - t1sz));

	granule_ttbr0_el1 = (tcr_el1 >> 14) & 0x3;
	pr_info("\t\t Granule Size for ttbr0_el1");
	pr_granule_ttbr0_el1(granule_ttbr0_el1);

	// The region size is 2(64-T0SZ) bytes.
	t0sz = tcr_el1 & 0x1F;
	pr_info("\t\t The region size for ttbr0_el1 is 2^%d", (64 - t0sz));
}


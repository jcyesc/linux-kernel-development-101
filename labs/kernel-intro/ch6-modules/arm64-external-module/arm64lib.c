// SPDX-License-Identifier: GPL-2.0

#include "arm64lib.h"

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
		pr_info("\t\t TGRAN4 supported");
		break;
	case TGRAN4_SUPPORTS_52_BIT_ADDRESSES:
		pr_info("\t\t TGRAN4 supports 52bit addresses");
		break;
	case TGRAN4_NOT_SUPPORTED:
		pr_info("\t\t TGRAN4 not supported");
		break;
	default:
		pr_info("Unknown TGRAN4");
	}
}

static void pr_tgranule16k(uint64_t aa64mmfr0_el1)
{
	int tgran16 = TGRAN16(aa64mmfr0_el1);

	switch (tgran16) {
	case TGRAN16_SUPPORTED:
		pr_info("\t\t TGRAN16 supported");
		break;
	case TGRAN16_SUPPORTS_52_BIT_ADDRESSES:
		pr_info("\t\t TGRAN16 supports 52bit addresses");
		break;
	case TGRAN16_NOT_SUPPORTED:
		pr_info("\t\t TGRAN16 not supported");
		break;
	default:
		pr_info("Unknown TGRAN16");
	}
}

static void pr_tgranule64k(uint64_t aa64mmfr0_el1)
{
	int tgran64 = TGRAN64(aa64mmfr0_el1);

	switch (tgran64) {
	case TGRAN64_SUPPORTED:
		pr_info("\t\t TGRAN64 supported");
		break;
	case TGRAN64_NOT_SUPPORTED:
		pr_info("\t\t TGRAN64 not supported");
		break;
	default:
		pr_info("Unknown TGRAN64");
	}
}

static void pr_tgranules(uint64_t aa64mmfr0_el1)
{
	pr_tgranule4k(aa64mmfr0_el1);
	pr_tgranule16k(aa64mmfr0_el1);
	pr_tgranule64k(aa64mmfr0_el1);
}

static void pr_pa_range(uint64_t aa64mmfr0_el1)
{
	int pa_range = PA_RANGE(aa64mmfr0_el1);

	switch (pa_range) {
	case PA_RANGE_32BITS:
		pr_info("\t\t PA range 32bits (4GB)");
		break;
	case PA_RANGE_36BITS:
		pr_info("\t\t PA range 36bits (64GB)");
		break;
	case PA_RANGE_40BITS:
		pr_info("\t\t PA range 40bits (1TB)");
		break;
	case PA_RANGE_42BITS:
		pr_info("\t\t PA range 42bits (4TB)");
		break;
	case PA_RANGE_44BITS:
		pr_info("\t\t PA range 44bits (16TB)");
		break;
	case PA_RANGE_48BITS:
		pr_info("\t\t PA range 46bits (256TB)");
		break;
	case PA_RANGE_52BITS:
		pr_info("\t\t PA range 52bits (4PB");
		break;
	default:
		pr_info("Unknown PA range");
		break;
	}
}

/*
 * Prints the AArch64 memory model feature register 0.
 */
void pr_aa64mmfr0_el1_reg(void)
{
	uint64_t aa64mmfr0_el1;

	asm volatile ("mrs %0, id_aa64mmfr0_el1" : "=r" (aa64mmfr0_el1));
	pr_info("ID_AA64MMFR0_EL1=0x%llx\n", aa64mmfr0_el1);

	pr_pa_range(aa64mmfr0_el1);
	pr_tgranules(aa64mmfr0_el1);
}

/*
 * Prints the cache level ID register (CLIDR_EL1). The CLIDR_EL1 Identifies the
 * type of cache, or caches, that are implemented at each level and can be
 * managed using the architected cache maintenance instructions that operate by
 * set/way, up to a maximum of seven levels. Also identifies the Level of
 * Coherence (LoC) and Level of Unification (LoU) for the cache hierarchy.
 *
 * https://developer.arm.com/documentation/ddi0595/2021-03/AArch64-Registers/CLIDR-EL1--Cache-Level-ID-Register
 */
void pr_clidr_el1(void)
{
	uint64_t clidr_el1;
	int level;

	asm volatile ("mrs %0, clidr_el1" : "=r" (clidr_el1));

	pr_info("CLIDR_EL1=0x%llx\n", clidr_el1);

	// The 21st LSBs contain the 7 cache levels information. 3 bits per level.
	// An ARMv8 CPU could have 7 cache levels.
	for (level = 1; level <= 7; level++) {
		uint64_t ctype = (clidr_el1 >> 3 * (level - 1)) & 7;

		switch (ctype) {
		case 0b000:
			pr_info("\t\t No cache at level %d", level);
			break;
		case 0b001:
			pr_info("\t\t Instruction cache only at level %d", level);
			break;
		case 0b010:
			pr_info("\t\t Data cache only at level %d", level);
			break;
		case 0b011:
			pr_info("\t\t Separate instruction and data cache at level %d", level);
			break;
		case 0b100:
			pr_info("\t\t Unified cache at level %d", level);
			break;
		default:
			pr_info("\t\t Invalid cached type");
			break;
		}

	}

	pr_info("\t\t Level of Coherency (LoC) at level %llu", (clidr_el1 >> 24) & 7); // Bits[26:24] LoC
}


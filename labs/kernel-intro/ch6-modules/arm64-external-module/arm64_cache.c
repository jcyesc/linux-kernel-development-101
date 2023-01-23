// SPDX-License-Identifier: GPL-2.0

#include "arm64_cache.h"

#include <linux/printk.h>
#include <linux/types.h>

/*
 * Returns the CCSIDR_EL1 register value that contains cache information
 * such as:
 *
 * - Line size
 * - # sets
 * - # ways
 *
 * We use the CSSELR_EL1 register to select the cache level and cache type.
 *
 * @param level Select the cache level 1..7, where the value 0 means level 1.
 * @param select_catch_type Selects the cache type (0 data cache/unified,
 *        1 instruction cache).
 */
uint64_t cache_get_ccsidr_el1_value(enum cache_level level,
		enum cache_type select_cache_type)
{
	uint64_t ccsidr_el1;
	uint64_t csselr_el1;

	// Bits [3:1] for level, Bit [0] for cache type.
	csselr_el1 = (level << 1) | select_cache_type;
	asm volatile ("msr csselr_el1, %0; isb" : : "r" (csselr_el1));
	asm volatile ("mrs %0, ccsidr_el1" : "=r" (ccsidr_el1));

	return ccsidr_el1;
}

/*
 * Returns the cache line size that is found in CCSIDR_EL1 in bits [2:0].
 *
 * See https://developer.arm.com/documentation/ddi0595/2021-06/AArch64-Registers/CCSIDR-EL1--Current-Cache-Size-ID-Register
 */
uint32_t cache_get_line_size_in_bytes(uint64_t ccsidr_el1)
{
	// line_size entry = (Log2(line size)) - 4
	// For example:
	//     line size entry = 0 = log2(16) - 4
	//     line size entry = 1 = log2(32) - 4
	uint32_t line_size_entry = ccsidr_el1 & 7;

	return 16 << line_size_entry;
}

/*
 * Returns the number of ways in the cache. This is found in CCSIDR_EL1 in
 * bits [12:3].
 */
uint32_t cache_get_num_ways(uint64_t ccsidr_el1)
{
	uint32_t ways = (uint32_t) (ccsidr_el1 >> 3) & 0x3FF;

	return ways + 1; // As per ARM documentation, we add 1.
}

/*
 * Returns the number of sets in the cache. This is found in CCSIDR_EL1 in
 * bits [27:13].
 */
uint32_t cache_get_num_sets(uint64_t ccsidr_el1)
{
	uint32_t sets = (uint32_t) (ccsidr_el1 >> 13) & 0x7FFF;

	return sets + 1; // Add 1, as per ARM documentation.
}

/*
 * Prints the cache info at the provided level and cache type.
 *
 * @param level Selects the cache level 1..7, where the value 0 means level 1.
 * @param ctype Selects the cache type (0 data cache/unified,
 *        1 instruction cache).
 */
static void cache_print_info(enum cache_level level, enum cache_type ctype)
{
	uint64_t ccsidr_el1;
	uint32_t cache_size;
	uint32_t line_size;
	uint32_t num_ways;
	uint32_t num_sets;

	ccsidr_el1 = cache_get_ccsidr_el1_value(level, ctype);
	line_size = cache_get_line_size_in_bytes(ccsidr_el1);
	num_ways = cache_get_num_ways(ccsidr_el1);
	num_sets = cache_get_num_sets(ccsidr_el1);
	cache_size = line_size * num_ways * num_sets;

	pr_info("\t\t\t cache size: %d bytes", cache_size);
	pr_info("\t\t\t Line size:  %d bytes", line_size);
	pr_info("\t\t\t num ways:   %d", num_ways);
	pr_info("\t\t\t num sets:   %d", num_sets);
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
void cache_print_clidr_el1(void)
{
	uint64_t clidr_el1;
	int level;

	asm volatile ("mrs %0, clidr_el1" : "=r" (clidr_el1));

	pr_info("CLIDR_EL1=0x%llx\n", clidr_el1);

	// The 21st LSBs contain the 7 cache levels information. 3 bits per level.
	// An ARMv8 CPU could have 7 cache levels.
	for (level = 0; level < 7; level++) {
		uint64_t ctype = (clidr_el1 >> 3 * (level)) & 7;
		int pr_level = level + 1;

		switch (ctype) {
		case 0b000:
			pr_info("\t\t Level %d: No cache", pr_level);
			break;
		case 0b001:
			pr_info("\t\t Level %d: Instruction cache only", pr_level);
			cache_print_info(level, INSTRUCTION);
			break;
		case 0b010:
			pr_info("\t\t Level %d: Data cache only", pr_level);
			cache_print_info(level, DATA_OR_UNIFIED);
			break;
		case 0b011:
			pr_info("\t\t Level %d: Separate instruction and data cache", pr_level);
			pr_info("\t\t\t Instruction cache");
			cache_print_info(level, INSTRUCTION);
			pr_info("\t\t\t Data cache");
			cache_print_info(level, DATA_OR_UNIFIED);
			break;
		case 0b100:
			pr_info("\t\t Level %d: Unified cache", pr_level);
			cache_print_info(level, DATA_OR_UNIFIED);
			break;
		default:
			pr_info("\t\t Level %d: Invalid cached type", pr_level);
			break;
		}
	}

	// Bits[26:24] LoC
	pr_info("\t\t Level of Coherency (LoC) at level %llu", (clidr_el1 >> 24) & 7);
}


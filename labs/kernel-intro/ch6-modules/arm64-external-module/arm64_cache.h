/* SPDX-License-Identifier: GPL-2.0 */
/*
 * ARM library to print cache information.
 */

#ifndef ARM64_CACHE_H_
#define ARM64_CACHE_H_

#include <linux/types.h>

/*
 * Cache levels supported in ARMv8 architecture.
 */
enum cache_level {
	LEVEL_1 = 0,
	LEVEL_2,
	LEVEL_3,
	LEVEL_4,
	LEVEL_5,
	LEVEL_6,
	LEVEL_7,
};

/*
 * Cache types supported in ARMv8 architecture.
 */
enum cache_type {
	DATA_OR_UNIFIED,
	INSTRUCTION,
};

void cache_print_clidr_el1(void);

uint64_t cache_get_ccsidr_el1_value(enum cache_level level,
	enum cache_type select_cache_type);

uint32_t cache_get_line_size_in_bytes(uint64_t ccsidr_el1);

uint32_t cache_get_num_ways(uint64_t ccsidr_el1);

uint32_t cache_get_num_sets(uint64_t ccsidr_el1);

#endif // ARM64_CACHE_H_


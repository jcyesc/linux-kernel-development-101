/* SPDX-License-Identifier: GPL-2.0 */
/*
 * ARM library to print system registers for:
 *   - Virtual memory info (granule size, vm address size, etc).
 *   - Cache configuration
 */

#ifndef ARM64_MM_H_
#define ARM64_MM_H_

void mm_print_aa64mmfr0_el1_reg(void);

#endif // ARM64_MM_H_

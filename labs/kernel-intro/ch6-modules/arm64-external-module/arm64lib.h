/* SPDX-License-Identifier: GPL-2.0 */
/*
 * ARM library to print system registers for:
 *   - Virtual memory info (granule size, vm address size, etc).
 *   - Cache configuration
 */

#ifndef ARM64LIB_H_
#define ARM64LIB_H_

void pr_aa64mmfr0_el1_reg(void);

void pr_clidr_el1(void);

#endif // ARM64LIB_H_


/*
 * Macros constructions
 *
 * This file show some examples of macros constructions that are found in the
 * Linux kernel source code.
 *
 * https://elixir.bootlin.com/linux/latest/source/include/linux/percpu-defs.h#L28
 *
 * #define get_cpu_var(var)          \
 * (*({                              \
 *      preempt_disable();           \
 *      this_cpu_ptr(&var);          \
 * }))
 */

#include <stdio.h>

#define deference1(var)  \
(*({                     \
	&var;                \
}))


#define deference2(var)  \
(*(                      \
	&var                 \
))

int main() {
	int value = 111;
	printf("deference1 ");
	deference1(value)++;
	printf("Value = %d\n", value);

	printf("deference2 ");
	deference2(value)++;
	printf("Value = %d\n", value);
}


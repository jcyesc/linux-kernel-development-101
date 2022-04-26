#include "thunder_cat.h"

#include <linux/kref.h>
#include <linux/printk.h>
#include <linux/string.h>

void thunder_cat_init(struct thunder_cat *td, char *name, int age)
{
	size_t len = (strlen(name) + 1) % THUNDER_CAT_NAME_SIZE;

	memcpy(td->name, name, len);
	td->name[THUNDER_CAT_NAME_SIZE - 1] = '\0';
	td->age = age;

	// Initializes the kref and set the counter to 1.
	kref_init(&td->refcount);
}

void thunder_cat_print(struct thunder_cat *td)
{
	unsigned int num_refs = kref_read(&td->refcount);
	pr_info("Thunder Cat [%s, %d] has [%d] references(s)!\n",
			td->name, td->age, num_refs);
}


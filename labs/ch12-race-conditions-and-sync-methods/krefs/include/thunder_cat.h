
#ifndef KREFS_THUNDER_CAT_H_
#define KREFS_THUNDER_CAT_H_

#include <linux/kref.h>


#define THUNDER_CAT_NAME_SIZE 256

struct thunder_cat {
	int age;
	char name[THUNDER_CAT_NAME_SIZE];
	struct kref refcount;
};

void thunder_cat_init(struct thunder_cat *td, char *name, int age);

void thunder_cat_print(struct thunder_cat *td);

#endif // KREFS_THUNDER_CAT_H_

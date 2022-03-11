
/*
 * This example shows how define typedefs for functions.
 */

#include <stdio.h>

typedef void (*error_handler_t)(int, double);

error_handler_t process(int error, error_handler_t handler) {
	handler(error, 23.56);

	return handler;
}

void custom_handler(int error, double value) {
	printf("custom_handler: error=%d, value=%f\n", error, value);
}

int main() {
	void (*handler)(int, double);
	handler = process(23, custom_handler);
	handler(3, 33.57);

	return 0;
}

/* Allocate a memory region and set a name for that region.
 *
 * This program shows:
 *
 * - how to use mmap
 * - set a name for the vm_area_struct
 * - print the vm areas using /proc/self/maps
 *
 * In order to set a name for a vm_area_struct:
 *
 * - the Linux Kernel has to be at least 5.17
 * - CONFIG_ANON_VMA_NAME=y has to be set
 *
 * To compile the program:
 *
 *   host $ aarch64-linux-gnu-g++ -o vm_anon_name vm_anon_name.cpp
 *   host $ scp -P 8022 vm_anon_name root@127.0.0.1:~
 */

#include <fstream>
#include <iostream>
#include <string>

#include <stdint.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <unistd.h>

#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

// The macros below are defined since the 5.17 Linux version.
// https://man7.org/linux/man-pages/man2/prctl.2.html
// https://elixir.bootlin.com/linux/v5.17.15/source/include/uapi/linux/prctl.h#L275
// https://elixir.bootlin.com/linux/v5.19.17/source/include/uapi/linux/prctl.h
#define PR_SET_VMA		0x53564d41
# define PR_SET_VMA_ANON_NAME		0

int main() {

	size_t size = getpagesize();
	int pid = getpid();
	const char *vm_name = "/secret/mem/area";
	void *addr;

	std::cout << "Memory map a region and print vm_area_struct\n" << "PID: "
			<< pid << std::endl;

	std::cout << "\nPress enter to mmap\n";
	std::cin.get();

	addr = mmap(nullptr, size, PROT_READ | PROT_WRITE,
			MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

	if (addr == MAP_FAILED) {
		handle_error("Failed to mmap region");
	}

	std::cout << "mmap addr: " << addr << " size: " << size << std::endl;

	std::cout << "\nPress enter to set vm_area_struct's name\n";
	std::cin.get();

	prctl(PR_SET_VMA, PR_SET_VMA_ANON_NAME, addr, size / 2, vm_name);

	uintptr_t end = (uintptr_t) addr + size;
	std::cout << "Set [" << addr << ", " << reinterpret_cast<const void*>(end)
			<< "] = " << vm_name << std::endl;

	std::cout << "\nPress enter to unmmap the region";
	std::cin.get();

	munmap(addr, size);

	std::cout << "\nPress enter to exit\n";
	std::cin.get();

	return 0;
}


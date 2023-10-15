/*
 * Process class
 *
 * This class is used to print information about the process such as:
 *
 *  - vm_area_struct regions
 *  - status
 *  - environment
 *  - command name
 *
 * To run the program, execute:
 *
 *     $  host $ aarch64-linux-gnu-g++ -std=c++11 -o process process.cpp
 *     *  host $ scp -P 8022 process root@127.0.0.1:~
 *
 *     $  target $ ./process
 */

#include <fstream>
#include <iostream>
#include <string>

#include <unistd.h>

void PrintHelper(std::string& fileName) {
	std::ifstream stream;

	std::cout << "---------------------------------------" << std::endl;
	std::cout << "Print " << fileName  << std::endl;
	std::cout << "---------------------------------------" << std::endl;

	stream.open(fileName, std::ifstream::in);
	if (!stream.is_open()) {
		std::cerr << "Failed to open the file: " << fileName << std::endl;
		exit(EXIT_FAILURE);
	}

	for (std::string line; std::getline(stream, line); ) {
		std::cout << line << std::endl;
	}

	std::cout << std::endl;

	stream.close();
}

class Process {

public:
	// Uses getpid() to get the pid of the current process.
	Process();
	Process(int pid);
	void PrintVmMaps();
	void PrintStatus();
	void PrintLimits();
	void PrintCmdline();
	void PrintMounts();
	void PrintEnviron();

private:
	int pid;
};

Process::Process() {
	pid = getpid();
}

Process::Process(int pid) : pid(pid) {
}

void Process::PrintVmMaps() {
	// The maps file for the currrent process is /proc/self/maps
	std::string fileName("/proc/" + std::to_string(pid) + "/maps");
	PrintHelper(fileName);
}

void Process::PrintStatus() {
	// The status file for the currrent process is /proc/self/status
	std::string fileName("/proc/" + std::to_string(pid) + "/status");
	PrintHelper(fileName);
}

void Process::PrintLimits() {
	// The limits file for the currrent process is /proc/self/limits
	std::string fileName("/proc/" + std::to_string(pid) + "/limits");
	PrintHelper(fileName);
}

void Process::PrintCmdline() {
	// The cmdline file for the currrent process is /proc/self/cmdline
	std::string fileName("/proc/" + std::to_string(pid) + "/cmdline");
	PrintHelper(fileName);
}

void Process::PrintMounts() {
	// The mounts file for the currrent process is /proc/self/mounts
	std::string fileName("/proc/" + std::to_string(pid) + "/mounts");
	PrintHelper(fileName);
}

void Process::PrintEnviron() {
	// The environ file for the currrent process is /proc/self/environ
	std::string fileName("/proc/" + std::to_string(pid) + "/environ");
	PrintHelper(fileName);
}

int main() {
	std::cout << "Printing Process information\n" << std::endl;

	int pid = getpid();
	Process p(pid);

	p.PrintVmMaps();
	p.PrintStatus();
	p.PrintLimits();
	p.PrintCmdline();
	p.PrintMounts();
	p.PrintEnviron();

	return 0;
}


#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

System::System() {
	kernel_ = LinuxParser::Kernel();
	os_ = LinuxParser::OperatingSystem();
}

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() {
        processes_.clear();

	for (auto& pid : LinuxParser::Pids())
		processes_.emplace_back(Process(pid));

	std::sort(processes_.begin(), processes_.end());
	return processes_;
}

std::string System::Kernel() { return kernel_; }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return os_; }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }

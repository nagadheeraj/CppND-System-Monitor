#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {
	command_ = LinuxParser::Command(pid_);
	ram_ = LinuxParser::Ram(pid_);
	user_ = LinuxParser::User(pid_);
	auto start_time = LinuxParser::UpTime(pid_) / sysconf(_SC_CLK_TCK);
	auto system_uptime = LinuxParser::UpTime();
	uptime_ = system_uptime - start_time;
}

int Process::Pid() const { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { return 0; }

string Process::Command() const { return command_; }

string Process::Ram() const { return ram_; }

string Process::User() const { return user_; }

long int Process::UpTime() const { return uptime_; }

bool Process::operator<(Process const& a) const { return UpTime() > a.UpTime(); }

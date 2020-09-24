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
}

int Process::Pid() const { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { return 0; }

string Process::Command() { return command_; }

string Process::Ram() { return ram_; }

string Process::User() { return user_; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return 0; }

bool Process::operator<(Process const& a) const { return Pid() > a.Pid(); }

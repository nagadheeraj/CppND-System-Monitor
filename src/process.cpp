#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {
  command_ = LinuxParser::Command(pid_);
  user_ = LinuxParser::User(pid_);
}

int Process::Pid() const { return pid_; }

float Process::CpuUtilization() const {
  const auto run_time = LinuxParser::ActiveJiffies(pid_) / sysconf(_SC_CLK_TCK);
  const auto sys_time = LinuxParser::UpTime();
  return static_cast<float>(run_time) / static_cast<float>(sys_time);
}

string Process::Command() const { return command_; }

string Process::Ram() const { return LinuxParser::Ram(pid_); }

string Process::User() const { return user_; }

long int Process::UpTime() const {
  const auto start_time = LinuxParser::UpTime(pid_) / sysconf(_SC_CLK_TCK);
  const auto system_uptime = LinuxParser::UpTime();
  return system_uptime - start_time;
}

bool Process::operator<(Process const& a) const {
  return CpuUtilization() > a.CpuUtilization();
}

#include <string>
#include <vector>

#include "linux_parser.h"
#include "processor.h"

using std::string;
using std::vector;

float Processor::Utilization() {
  vector<string> times = LinuxParser::CpuUtilization();
  long user = stol(times[0]);
  long nice = stol(times[1]);
  long system = stol(times[2]);
  long idle = stol(times[3]);
  long iowait = stol(times[4]);
  long irq = stol(times[5]);
  long softirq = stol(times[6]);
  long steal = stol(times[7]);
  long guest = stol(times[8]);
  long guest_nice = stol(times[9]);

  user -= guest;
  nice -= guest_nice;
  idle += iowait;
  system += irq + softirq;

  long total = user + nice + system + idle + steal + guest + guest_nice;
  long totald = total - prevTotal_;
  long idled = idle - prevIdle_;
  prevTotal_ = total;
  prevIdle_ = idle;
  return (totald - idled) / float(totald);
}

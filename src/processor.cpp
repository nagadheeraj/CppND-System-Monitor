#include <string>
#include <vector>

#include "linux_parser.h"
#include "processor.h"

using std::string;
using std::vector;

float Processor::Utilization() {
  return static_cast<float>(LinuxParser::ActiveJiffies()) / static_cast<float>(LinuxParser::Jiffies());
}

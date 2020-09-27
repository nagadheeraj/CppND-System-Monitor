#include <string>
#include <vector>

#include "linux_parser.h"
#include "processor.h"

using std::string;
using std::vector;

float Processor::Utilization() {
  return LinuxParser::ActiveJiffies() / LinuxParser::Jiffies();
}

#include <iomanip>
#include <sstream>
#include <string>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  static constexpr int SECS_PER_MIN = 60;
  static constexpr int MINS_PER_HR = 60;
  std::ostringstream ss;
  long mins = seconds / SECS_PER_MIN;
  long hrs = mins / MINS_PER_HR;

  ss << std::setw(2) << std::setfill('0') << hrs << ":";
  ss << std::setw(2) << std::setfill('0') << mins % MINS_PER_HR << ":";
  ss << std::setw(2) << std::setfill('0') << seconds % SECS_PER_MIN;
  return ss.str();
}

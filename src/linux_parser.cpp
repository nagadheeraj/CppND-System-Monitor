#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

class FileParser {
 public:
  FileParser(string filename) {
    std::ifstream stream(filename);
    if (!stream) return;
    string line;
    while (std::getline(stream, line)) lines_.emplace_back(line);
  }

  // Description: GetString(line_number, word_position)
  //
  // Returns the word if found in the given line number
  // and position of the word in that line.
  //
  // Returns:
  //	'string' value of the word if found.
  //	'Empty' string if not found.
  string GetString(unsigned int line, unsigned int word) {
    if (line < 1 || line > lines_.size()) return string();

    std::istringstream ss(lines_[line - 1]);
    vector<string> words;
    string w;
    while (ss >> w) words.emplace_back(w);

    if (word < 1 || word > words.size()) return string();

    return words[word - 1];
  }

  // Description: GetLong(line_number, word_position)
  //
  // Similar to GetString but returns long value
  //
  // Returns:
  //	'long' value of the word if found.
  //	'0' if not found.
  long GetLong(unsigned int line, unsigned int word) {
    string value = GetString(line, word);
    return value.empty() ? 0 : stol(value);
  }

  // Description: GetWords(key)
  //
  // The 'key' is the first word of a line and the
  // 'values' are the words found imediately after
  // the key on the same line.
  //
  // Returns:
  //	'vector of string' values if the key is found
  //	'Empty vector' if key is not found
  vector<string> GetWords(string target_key) {
    for (const auto& line : lines_) {
      string key;
      std::istringstream ss(line);
      ss >> key;
      if (key != target_key) continue;

      vector<string> words;
      string word;
      while (ss >> word) words.emplace_back(word);

      return words;
    }

    return {};
  }

  // Description: GetString(key)
  //
  // The 'key' is the first word of a line and the
  // 'value' is the immediate word after the key
  // on the same line.
  //
  // Returns:
  //	'string' value if the key is found.
  //	'Empty' string if the key is not found.
  string GetString(string target_key) {
    for (const auto& line : lines_) {
      string key, value;
      std::istringstream ss(line);
      ss >> key >> value;
      if (key == target_key) return value;
    }

    return string();
  }

  // Description: GetLong(key)
  //
  // Same as GetString
  //
  // Returns:
  //	'long' value if the key is found.
  //	'0L' if the key is not found.
  long GetLong(string target_key) {
    string value = GetString(target_key);
    return value.empty() ? 0 : stol(value);
  }

  // Description: GetLines()
  //
  // Returns all lines in the file
  //
  // Returns:
  //	List of lines
  const vector<string>& GetLines() const { return lines_; }

 private:
  vector<string> lines_;
};

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  FileParser parser(kProcDirectory + kMeminfoFilename);
  const long mem_total = parser.GetLong("MemTotal:");
  const long mem_free = parser.GetLong("MemFree:");
  const long buffers = parser.GetLong("Buffers:");
  long cached = parser.GetLong("Cached:");
  const long sreclaimable = parser.GetLong("SReclaimable:");
  const long shmem = parser.GetLong("Shmem:");
  long mem_used = mem_total - mem_free;

  cached += sreclaimable - shmem;
  mem_used -= buffers + cached;
  return mem_used / float(mem_total);
}

long LinuxParser::UpTime() {
  std::ifstream stream(kProcDirectory + kUptimeFilename);

  if (!stream.is_open()) return 0;

  string line;
  std::getline(stream, line);
  std::istringstream ss(line);
  long res;
  ss >> res;
  return res;
}

long LinuxParser::Jiffies() { return ActiveJiffies() + IdleJiffies(); }

long LinuxParser::ActiveJiffies(int pid) {
  FileParser parser(kProcDirectory + to_string(pid) + kStatFilename);
  constexpr auto PROC_UTIME_TIME_IDX = 14;
  constexpr auto PROC_STIME_TIME_IDX = 15;
  const auto utime = parser.GetLong(1, PROC_UTIME_TIME_IDX);
  const auto stime = parser.GetLong(1, PROC_STIME_TIME_IDX);
  return utime + stime;
}

long LinuxParser::ActiveJiffies() {
  FileParser parser(kProcDirectory + kStatFilename);
  constexpr auto PROC_USER_IDX = 2;
  constexpr auto PROC_NICE_IDX = 3;
  constexpr auto PROC_SYSTEM_IDX = 4;
  constexpr auto PROC_IRQ_IDX = 7;
  constexpr auto PROC_SOFTIRQ_IDX = 8;
  constexpr auto PROC_STEAL_IDX = 9;

  return parser.GetLong(1, PROC_USER_IDX) + parser.GetLong(1, PROC_NICE_IDX) +
         parser.GetLong(1, PROC_SYSTEM_IDX) + parser.GetLong(1, PROC_IRQ_IDX) +
         parser.GetLong(1, PROC_SOFTIRQ_IDX) +
         parser.GetLong(1, PROC_STEAL_IDX);
}

long LinuxParser::IdleJiffies() {
  FileParser parser(kProcDirectory + kStatFilename);
  constexpr auto PROC_IDLE_IDX = 5;
  constexpr auto PROC_IOWAIT_IDX = 6;

  return parser.GetLong(1, PROC_IDLE_IDX) + parser.GetLong(1, PROC_IOWAIT_IDX);
}

vector<string> LinuxParser::CpuUtilization() {
  FileParser parser(kProcDirectory + kStatFilename);
  return parser.GetWords("cpu");
}

int LinuxParser::TotalProcesses() {
  FileParser parser(kProcDirectory + kStatFilename);
  return parser.GetLong("processes");
}

int LinuxParser::RunningProcesses() {
  FileParser parser(kProcDirectory + kStatFilename);
  return parser.GetLong("procs_running");
}

string LinuxParser::Command(int pid) {
  FileParser parser(kProcDirectory + to_string(pid) + kCmdlineFilename);
  const auto& lines = parser.GetLines();

  return (lines.size() == 0) ? string() : lines[0];
}

string LinuxParser::Ram(int pid) {
  FileParser parser(kProcDirectory + to_string(pid) + kStatusFilename);
  auto ram_mb = parser.GetLong("VmSize:") / 1024;  // Convert to mb

  return to_string(ram_mb);
}

string LinuxParser::Uid(int pid) {
  FileParser parser(kProcDirectory + to_string(pid) + kStatusFilename);
  return parser.GetString("Uid:");
}

string LinuxParser::User(int pid) {
  FileParser parser(kPasswordPath);

  for (auto line : parser.GetLines()) {
    std::replace(line.begin(), line.end(), ':', ' ');
    string user, passwd, uid;
    std::istringstream ss(line);
    ss >> user >> passwd >> uid;
    if (uid == LinuxParser::Uid(pid)) return user;
  }

  return string();
}

long LinuxParser::UpTime(int pid) {
  FileParser parser(kProcDirectory + to_string(pid) + kStatFilename);
  constexpr auto PROC_START_TIME_IDX = 22;
  return parser.GetLong(1, PROC_START_TIME_IDX);
}

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
		while (std::getline(stream, line))
			lines_.emplace_back(line);
	}

	// Description:
	//
	// The 'key' is the first word of a line and the
	// 'values' are the words found imediately after
	// the key on the same line.
	//
	// Returns:
	//	'vector of string' values if the key is found
	//	'Empty vector' if key is not found
	vector<string> GetWords(string target_key) {
		for (const auto &line : lines_) {
			string key;
			std::istringstream ss(line);
			ss >> key;
			if (key != target_key)
				continue;

			vector<string> words;
			string word;
			while (ss >> word)
				words.emplace_back(word);

			return words;
		}

		return {};
	}

	// Description:
	//
	// The 'key' is the first word of a line and the
	// 'value' is the immediate word after the key
	// on the same line.
	//
	// Returns:
	//	'string' value if the key is found.
	//	'Empty' string if the key is not found.
	string GetString(string target_key) {
		for (const auto &line : lines_) {
			string key, value;
			std::istringstream ss(line);
			ss >> key >> value;
			if (key == target_key)
				return value;
		}

		return string();
	}

	// Description:
	//
	// Same as GetString
	//
	// Returns:
	//	'long' value if if the key is found.
	//	'0L' if the key is not found.
	long GetLong(string target_key) {
		string value = GetString(target_key);
		return value.empty() ? 0 : stol(value);
	}
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
	long mem_total = parser.GetLong("MemTotal:");
	long mem_free = parser.GetLong("MemFree:");
	long buffers = parser.GetLong("Buffers:");
	long cached = parser.GetLong("Cached:");
	long sreclaimable = parser.GetLong("SReclaimable:");
	long shmem = parser.GetLong("Shmem:");
	long mem_used = mem_total - mem_free;

	cached += sreclaimable - shmem;
	mem_used -= buffers + cached;
	return mem_used / float(mem_total);
}

long LinuxParser::UpTime() {
	std::ifstream stream(kProcDirectory + kUptimeFilename);

	if (!stream.is_open())
		return 0;

	string line;
	std::getline(stream, line);
	std::istringstream ss(line);
	long res;
	ss >> res;
	return res;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

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

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }

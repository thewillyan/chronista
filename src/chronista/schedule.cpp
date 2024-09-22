#include "chronista/schedule.hpp"
#include <istream>

using namespace chronista;

Schedule::Schedule() { this->schedule = std::vector<Operation>(); }

Schedule::Schedule(std::istream &input) { this->schedule = this->parse(input); }

int Schedule::size() { return this->schedule.size(); }

std::string Schedule::to_string() {
  std::string result = "";
  for (Operation operation : this->schedule) {
    result += operation.get_operation_string();
  }
  return result;
}

void Schedule::add(Operation operation) { this->schedule.push_back(operation); }

std::vector<Operation> Schedule::parse(std::istream &input) {
  std::vector<Operation> schedule;
  std::string line;
  while (std::getline(input, line)) {
    schedule.push_back(Operation(line));
  }
  return schedule;
}

std::vector<Operation> Schedule::get_schedule() { return this->schedule; }

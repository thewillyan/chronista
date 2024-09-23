#include "chronista/schedule.hpp"
#include <algorithm>
#include <iostream>
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

void Schedule::add(Operation operation) {
  this->schedule.push_back(operation);
  this->set_transaction_ids(operation.get_transaction_id());
}

std::vector<Operation> Schedule::parse(std::istream &input) {
  std::vector<Operation> schedule;
  std::string line;
  while (std::getline(input, line)) {
    schedule.push_back(Operation(line));
    this->set_transaction_ids(schedule.back().get_transaction_id());
  }
  return schedule;
}

std::vector<Operation> Schedule::get_schedule() { return this->schedule; }

void Schedule::remove_most_recent_transaction(
    std::vector<int> transaction_ids) {
  unsigned int most_recent_transaction_id = this->get_transaction_ids().back();
  for (int i = this->size() - 1; i >= 0; i--) {
    if (this->schedule[i].get_transaction_id() == most_recent_transaction_id) {
      this->schedule.erase(this->schedule.begin() + i);
    }
  }
  this->transaction_ids.pop_back();
}

void Schedule::set_transaction_ids(int transaction_id) {
  if (std::find(this->transaction_ids.begin(), this->transaction_ids.end(),
                transaction_id) == this->transaction_ids.end()) {
    this->transaction_ids.push_back(transaction_id);
  }
}

std::vector<int> Schedule::get_transaction_ids() {
  return this->transaction_ids;
}
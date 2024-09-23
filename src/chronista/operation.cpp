#include <regex>
#include <string>

#include "chronista/operation.hpp"
#include <iostream>

using namespace chronista;

Operation::Operation(std::string operation) : was_scheduled(false) {
  if (!std::regex_match(
          operation,
          std::regex("^T[0-9]+: (r|w|ul)\\([0-9]+(\\.[0-9]+){0,3}\\)$")) &&
      !std::regex_match(operation, std::regex("^T[0-9]+: c$"))) {
    throw std::invalid_argument("Invalid operation format");
  }
  std::tie(this->operation_type, this->transaction_id, this->granularity) =
      this->parse_operation(operation);
  if (this->operation_type == OperationType::Commit) {
    this->full_resource_path =
        std::vector<std::tuple<chronista::Granularity, unsigned int>>();
    this->resource = NULL;
  } else {
    this->set_full_resource_path(operation);
    this->set_resource(operation);
  }

  this->operation_string = this->to_string(operation);
}

std::tuple<OperationType, int, Granularity>
Operation::parse_operation(std::string operation) {
  std::string operation_type_token = operation.substr(
      operation.find(":") + 2, operation.find("(") - operation.find(":") - 2);
  OperationType operation_type;
  if (operation_type_token == "c") {
    operation_type = OperationType::Commit;
  } else if (operation_type_token == "r") {
    operation_type = OperationType::Read;
  } else if (operation_type_token == "w") {
    operation_type = OperationType::Write;
  } else {
    operation_type = OperationType::UpdateLock;
  }
  std::string transaction_id_token =
      operation.substr(1, operation.find(":") - 1);
  int transaction_id = std::stoi(transaction_id_token);
  std::string granularity_token = operation.substr(
      operation.find("(") + 1, operation.find(")") - operation.find("(") - 1);
  if (operation_type == OperationType::Commit) {
    return std::make_tuple(operation_type, transaction_id, Granularity::None);
  }
  int count = 0;
  for (char ch : granularity_token) {
    if (ch == '.') {
      ++count;
    }
  }
  Granularity granularity_type;
  switch (count) {
  case 0:
    granularity_type = Granularity::Database;
    break;
  case 1:
    granularity_type = Granularity::Table;
    break;
  case 2:
    granularity_type = Granularity::Page;
    break;
  case 3:
    granularity_type = Granularity::Tuple;
    break;
  }
  return std::make_tuple(operation_type, transaction_id, granularity_type);
}

std::string Operation::to_string(std::string operation) {
  std::string operation_type_string;
  std::string operation_type_token = operation.substr(
      operation.find(":") + 2, operation.find("(") - operation.find(":") - 2);
  std::string transaction_id_token =
      operation.substr(1, operation.find(":") - 1);
  if (operation_type_token == "c") {
    return operation_type_token + transaction_id_token;
  }
  std::string granularity_token = operation.substr(
      operation.find("(") + 1, operation.find(")") - operation.find("(") - 1);
  return operation_type_token + transaction_id_token + "(" + granularity_token +
         ")";
}

OperationType Operation::get_operation() const { return this->operation_type; }

int Operation::get_transaction_id() const { return this->transaction_id; }

Granularity Operation::get_granularity() const { return this->granularity; }

std::string Operation::get_operation_string() const { return this->operation_string; }

std::vector<std::tuple<chronista::Granularity, unsigned int>>
Operation::get_full_resource_path() const {
  return this->full_resource_path;
}

unsigned int Operation::get_resource() const { return this->resource; }

void Operation::set_full_resource_path(std::string operation) {
  std::string granularity_token = operation.substr(
      operation.find("(") + 1, operation.find(")") - operation.find("(") - 1);
  std::vector<std::tuple<chronista::Granularity, unsigned int>> full_resource_path;
  std::string resource_token;
  for (char ch : granularity_token) {
    if (ch == '.') {
      full_resource_path.push_back(
          std::make_tuple(static_cast<Granularity>(full_resource_path.size()),
                          std::stoi(resource_token)));
      resource_token = "";
    } else {
      resource_token += ch;
    }
  }
  full_resource_path.push_back(
      std::make_tuple(static_cast<Granularity>(full_resource_path.size()),
                      std::stoi(resource_token)));
  this->full_resource_path = full_resource_path;
}

void Operation::set_resource(std::string operation) {
  std::string granularity_token = operation.substr(
      operation.find("(") + 1, operation.find(")") - operation.find("(") - 1);
  std::string resource_token;
  for (char ch : granularity_token) {
    if (ch == '.') {
      resource_token = "";
    } else {
      resource_token += ch;
    }
  }
  this->resource = std::stoi(resource_token);
}

bool Operation::get_was_scheduled() {
  return this->was_scheduled;
}

void Operation::set_was_scheduled (bool ws) {

  this->was_scheduled = ws;

}
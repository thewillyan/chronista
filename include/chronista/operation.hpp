#ifndef CHRONISTA_OPERATION
#define CHRONISTA_OPERATION

#include <variant>
#include <vector>

namespace chronista {
enum class OperationType : unsigned int { Read, Write, Commit, UpdateLock };
enum class Granularity : unsigned int { Database, Table, Page, Tuple, None };

class Operation {
private:
  OperationType operation_type;
  int transaction_id;
  Granularity granularity;
  std::vector<std::tuple<Granularity, unsigned int>> full_resource_path;
  unsigned int resource;
  bool was_scheduled;

  std::string operation_string;
  /// @brief Parse the operation string
  /// @param operation String to be parsed. Example: "T1: r(a.b.c)"
  std::tuple<OperationType, int, Granularity>
  parse_operation(std::string operation);

public:
  /// @brief Constructor for the Operation class that receives an operation
  /// string and parses it
  Operation(std::string operation);
  /// @brief Getter for operation_type
  OperationType get_operation();
  /// @brief Getter for transaction_id
  int get_transaction_id();
  /// @brief Getter for granularity
  Granularity get_granularity();
  /// @brief Getter for operation_string
  std::string get_operation_string();
  /// @brief Getter for full_resource_path
  std::vector<std::tuple<Granularity, unsigned int>> get_full_resource_path();
  /// @brief Getter for resource
  unsigned int get_resource();
  /// @brief Set the full_resource_path attribute
  void set_full_resource_path(std::string operation);
  /// @brief Set the resource attribute
  void set_resource(std::string operation);
  /// @brief Convert the operation to a string. Example: "T1: r(a.b.c)" ->
  /// "r1(a.b.c)"
  static std::string to_string(std::string operation);
  /// @brief Getter for the was_scheduled
  bool get_was_scheduled();
  void set_was_scheduled (bool ws);
};
} // namespace chronista

#endif
#ifndef CHRONISTA_OPERATION
#define CHRONISTA_OPERATION

namespace chronista {
enum OperationType : unsigned int { Read, Write, Commit, UpdateLock };
enum Granularity : unsigned int { DB, File, Table, Tuple, Index, None };

class Operation {
private:
  OperationType operation_type;
  int transaction_id;
  Granularity granularity;
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
  /// @brief Convert the operation to a string. Example: "T1: r(a.b.c)" ->
  /// "r1(a.b.c)"
  static std::string to_string(std::string operation);
};
} // namespace chronista

#endif
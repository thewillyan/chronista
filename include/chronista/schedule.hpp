#ifndef CHRONISTA_SCHEDULE
#define CHRONISTA_SCHEDULE

#include <string>
#include <vector>

#include "operation.hpp"
using namespace std;

namespace chronista {
class Schedule {
private:
  std::vector<Operation> schedule;

public:
  /// @brief Constructor for the Schedule class that initializes the schedule
  /// vector with an empty vector
  Schedule();
  /// @brief Constructor for the Schedule class that receives an input stream
  /// and parses it to create the schedule vector
  /// @param input
  Schedule(std::istream &input);
  /// @brief Getter for the size of the schedule
  int size();
  /// @brief Getter for the schedule
  void add(Operation operation);
  /// @brief Getter for the schedule as a string. Example: [Operation("T1:
  /// r(a.b.c)"), Operation("T2: w(a.b)")] -> "r1(a.b.c)w2(a.b)"
  std::string to_string();
  /// @brief Getter for the schedule
  std::vector<Operation> get_schedule();
  /// @brief Parse the input stream to create the schedule vector
  std::vector<Operation> parse(std::istream &input);
};
} // namespace chronista

#endif
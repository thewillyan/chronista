#include "doctest/doctest.h"
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

#include "chronista/lockinfo.hpp"
#include "chronista/operation.hpp"

TEST_CASE("Tests the operation class") {
  SUBCASE("Should create an operation") {
    std::string operation_data1 = "T1: r(3.60.3)";
    chronista::Operation *operation1 =
        new chronista::Operation(operation_data1);
    CHECK(operation1->get_operation() == chronista::OperationType::Read);
    CHECK(operation1->get_transaction_id() == 1);
    CHECK(operation1->get_granularity() == chronista::Granularity::Page);
    CHECK(operation1->get_resource() == 3);
    std::vector<std::tuple<chronista::Granularity, unsigned int>> full_resource_path_operation1 =
        operation1->get_full_resource_path();
    CHECK(std::get<0>(full_resource_path_operation1[0]) == chronista::Granularity::Database);
    CHECK(std::get<1>(full_resource_path_operation1[0]) == 3);
    CHECK(std::get<0>(full_resource_path_operation1[1]) == chronista::Granularity::Table);
    CHECK(std::get<1>(full_resource_path_operation1[1]) == 60);
    CHECK(std::get<0>(full_resource_path_operation1[2]) == chronista::Granularity::Page);
    CHECK(std::get<1>(full_resource_path_operation1[2]) == 3);
    CHECK(operation1->get_operation_string() == "r1(3.60.3)");

    std::string operation_data2 = "T2: c";
    chronista::Operation *operation2 =
        new chronista::Operation(operation_data2);
    CHECK(operation2->get_operation() == chronista::OperationType::Commit);
    CHECK(operation2->get_transaction_id() == 2);
    CHECK(operation2->get_granularity() == chronista::Granularity::None);
    CHECK(operation2->get_resource() == NULL);
    CHECK(operation2->get_full_resource_path().empty());
    CHECK(operation2->get_operation_string() == "c2");

    std::string operation_data3 = "T1: ul(1.2.3.4)";
    chronista::Operation *operation3 =
        new chronista::Operation(operation_data3);
    CHECK(operation3->get_operation() == chronista::OperationType::UpdateLock);
    CHECK(operation3->get_transaction_id() == 1);
    CHECK(operation3->get_granularity() == chronista::Granularity::Tuple);
    CHECK(operation3->get_resource() == 4);
    std::vector<std::tuple<chronista::Granularity, unsigned int>> full_resource_path_operation2 =
        operation3->get_full_resource_path();
    CHECK(std::get<0>(full_resource_path_operation2[0]) == chronista::Granularity::Database);
    CHECK(std::get<1>(full_resource_path_operation2[0]) == 1);
    CHECK(std::get<0>(full_resource_path_operation2[1]) == chronista::Granularity::Table);
    CHECK(std::get<1>(full_resource_path_operation2[1]) == 2);
    CHECK(std::get<0>(full_resource_path_operation2[2]) == chronista::Granularity::Page);
    CHECK(std::get<1>(full_resource_path_operation2[2]) == 3);
    CHECK(std::get<0>(full_resource_path_operation2[3]) == chronista::Granularity::Tuple);
    CHECK(std::get<1>(full_resource_path_operation2[3]) == 4);
    CHECK(operation3->get_operation_string() == "ul1(1.2.3.4)");
  }

  SUBCASE("Should throw an exception for invalid operation format") {
    CHECK_THROWS_AS(chronista::Operation("T1: x(1)"), std::invalid_argument);
    CHECK_THROWS_AS(chronista::Operation("T1: x"), std::invalid_argument);
    CHECK_THROWS_AS(chronista::Operation("T1: rc"), std::invalid_argument);
    CHECK_THROWS_AS(chronista::Operation("T1:r(1)"), std::invalid_argument);
    CHECK_THROWS_AS(chronista::Operation("T1:  r(1)"), std::invalid_argument);
    CHECK_THROWS_AS(chronista::Operation("T1: r(1) "), std::invalid_argument);
    CHECK_THROWS_AS(chronista::Operation("TT1: r(1)"), std::invalid_argument);
    CHECK_THROWS_AS(chronista::Operation("TA: r(1)"), std::invalid_argument);
    CHECK_THROWS_AS(chronista::Operation("T1# r(1)"), std::invalid_argument);
    CHECK_THROWS_AS(chronista::Operation("T1: x(1)"), std::invalid_argument);
    CHECK_THROWS_AS(chronista::Operation("T1: r(1.)"), std::invalid_argument);
    CHECK_THROWS_AS(chronista::Operation("T1: r(@)"), std::invalid_argument);
    CHECK_THROWS_AS(chronista::Operation("T1: r(1.2.3.4.5)"),
                    std::invalid_argument);
  }

  SUBCASE("Should transform the operation to a string") {
    std::string operation_data = "T1: r(1.2.3)";
    CHECK(chronista::Operation::to_string(operation_data) == "r1(1.2.3)");
  }
}
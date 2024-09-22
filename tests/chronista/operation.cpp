#include "doctest/doctest.h"
#include <string>
#include <stdexcept>

#include "chronista/operation.hpp"

TEST_CASE("Tests the operation class")
{
    SUBCASE("Should create an operation")
    {
        std::string operation_data1 = "T1: r(a.b.c.d.e)";
        chronista::Operation operation1 = chronista::Operation(operation_data1);
        CHECK(operation1.get_operation() == chronista::OperationType::Read);
        CHECK(operation1.get_transaction_id() == 1);
        CHECK(operation1.get_granularity() == chronista::Granularity::Index);
        CHECK(operation1.get_operation_string() == "r1(a.b.c.d.e)");

        std::string operation_data2 = "T2: c";
        chronista::Operation operation2 = chronista::Operation(operation_data2);
        CHECK(operation2.get_operation() == chronista::OperationType::Commit);
        CHECK(operation2.get_transaction_id() == 2);
        CHECK(operation2.get_granularity() == chronista::Granularity::None);
        CHECK(operation2.get_operation_string() == "c2");
    }

    SUBCASE("Should throw an exception for invalid operation format")
    {
        CHECK_THROWS_AS(chronista::Operation("T1: x(a)"), std::invalid_argument);
        CHECK_THROWS_AS(chronista::Operation("T1: x"), std::invalid_argument);
        CHECK_THROWS_AS(chronista::Operation("T1: rc"), std::invalid_argument);
        CHECK_THROWS_AS(chronista::Operation("T1:r(a)"), std::invalid_argument);
        CHECK_THROWS_AS(chronista::Operation("T1:  r(a)"), std::invalid_argument);
        CHECK_THROWS_AS(chronista::Operation("T1: r(a) "), std::invalid_argument);
        CHECK_THROWS_AS(chronista::Operation("TT1: r(a)"), std::invalid_argument);
        CHECK_THROWS_AS(chronista::Operation("TA: r(a)"), std::invalid_argument);
        CHECK_THROWS_AS(chronista::Operation("T1# r(a)"), std::invalid_argument);
        CHECK_THROWS_AS(chronista::Operation("T1: x(a)"), std::invalid_argument);
        CHECK_THROWS_AS(chronista::Operation("T1: r(a.)"), std::invalid_argument);
        CHECK_THROWS_AS(chronista::Operation("T1: r(@)"), std::invalid_argument);
        CHECK_THROWS_AS(chronista::Operation("T1: r(a.b.c.d.e.f)"), std::invalid_argument);
    }

    SUBCASE("Should transform the operation to a string")
    {
        std::string operation_data = "T1: r(a.b.c)";
        CHECK(chronista::Operation::to_string(operation_data) == "r1(a.b.c)");
    }
}
#include "doctest/doctest.h"
#include <sstream>

#include "chronista/schedule.hpp"
#include <iostream>

TEST_CASE("Tests the schedule class") {
  SUBCASE("Should create a schedule") {
    std::istringstream iss1("");
    std::istream &data1 = iss1;
    chronista::Schedule *schedule1 = new chronista::Schedule(data1);
    CHECK(schedule1->size() == 0);
    CHECK(schedule1->to_string() == "");
    CHECK(schedule1->get_transaction_ids() == std::vector<int>{});

    std::istringstream iss2("T1: r(1.2.3.4)\n"
                            "T2: w(1.2)\n"
                            "T2: ul(1.2)\n"
                            "T1: c\n");
    std::istream &data2 = iss2;
    chronista::Schedule *schedule2 = new chronista::Schedule(data2);
    CHECK(schedule2->size() == 4);
    CHECK(schedule2->to_string() == "r1(1.2.3.4)w2(1.2)ul2(1.2)c1");
    CHECK(schedule2->get_transaction_ids() == std::vector<int>{1, 2});

    std::istringstream iss3("");
    std::istream &data3 = iss3;
    chronista::Schedule *schedule3 = new chronista::Schedule(data3);
    chronista::Operation *operation1 =
        new chronista::Operation("T1: r(1.2.3.4)");
    chronista::Operation *operation2 = new chronista::Operation("T2: w(1.2)");
    chronista::Operation *operation3 = new chronista::Operation("T2: ul(1.2)");
    chronista::Operation *operation4 = new chronista::Operation("T1: c");
    schedule3->add(*operation1);
    schedule3->add(*operation2);
    schedule3->add(*operation3);
    schedule3->add(*operation4);
    CHECK(schedule3->size() == 4);
    CHECK(schedule3->to_string() == "r1(1.2.3.4)w2(1.2)ul2(1.2)c1");
    CHECK(schedule3->get_transaction_ids() == std::vector<int>{1, 2});
  }

  SUBCASE("Should remove most recent transaction from the schedule") {
    std::istringstream iss1("T2: r(1.2.3.4)\n"
                            "T2: w(1.2)\n"
                            "T2: ul(1.2)\n"
                            "T1: c\n"
                            "T3: r(1.2.3.4)\n"
                            "T3: w(1.2)\n"
                            "T2: ul(1.2)\n"
                            "T1: r(1.2.3)\n");
    std::istream &data1 = iss1;
    chronista::Schedule *schedule1 = new chronista::Schedule(data1);
    CHECK(schedule1->size() == 8);
    CHECK(schedule1->to_string() ==
          "r2(1.2.3.4)w2(1.2)ul2(1.2)c1r3(1.2.3.4)w3(1.2)ul2(1.2)r1(1.2.3)");
    CHECK(schedule1->get_transaction_ids() == std::vector<int>{2, 1, 3});
    schedule1->remove_most_recent_transaction(std::vector<int>{3, 2, 1});
    CHECK(schedule1->size() == 6);
    CHECK(schedule1->to_string() ==
          "r2(1.2.3.4)w2(1.2)ul2(1.2)c1ul2(1.2)r1(1.2.3)");
    CHECK(schedule1->get_transaction_ids() == std::vector<int>{2, 1});
  }
}
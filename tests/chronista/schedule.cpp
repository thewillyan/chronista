#include "doctest/doctest.h"
#include <sstream>

#include "chronista/schedule.hpp"

TEST_CASE("Tests the schedule class") {
  SUBCASE("Should create a schedule") {
    std::istringstream iss1("");
    std::istream &data1 = iss1;
    chronista::Schedule *schedule1 = new chronista::Schedule(data1);
    CHECK(schedule1->size() == 0);
    CHECK(schedule1->to_string() == "");

    std::istringstream iss2("T1: r(a.b.c.d)\n"
                            "T2: w(a.b)\n"
                            "T1: c\n");
    std::istream &data2 = iss2;
    chronista::Schedule *schedule2 = new chronista::Schedule(data2);
    CHECK(schedule2->size() == 3);
    CHECK(schedule2->to_string() == "r1(a.b.c.d)w2(a.b)c1");

    std::istringstream iss3("");
    std::istream &data3 = iss3;
    chronista::Schedule *schedule3 = new chronista::Schedule(data3);
    chronista::Operation *operation1 =
        new chronista::Operation("T1: r(a.b.c.d)");
    chronista::Operation *operation2 = new chronista::Operation("T2: w(a.b)");
    chronista::Operation *operation3 = new chronista::Operation("T1: c");
    schedule3->add(*operation1);
    schedule3->add(*operation2);
    schedule3->add(*operation3);
    CHECK(schedule3->size() == 3);
    CHECK(schedule3->to_string() == "r1(a.b.c.d)w2(a.b)c1");
  }

  SUBCASE("Should not create a schedule") {}
}
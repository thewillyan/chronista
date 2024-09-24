#include "chronista/2v2pl.hpp"
#include "chronista/operation.hpp"
#include "chronista/schedule.hpp"
#include <fstream>
#include <iostream>
#include <set>
#include <string>

const auto HELP = "chronista CLI v0.1.0\n"
                  "Usage: chronista-cli [option] [input|filename]\n\n"
                  "Options:\n"
                  "-f\tReceive the transactions from a file\n"
                  "-i\tReceive the transactions from the STDIN\n"
                  "-h\tDisplay help";

std::set<int> get_aborted_trans(chronista::Schedule &in,
                                chronista::Schedule &out) {
  std::set<int> trans_ids;

  // add all transations from the input to trans_ids
  for (chronista::Operation &op : in.get_schedule()) {
    trans_ids.insert(op.get_transaction_id());
  }

  // erase all the transactions from the output on trans_ids leaving only the
  // aborted transations.
  for (chronista::Operation &op : out.get_schedule()) {
    trans_ids.erase(op.get_transaction_id());
  }

  return trans_ids;
}

void handle_trans_input(std::istream &is) {
  chronista::Schedule in_schedule(is);
  chronista::Scheduler2V2PL scheduler;
  chronista::Schedule out_schedule = scheduler.schedule(in_schedule);

  const std::set<int> aborted_trans =
      get_aborted_trans(in_schedule, out_schedule);

  if (out_schedule.size() > 0) {
    std::cout << "Scheduler result: " << out_schedule.to_string() << '.'
              << std::endl;
  }

  if (aborted_trans.size() > 0) {
    std::cout << "Aborted transactions:";
    for (const int &id : aborted_trans) {
      std::cout << " T" << id;
    }
    std::cout << '.' << std::endl;
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2 || argc > 3) {
    std::cout << "Invalid number of arguments. See -h for help." << std::endl;
    return 1;
  }

  const std::string opt = argv[1];

  if (opt == "-f") {
    if (argc != 3) {
      std::cout << "Invalid argument to -f. See -h for help." << std::endl;
      return 1;
    }
    std::ifstream file(argv[2]);
    if (!file) {
      std::cout << "Failed to open the file '" << argv[2] << "'." << std::endl;
      return 1;
    }
    handle_trans_input(file);
  } else if (opt == "-i") {
    if (argc != 2) {
      std::cout << "Invalid argument to -i. See -h for help." << std::endl;
      return 1;
    }
    handle_trans_input(std::cin);
  } else if (opt == "-h") {
    if (argc != 2) {
      std::cout << "Invalid argument to -h." << std::endl;
      return 1;
    }
    std::cout << HELP << std::endl;
  } else {
    std::cout << "Invalid option '" << opt << "'!" << std::endl;
    return 1;
  }

  return 0;
}

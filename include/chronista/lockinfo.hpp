#ifndef CHRONISTA_LOCKINFO
#define CHRONISTA_LOCKINFO

#include <atomic>
#include <memory>
#include <unordered_map>
#include <vector>

namespace chronista {
enum RscType : unsigned int {
  DB,
  File,
  Table,
  Tuple,
  Index,
};

enum ReqStatus : unsigned int {
  Granted,
  Converting,
  Waiting,
};

enum LockType : unsigned int {
  Read,
  Write,
  Certify,
};

class LockInfoTuple {
private:
  // auto incremental id
  static inline std::atomic<unsigned int> li_id = 0;

  const unsigned int id;
  const unsigned int db_id;
  const RscType rsc_type;
  const unsigned int req_tid;
  ReqStatus req_status;
  LockType req_lock_type;

public:
  LockInfoTuple(unsigned int database_id, RscType resource_type,
                unsigned int transaction_id, ReqStatus status,
                LockType lock_type);
  unsigned int get_id() const;
  ReqStatus get_status() const;
  unsigned int get_db_id() const;
  unsigned int get_trans_id() const;
  LockType get_lock_type() const;
  RscType get_rsc_type() const;

  // set status to Granted
  void status_granted();
  // set status to Waiting
  void status_waiting();
  // set status to Converting
  void status_converting();

  // convert to certify lock
  void convert_to_certify();
};

class LockInfo {
private:
  std::unordered_map<unsigned int, std::shared_ptr<LockInfoTuple>> tuples;

public:
  LockInfo();

  // Get the lock with the given id
  std::shared_ptr<LockInfoTuple> &get(unsigned int const lock_id);
  // Add a lock and returns its ID
  unsigned int add(const std::shared_ptr<LockInfoTuple> &tuple);
  // Remove the lock with the given ID
  void rm(unsigned int const lock_id);

  // Get all the locks from the given transaction
  std::vector<std::shared_ptr<LockInfoTuple>>
  get_transaction_locks(const unsigned int &trans_id);
  // Remove all locks from the given transaction
  void rm_transaction_locks(const unsigned int &trans_id);
};
} // namespace chronista

#endif

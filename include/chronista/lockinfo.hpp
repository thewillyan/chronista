#ifndef CHRONISTA_LOCKINFO
#define CHRONISTA_LOCKINFO

#include <atomic>
#include <memory>
#include <unordered_map>
#include <vector>

namespace chronista {
enum RscType : unsigned int {
  Database,
  Table,
  Page,
  Tuple,
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
  Update,
  IRead,
  IWrite,
  ICertify,
  IUpdate,
};

class LockInfoTuple {
private:
  // auto incremental id
  static inline std::atomic<unsigned int> li_id = 0;

  const unsigned int id;
  const unsigned int db_id;
  const unsigned int rsc_id;
  const RscType rsc_type;
  const unsigned int req_tid;
  ReqStatus req_status;
  LockType req_lock_type;

public:
  LockInfoTuple(unsigned int database_id, unsigned int resource_id,
                RscType resource_type, unsigned int transaction_id,
                ReqStatus status, LockType lock_type);
  unsigned int get_id() const;
  ReqStatus get_status() const;
  unsigned int get_db_id() const;
  unsigned int get_rsc_id() const;
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
  void set_lock_type(LockType lock_type);
};

class LockInfo {
private:
  std::unordered_map<unsigned int, std::shared_ptr<LockInfoTuple>> tuples;

public:
  LockInfo();

  // Get the lock with the given id
  std::shared_ptr<LockInfoTuple> &get(const unsigned int lock_id);
  // Add a lock and returns its ID
  unsigned int add(const std::shared_ptr<LockInfoTuple> &tuple);
  // Remove the lock with the given ID
  void rm(const unsigned int lock_id);

  // Get all the locks from the given transaction
  std::vector<std::shared_ptr<LockInfoTuple>>
  get_transaction_locks(const unsigned int &trans_id) const;
  // Get all the locks from the given transaction, given resource_id and given type.
  std::vector<std::shared_ptr<LockInfoTuple>>
  get_transaction_locks(const unsigned int &trans_id, LockType lock_type) const;
  std::vector<std::shared_ptr<LockInfoTuple>>
  get_transaction_locks(const unsigned int &trans_id, unsigned int rsc_id, LockType lock_type) const;
  // Remove all locks from the given transaction
  void rm_transaction_locks(const unsigned int &trans_id);

  // Get all the locks from a given resource
  std::vector<std::shared_ptr<LockInfoTuple>>
  get_rsc_locks(const unsigned int database_id, const unsigned int resource_id,
                const RscType resource_type) const;
  // Get all the locks of a given resource
  std::vector<std::shared_ptr<LockInfoTuple>>
  get_rsc_locks(const unsigned int &resource_id) const;
  // Get all the locks of a given resource and given lock type
  std::vector<std::shared_ptr<LockInfoTuple>>
  get_rsc_locks(const unsigned int &resource_id, const LockType lock_type) const;
};
} // namespace chronista

#endif

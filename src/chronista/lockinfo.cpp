#include "chronista/lockinfo.hpp"
#include <unordered_map>

using namespace chronista;

LockInfoTuple::LockInfoTuple(unsigned int database_id, RscType resource_type,
                             unsigned int transaction_id, ReqStatus status,
                             LockType lock_type)
    : id{li_id++}, db_id{database_id}, rsc_type{resource_type},
      req_tid{transaction_id}, req_status{status}, req_lock_type{lock_type} {}

unsigned int LockInfoTuple::get_id() const { return id; }
ReqStatus LockInfoTuple::get_status() const { return req_status; }
unsigned int LockInfoTuple::get_db_id() const { return db_id; }
unsigned int LockInfoTuple::get_trans_id() const { return req_tid; };
LockType LockInfoTuple::get_lock_type() const { return req_lock_type; }
RscType LockInfoTuple::get_rsc_type() const { return rsc_type; }

// set status to Granted
void LockInfoTuple::status_granted() { req_status = ReqStatus::Granted; }

// set status to Waiting
void LockInfoTuple::status_waiting() { req_status = ReqStatus::Waiting; }

// set status to Converting
void LockInfoTuple::status_converting() { req_status = ReqStatus::Converting; }

// convert to certify lock
void LockInfoTuple::convert_to_certify() {
  req_lock_type = LockType::Certify;
  req_status = ReqStatus::Granted;
}

LockInfo::LockInfo()
    : tuples{
          std::unordered_map<unsigned int, std::shared_ptr<LockInfoTuple>>()} {}

// Get the lock with the given id
std::shared_ptr<LockInfoTuple> &LockInfo::get(unsigned int const lock_id) {
  return tuples[lock_id];
}

// Add a lock and returns its ID
unsigned int LockInfo::add(const std::shared_ptr<LockInfoTuple> &tuple) {
  const unsigned int &id = tuple->get_id();
  tuples[id] = tuple;
  return id;
}

// Remove the lock with the given ID
void LockInfo::rm(unsigned int const lock_id) { tuples.erase(lock_id); }

// Get all the locks from the given transaction
std::vector<std::shared_ptr<LockInfoTuple>>
LockInfo::get_transaction_locks(const unsigned int &trans_id) {
  std::vector<std::shared_ptr<LockInfoTuple>> v{};
  for (const auto &pair : tuples) {
    const std::shared_ptr<LockInfoTuple> &tuple_ptr = pair.second;
    if (tuple_ptr->get_trans_id() == trans_id) {
      v.emplace_back(tuple_ptr);
    }
  }
  return v;
}

// Remove all locks from the given transaction
void LockInfo::rm_transaction_locks(const unsigned int &trans_id) {
  std::vector<unsigned int> to_rm{};
  for (const auto &pair : tuples) {
    if (pair.second->get_trans_id() == trans_id) {
      to_rm.emplace_back(pair.first);
    }
  }

  for (const unsigned int &k : to_rm) {
    tuples.erase(k);
  }
}

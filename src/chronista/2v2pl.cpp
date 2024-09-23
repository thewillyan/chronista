#include "chronista/2v2pl.hpp"
#include <forward_list>
#include <unordered_set>
#include <algorithm>
#include <iostream>

using namespace std;

namespace chronista {

Scheduler2V2PL::Scheduler2V2PL () {}

bool Scheduler2V2PL::try_to_lock (Operation& operation) {

    bool
        operation_can_be_schedule = true,
        is_same_transaction = false,
        lock_already_inherited = false;
    std::vector<std::tuple<Granularity, unsigned int>> resource_path = operation.get_full_resource_path();

    unsigned int database_id;
    if (!resource_path.empty()) {

        database_id = std::get<1>(resource_path[0]);

    }

    LockType intention_lock_type;
    std::vector<std::shared_ptr<LockInfoTuple>> resource_locks = syslockinfo.get_rsc_locks(operation.get_resource());

    // Checks if ascendants have a lock and then apply it.
    std::vector<std::shared_ptr<LockInfoTuple>> ascendant_locks;
    for (int i = 0; i < (int) resource_path.size() - 1; i++) {

        ascendant_locks = syslockinfo.get_rsc_locks(std::get<1>(resource_path[i]));
        for (const auto& ascendant_lock : ascendant_locks) {

            if (
                ascendant_lock->get_status() == ReqStatus::Granted && (
                    ascendant_lock->get_lock_type() == LockType::Read ||
                    ascendant_lock->get_lock_type() == LockType::Write ||
                    ascendant_lock->get_lock_type() == LockType::Update ||
                    ascendant_lock->get_lock_type() == LockType::Certify
                )
            ) {

                lock_already_inherited = syslockinfo.get_transaction_locks(ascendant_lock->get_trans_id(), operation.get_resource(), ascendant_lock->get_lock_type()).size() > 0;

                if (!lock_already_inherited) {

                    syslockinfo.add(std::make_shared<LockInfoTuple>(
                        database_id,
                        operation.get_resource(),
                        resource_map.at(operation.get_granularity()),
                        ascendant_lock->get_trans_id(),
                        ReqStatus::Granted,
                        ascendant_lock->get_lock_type()
                    ));

                }

            }

        }

    }

    // Try to get a lock
    if (operation.get_operation() == OperationType::Read) {

        std::vector<std::shared_ptr<LockInfoTuple>> locks_same_resource = syslockinfo.get_rsc_locks(operation.get_resource());
        bool
            lock_already_exists = false,
            is_lock_already_granted = true;

        std::shared_ptr<LockInfoTuple> lock_waiting = nullptr;

        intention_lock_type = LockType::IRead;

        for (const auto& lock_same_resource : locks_same_resource) {

            is_same_transaction = operation.get_transaction_id() == lock_same_resource->get_trans_id();

            if (!is_same_transaction && lock_same_resource->get_status() == ReqStatus::Granted && !lock_compatibility.at(LockType::Read).at(lock_same_resource->get_lock_type())) {

                operation_can_be_schedule = false;
                wait_for_graph.add_arc(operation.get_transaction_id(), lock_same_resource->get_trans_id());

            } else if (is_same_transaction) {

                if (lock_same_resource->get_lock_type() == LockType::Read || lock_same_resource->get_lock_type() == LockType::Write || lock_same_resource->get_lock_type() == LockType::Update || lock_same_resource->get_lock_type() == LockType::Certify) {

                    lock_already_exists = true;
                    if (lock_same_resource->get_status() == ReqStatus::Waiting) {

                        is_lock_already_granted = false;
                        lock_waiting = lock_same_resource;

                    }

                }

            }

        }

        if (operation_can_be_schedule) {

            // Adds the lock if it does not exist.
            if (!lock_already_exists) {

                syslockinfo.add(std::make_shared<LockInfoTuple>(
                    database_id,
                    operation.get_resource(),
                    resource_map.at(operation.get_granularity()),
                    operation.get_transaction_id(),
                    ReqStatus::Granted,
                    LockType::Read
                ));

            } else {

                lock_waiting->status_granted();

            }

        } else {

            if (!lock_already_exists){

                syslockinfo.add(std::make_shared<LockInfoTuple>(
                    database_id,
                    operation.get_resource(),
                    resource_map.at(operation.get_granularity()),
                    operation.get_transaction_id(),
                    ReqStatus::Waiting,
                    LockType::Read
                ));

            }

        }

    } else if (operation.get_operation() == OperationType::Write) {

        std::vector<std::shared_ptr<LockInfoTuple>>
            locks_same_resource = syslockinfo.get_rsc_locks(operation.get_resource());
        bool
            lock_already_exists = false,
            is_lock_already_granted = true,
            read_lock_already_granted = false;
        std::shared_ptr<LockInfoTuple>
            read_lock = nullptr,
            lock_waiting = nullptr;

        intention_lock_type = LockType::IWrite;

        for (const auto& lock_same_resource : locks_same_resource) {

            is_same_transaction = operation.get_transaction_id() == lock_same_resource->get_trans_id(); 

            if (!is_same_transaction && lock_same_resource->get_status() == ReqStatus::Granted && !lock_compatibility.at(LockType::Write).at(lock_same_resource->get_lock_type())) {

                operation_can_be_schedule = false;
                wait_for_graph.add_arc(operation.get_transaction_id(), lock_same_resource->get_trans_id());

            } else if (is_same_transaction) {

                if (lock_same_resource->get_lock_type() == LockType::Write || lock_same_resource->get_lock_type() == LockType::Certify) {

                    lock_already_exists = true;
                    if (lock_same_resource->get_status() == ReqStatus::Waiting) {

                        is_lock_already_granted = false;
                        lock_waiting = lock_same_resource;

                    }

                }

                if (lock_same_resource->get_status() == ReqStatus::Granted && (lock_same_resource->get_lock_type() == LockType::Read || lock_same_resource->get_lock_type() == LockType::Update)) {

                    read_lock_already_granted = true;
                    read_lock = syslockinfo.get(lock_same_resource->get_id());

                }

            }

        }

        if (operation_can_be_schedule) {

            // Adds the lock if it does not exist.
            if (!lock_already_exists) {

                if (!read_lock_already_granted) {

                    syslockinfo.add(std::make_shared<LockInfoTuple>(
                        database_id,
                        operation.get_resource(),
                        resource_map.at(operation.get_granularity()),
                        operation.get_transaction_id(),
                        ReqStatus::Granted,
                        LockType::Write
                    ));

                } else {

                    read_lock->set_lock_type(LockType::Write);

                }

            } else {

                if (!is_lock_already_granted) {

                    lock_waiting->status_granted();

                }

            }

        } else {

            if (!lock_already_exists) {

                syslockinfo.add(std::make_shared<LockInfoTuple>(
                    database_id,
                    operation.get_resource(),
                    resource_map.at(operation.get_granularity()),
                    operation.get_transaction_id(),
                    ReqStatus::Waiting,
                    LockType::Write
                ));

            }

        }

    } else if (operation.get_operation() == OperationType::UpdateLock) {

        // TODO: Update Lock

    } else {

        std::vector<std::shared_ptr<LockInfoTuple>>
            locks_same_transaction = syslockinfo.get_transaction_locks(operation.get_transaction_id()),
            locks_current_resource;
        bool write_lock_can_be_converted = true;

        intention_lock_type = LockType::ICertify;

        for (const auto& lock_same_transaction : locks_same_transaction) {

            if (lock_same_transaction->get_lock_type() == LockType::Write) {

                write_lock_can_be_converted = true;

                locks_current_resource = syslockinfo.get_rsc_locks(lock_same_transaction->get_rsc_id());
                for (const auto& lock_current_resource : locks_current_resource) {

                    is_same_transaction = lock_same_transaction->get_trans_id() == lock_current_resource->get_trans_id();
                    if (!is_same_transaction) {

                        write_lock_can_be_converted = false;
                        wait_for_graph.add_arc(operation.get_transaction_id(), lock_current_resource->get_trans_id());

                    }

                }

                if (write_lock_can_be_converted) {

                    lock_same_transaction->set_lock_type(LockType::Certify);

                } else {

                    operation_can_be_schedule = false;
                    lock_same_transaction->status_converting();

                }

            }

        }

    }

    // TODO: Se o bloqueio tiver sido concedido, subir os bloqueios intencionais.

    // Apply intention locks.
    if (operation_can_be_schedule) {

        bool intention_lock_already_exists = false;

        for (int i = 0; i < (int) resource_path.size() - 1; i++) {

            ascendant_locks = syslockinfo.get_rsc_locks(std::get<1>(resource_path[i]));
            for (const auto& ascendant_lock : ascendant_locks) {

                if (ascendant_lock->get_status() == ReqStatus::Granted && ascendant_lock->get_lock_type() == intention_lock_type && ascendant_lock->get_trans_id() == operation.get_transaction_id()) {

                    intention_lock_already_exists = true;

                }

            }

            if (!intention_lock_already_exists) {

                syslockinfo.add(std::make_shared<LockInfoTuple>(
                    database_id,
                    std::get<1>(resource_path[i]),
                    resource_map.at(std::get<0>(resource_path[i])),
                    operation.get_transaction_id(),
                    ReqStatus::Granted,
                    intention_lock_type
                ));

            }

        }

    }

    return operation_can_be_schedule;

}

Schedule Scheduler2V2PL::schedule (Schedule incoming_schedule) {

    std::vector<Operation>
        operations = incoming_schedule.get_schedule(),
        waiting_list,
        final_operations;
    std::vector<unsigned int> deadlock_nodes;
    std::forward_list<unsigned int> chronology;
    

    std::unordered_set<unsigned int> waiting_transactions;

    for (auto it = operations.begin(); it != operations.end();) {

        if (it->get_was_scheduled()) {

            ++it;
            continue;

        }

        unsigned int current_transaction_id = it->get_transaction_id();

        // Verifica se a transação já está em espera.
        if (waiting_transactions.find(current_transaction_id) != waiting_transactions.end()) {
            waiting_list.push_back(*it);  // Coloca a operação em espera.
            ++it;
            continue;  // Pula para a próxima operação.
        }

        // Checks if the transaction is in chronology.
        if (std::find(chronology.begin(), chronology.end(), it->get_transaction_id()) == chronology.end()) {

            chronology.push_front(it->get_transaction_id());

        }

        if (try_to_lock(*it)) {

            final_operations.push_back(*it);
            it->set_was_scheduled(true);
            if (it->get_operation() == OperationType::Commit) {

                wait_for_graph.remove_node(it->get_transaction_id());
                syslockinfo.rm_transaction_locks(it->get_transaction_id());

            }

        } else {

            std::cout << "AAAA" << endl;
            for (auto a : wait_for_graph.adjacency_lists) {

                std::cout << a.first << ": ";

                for (auto b : a.second) {

                    std::cout << b << " ";

                }

                std::cout << std::endl;

            }
            std::cout << std::endl;

            deadlock_nodes = wait_for_graph.find_deadlock_nodes();
            if (deadlock_nodes.size() > 0) {

                unsigned int aborted_transaction = incoming_schedule.remove_most_recent_transaction(deadlock_nodes, chronology);

                // wait_for_graph = WaitForGraph();
                // syslockinfo = LockInfo();
                // return schedule(incoming_schedule);
                
                this->abort_transaction(aborted_transaction, final_operations, operations, waiting_list);

                it = operations.begin();

            } else {
                waiting_list.push_back(*it);
                waiting_transactions.insert(it->get_transaction_id());  // Marca a transação como em espera.
            }

        }

        for (auto it_wl = waiting_list.begin(); it_wl != waiting_list.end();) {
            unsigned int waiting_transaction_id = it_wl->get_transaction_id();

            // // Verifica se a transação já está marcada como em espera.
            // if (waiting_transactions.find(waiting_transaction_id) != waiting_transactions.end()) {
            //     ++it_wl;
            //     continue;
            // }

            if (try_to_lock(*it_wl)) {
                final_operations.push_back(*it_wl);
                it_wl->set_was_scheduled(true);

                if (it_wl->get_operation() == OperationType::Commit) {

                    wait_for_graph.remove_node(it_wl->get_transaction_id());
                    syslockinfo.rm_transaction_locks(it_wl->get_transaction_id());

                }

                it_wl = waiting_list.erase(it_wl);  // Remove a operação que foi escalonada com sucesso.
            } else {

                deadlock_nodes = wait_for_graph.find_deadlock_nodes();
                if (deadlock_nodes.size() > 0) {

                    unsigned int aborted_transaction = incoming_schedule.remove_most_recent_transaction(deadlock_nodes, chronology);

                    this->abort_transaction(aborted_transaction, final_operations, operations, waiting_list);

                    it_wl = waiting_list.begin();
                    it = operations.begin();

                } else {

                    ++it_wl;  // Tenta a próxima operação.

                }
            }
        }

        cout << "waiting: ";
        for (auto op : waiting_list) {

            cout << op.get_operation_string() << " ";

        }
        cout << endl;

    }

    Schedule final_schedule;

    for (auto& op : final_operations) final_schedule.add(op);

    return final_schedule;

}

void Scheduler2V2PL::abort_transaction (unsigned int transaction_id, std::vector<Operation> &final_operations, std::vector<Operation> &operations, std::vector<Operation> &waiting_list) {

    wait_for_graph.remove_node(transaction_id);
    syslockinfo.rm_transaction_locks(transaction_id);

    for (auto it_rm = final_operations.begin(); it_rm != final_operations.end();) {

        if (it_rm->get_transaction_id() == transaction_id) {

            it_rm = final_operations.erase(it_rm);

        } else {

            ++it_rm;

        }

    }

    for (auto it_rm = waiting_list.begin(); it_rm != waiting_list.end();) {

        if (it_rm->get_transaction_id() == transaction_id) {

            it_rm = waiting_list.erase(it_rm);

        } else {

            ++it_rm;

        }

    }

    for (auto it_rm = operations.begin(); it_rm != operations.end();) {

        if (it_rm->get_transaction_id() == transaction_id) {

            it_rm = operations.erase(it_rm);

        } else {

            ++it_rm;

        }

    }

}

} // namespace chronista
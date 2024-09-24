#include "chronista/2v2pl.hpp"
#include <forward_list>
#include <unordered_set>
#include <algorithm>
#include <iostream>

using namespace std;

namespace chronista {

Scheduler2V2PL::Scheduler2V2PL () {}

bool Scheduler2V2PL::try_to_convert_lock (const unsigned int lock_id) {

    bool
        lock_can_be_converted = true,
        converted = false;
    LockType convert_to;
    std::shared_ptr<LockInfoTuple> lock_info = syslockinfo.get(lock_id);

    if (lock_info->get_lock_type() == LockType::Read) {

        // Convert to Write Lock.
        convert_to = LockType::Write;

        // Checks for Write Locks.
        std::vector<std::shared_ptr<LockInfoTuple>> write_locks_for_same_resource = syslockinfo.get_rsc_locks(lock_info->get_rsc_id(), LockType::Write);
        for (const auto& write_lock : write_locks_for_same_resource) {

            if (write_lock->get_trans_id() != lock_info->get_trans_id() && write_lock->get_status() == ReqStatus::Granted) {

                lock_can_be_converted = false;
                wait_for_graph.add_arc(lock_info->get_trans_id(), write_lock->get_trans_id());

            }

        }

        // Checks for IWrite Locks.
        std::vector<std::shared_ptr<LockInfoTuple>> i_write_locks_for_same_resource = syslockinfo.get_rsc_locks(lock_info->get_rsc_id(), LockType::IWrite);
        for (const auto& i_write_lock : i_write_locks_for_same_resource) {

            if (i_write_lock->get_trans_id() != lock_info->get_trans_id() && i_write_lock->get_status() == ReqStatus::Granted) {

                lock_can_be_converted = false;
                wait_for_graph.add_arc(lock_info->get_trans_id(), i_write_lock->get_trans_id());

            }

        }

        // Checks for Certify Locks.
        std::vector<std::shared_ptr<LockInfoTuple>> certify_locks_for_same_resource = syslockinfo.get_rsc_locks(lock_info->get_rsc_id(), LockType::Certify);
        for (const auto& certify_lock : certify_locks_for_same_resource) {

            if (certify_lock->get_trans_id() != lock_info->get_trans_id() && certify_lock->get_status() == ReqStatus::Granted) {

                lock_can_be_converted = false;
                wait_for_graph.add_arc(lock_info->get_trans_id(), certify_lock->get_trans_id());

            }

        }

        // Checks for ICertify Locks.
        std::vector<std::shared_ptr<LockInfoTuple>> i_certify_locks_for_same_resource = syslockinfo.get_rsc_locks(lock_info->get_rsc_id(), LockType::ICertify);
        for (const auto& i_certify_lock : i_certify_locks_for_same_resource) {

            if (i_certify_lock->get_trans_id() != lock_info->get_trans_id() && i_certify_lock->get_status() == ReqStatus::Granted) {

                lock_can_be_converted = false;
                wait_for_graph.add_arc(lock_info->get_trans_id(), i_certify_lock->get_trans_id());

            }

        }

    } else if (lock_info->get_lock_type() == LockType::Write) {

        // Convert to Certify Lock.
        convert_to = LockType::Certify;

        // Checks for Read Locks.
        std::vector<std::shared_ptr<LockInfoTuple>> read_locks_for_same_resource = syslockinfo.get_rsc_locks(lock_info->get_rsc_id(), LockType::Read);
        for (const auto& read_lock : read_locks_for_same_resource) {

            if (read_lock->get_trans_id() != lock_info->get_trans_id() && read_lock->get_status() == ReqStatus::Granted) {

                lock_can_be_converted = false;
                wait_for_graph.add_arc(lock_info->get_trans_id(), read_lock->get_trans_id());

            }

        }

        // Checks for IRead Locks.
        std::vector<std::shared_ptr<LockInfoTuple>> i_read_locks_for_same_resource = syslockinfo.get_rsc_locks(lock_info->get_rsc_id(), LockType::IRead);
        for (const auto& i_read_lock : i_read_locks_for_same_resource) {

            if (i_read_lock->get_trans_id() != lock_info->get_trans_id() && i_read_lock->get_status() == ReqStatus::Granted) {

                lock_can_be_converted = false;
                wait_for_graph.add_arc(lock_info->get_trans_id(), i_read_lock->get_trans_id());

            }

        }

    } else {

        converted = false;

    }

    if (lock_can_be_converted) {

        lock_info->set_lock_type(convert_to);
        lock_info->status_granted();
        converted = true;

    } else {

        lock_info->status_converting();
        converted = false;

    }

    return converted;

}

bool Scheduler2V2PL::try_to_lock (const unsigned int &database_id, const unsigned int &resource_id, const RscType &resource_type, const unsigned int &transaction_id, const LockType &lock_type) {

    bool
        grant_lock = true,
        granted_lock_already_exists = false,
        waiting_lock_already_exists = false,
        has_a_high_order_lock = false;
    std::vector<std::shared_ptr<LockInfoTuple>> same_transaction_locks = syslockinfo.get_transaction_locks(transaction_id);
    std::shared_ptr<LockInfoTuple> lock_info = nullptr;

    if (lock_type == LockType::Read) {

        // READ

        // Check for granted higher order locks

        // Checks for granted Certify Locks.
        std::vector<std::shared_ptr<LockInfoTuple>> high_order_locks_for_same_resource = syslockinfo.get_rsc_locks(resource_id, LockType::Certify);
        for (const auto& certify_lock : high_order_locks_for_same_resource) {

            if (certify_lock->get_trans_id() == transaction_id && certify_lock->get_status() == ReqStatus::Granted) {

                grant_lock = true;
                has_a_high_order_lock = true;
                break;

            }

        }

        // Checks for granted Write Locks.
        high_order_locks_for_same_resource = syslockinfo.get_rsc_locks(resource_id, LockType::Write);
        for (const auto& write_lock : high_order_locks_for_same_resource) {

            if (write_lock->get_trans_id() == transaction_id && write_lock->get_status() == ReqStatus::Granted) {

                grant_lock = true;
                has_a_high_order_lock = true;
                break;

            }

        }

        if (!has_a_high_order_lock) {

            // Checks for granted Certify Locks.
            std::vector<std::shared_ptr<LockInfoTuple>> certify_locks_for_same_resource = syslockinfo.get_rsc_locks(resource_id, LockType::Certify);
            for (const auto& certify_lock : certify_locks_for_same_resource) {

                if (certify_lock->get_trans_id() != transaction_id && certify_lock->get_status() == ReqStatus::Granted) {

                    grant_lock = false;
                    wait_for_graph.add_arc(transaction_id, certify_lock->get_trans_id());

                }

            }

            // Checks for granted ICertify Locks.
            std::vector<std::shared_ptr<LockInfoTuple>> i_certify_locks_for_same_resource = syslockinfo.get_rsc_locks(resource_id, LockType::ICertify);
            for (const auto& i_certify_lock : i_certify_locks_for_same_resource) {

                if (i_certify_lock->get_trans_id() != transaction_id && i_certify_lock->get_status() == ReqStatus::Granted) {

                    grant_lock = false;
                    wait_for_graph.add_arc(transaction_id, i_certify_lock->get_trans_id());

                }

            }

            // No certify lock found.
            if (grant_lock) {

                // Checks for already existent Read Locks (granted or waiting) for the given transaction.
                std::vector<std::shared_ptr<LockInfoTuple>> read_locks_for_same_resource = syslockinfo.get_rsc_locks(resource_id, LockType::Read);
                for (const auto& read_lock : read_locks_for_same_resource) {

                    if (read_lock->get_trans_id() == transaction_id) {

                        if (read_lock->get_status() == ReqStatus::Granted) {

                            granted_lock_already_exists = true;
                            lock_info = read_lock;
                            break;

                        } else if (read_lock->get_status() == ReqStatus::Waiting) {

                            waiting_lock_already_exists = true;
                            lock_info = read_lock;
                            break;

                        }

                    }

                }

                // Checks for converting Write Locks (granted Read Lock) for the given transaction.
                std::vector<std::shared_ptr<LockInfoTuple>> converting_write_locks_for_same_resource = syslockinfo.get_rsc_locks(resource_id, LockType::Write);
                for (const auto& write_lock : converting_write_locks_for_same_resource) {

                    if (write_lock->get_trans_id() == transaction_id && write_lock->get_status() == ReqStatus::Converting) {

                        granted_lock_already_exists = true;
                        lock_info = write_lock;
                        break;

                    }

                }

                if (!waiting_lock_already_exists && !granted_lock_already_exists) {

                    // Checks for converting IWrite Locks (granted IRead Lock) for the given transaction.
                    std::vector<std::shared_ptr<LockInfoTuple>> converting_i_write_locks_for_same_resource = syslockinfo.get_rsc_locks(resource_id, LockType::IWrite);
                    for (const auto& i_write_lock : converting_i_write_locks_for_same_resource) {

                        if (i_write_lock->get_trans_id() == transaction_id && i_write_lock->get_status() == ReqStatus::Converting) {

                            granted_lock_already_exists = true;
                            lock_info = i_write_lock;
                            break;

                        }

                    }

                }

                if (waiting_lock_already_exists) {

                    lock_info->status_granted();

                } else if (!granted_lock_already_exists) {

                    lock_info = std::make_shared<LockInfoTuple>(
                        database_id,
                        resource_id,
                        resource_type,
                        transaction_id,
                        ReqStatus::Granted,
                        lock_type
                    );
                    syslockinfo.add(lock_info);

                }

            } else {

                // Checks for Read Locks waiting.
                std::vector<std::shared_ptr<LockInfoTuple>> read_locks_for_same_resource = syslockinfo.get_rsc_locks(resource_id, LockType::Read);
                for (const auto& read_lock : read_locks_for_same_resource) {

                    if (read_lock->get_trans_id() == transaction_id && read_lock->get_status() == ReqStatus::Waiting) {

                        waiting_lock_already_exists = true;
                        break;

                    }

                }

                if (!waiting_lock_already_exists) {

                    lock_info = std::make_shared<LockInfoTuple>(
                        database_id,
                        resource_id,
                        resource_type,
                        transaction_id,
                        ReqStatus::Waiting,
                        lock_type
                    );
                    syslockinfo.add(lock_info);

                }

            }

        }

    } else if (lock_type == LockType::Write) {

        // WRITE

        bool has_a_read_lock = false;

        std::vector<std::shared_ptr<LockInfoTuple>> read_locks_for_same_resource = syslockinfo.get_rsc_locks(resource_id, LockType::Read);
        for (const auto& read_lock : read_locks_for_same_resource) {

            if (read_lock->get_trans_id() != transaction_id && read_lock->get_status() == ReqStatus::Granted) {

                has_a_read_lock = true;
                lock_info = read_lock;
                break;

            }

        }

        if (has_a_read_lock) {

            grant_lock = try_to_convert_lock(lock_info->get_id());

        } else {

            // Checks for granted Write Locks.
            std::vector<std::shared_ptr<LockInfoTuple>> write_locks_for_same_resource = syslockinfo.get_rsc_locks(resource_id, LockType::Write);
            for (const auto& write_lock : write_locks_for_same_resource) {

                if (write_lock->get_trans_id() != transaction_id && write_lock->get_status() == ReqStatus::Granted) {

                    grant_lock = false;
                    wait_for_graph.add_arc(transaction_id, write_lock->get_trans_id());

                }

            }

            // Checks for granted IWrite Locks.
            std::vector<std::shared_ptr<LockInfoTuple>> i_write_locks_for_same_resource = syslockinfo.get_rsc_locks(resource_id, LockType::IWrite);
            for (const auto& i_write_lock : i_write_locks_for_same_resource) {

                if (i_write_lock->get_trans_id() != transaction_id && i_write_lock->get_status() == ReqStatus::Granted) {

                    grant_lock = false;
                    wait_for_graph.add_arc(transaction_id, i_write_lock->get_trans_id());

                }

            }

            // Checks for granted Certify Locks.
            std::vector<std::shared_ptr<LockInfoTuple>> certify_locks_for_same_resource = syslockinfo.get_rsc_locks(resource_id, LockType::Certify);
            for (const auto& certify_lock : certify_locks_for_same_resource) {

                if (certify_lock->get_trans_id() != transaction_id && (certify_lock->get_status() == ReqStatus::Granted || certify_lock->get_status() == ReqStatus::Converting)) {

                    grant_lock = false;
                    wait_for_graph.add_arc(transaction_id, certify_lock->get_trans_id());

                }

            }

            // Checks for granted ICertify Locks.
            std::vector<std::shared_ptr<LockInfoTuple>> i_certify_locks_for_same_resource = syslockinfo.get_rsc_locks(resource_id, LockType::ICertify);
            for (const auto& i_certify_lock : i_certify_locks_for_same_resource) {

                if (i_certify_lock->get_trans_id() != transaction_id && (i_certify_lock->get_status() == ReqStatus::Granted || i_certify_lock->get_status() == ReqStatus::Converting)) {

                    grant_lock = false;
                    wait_for_graph.add_arc(transaction_id, i_certify_lock->get_trans_id());

                }

            }

            // No incompatible lock found.
            if (grant_lock) {

                // Checks for already existent Write Locks (granted or waiting) for the given transaction.
                std::vector<std::shared_ptr<LockInfoTuple>> write_locks_for_same_resource = syslockinfo.get_rsc_locks(resource_id, LockType::Write);
                for (const auto& write_lock : write_locks_for_same_resource) {

                    if (write_lock->get_trans_id() == transaction_id) {

                        if (write_lock->get_status() == ReqStatus::Granted) {

                            granted_lock_already_exists = true;
                            lock_info = write_lock;
                            break;

                        } else if (write_lock->get_status() == ReqStatus::Waiting) {

                            waiting_lock_already_exists = true;
                            lock_info = write_lock;
                            break;

                        }

                    }

                }

                // Checks for converting Certify Locks (granted Write Lock) for the given transaction.
                std::vector<std::shared_ptr<LockInfoTuple>> converting_certify_locks_for_same_resource = syslockinfo.get_rsc_locks(resource_id, LockType::Certify);
                for (const auto& certify_lock : converting_certify_locks_for_same_resource) {

                    if (certify_lock->get_trans_id() == transaction_id && certify_lock->get_status() == ReqStatus::Converting) {

                        granted_lock_already_exists = true;
                        lock_info = certify_lock;
                        break;

                    }

                }

                if (!waiting_lock_already_exists && !granted_lock_already_exists) {

                    // Checks for converting ICertify Locks (granted IWrite Lock) for the given transaction.
                    std::vector<std::shared_ptr<LockInfoTuple>> converting_i_certify_locks_for_same_resource = syslockinfo.get_rsc_locks(resource_id, LockType::ICertify);
                    for (const auto& i_certify_lock : converting_i_certify_locks_for_same_resource) {

                        if (i_certify_lock->get_trans_id() == transaction_id && i_certify_lock->get_status() == ReqStatus::Converting) {

                            granted_lock_already_exists = true;
                            lock_info = i_certify_lock;
                            break;

                        }

                    }

                }

                if (waiting_lock_already_exists) {

                    lock_info->status_granted();

                } else if (!granted_lock_already_exists) {

                    lock_info = std::make_shared<LockInfoTuple>(
                        database_id,
                        resource_id,
                        resource_type,
                        transaction_id,
                        ReqStatus::Granted,
                        lock_type
                    );
                    syslockinfo.add(lock_info);

                }

            } else {

                // Checks for Write Locks waiting.
                std::vector<std::shared_ptr<LockInfoTuple>> write_locks_for_same_resource = syslockinfo.get_rsc_locks(resource_id, LockType::Write);
                for (const auto& write_lock : write_locks_for_same_resource) {

                    if (write_lock->get_trans_id() == transaction_id && write_lock->get_status() == ReqStatus::Waiting) {

                        waiting_lock_already_exists = true;
                        break;

                    }

                }

                if (!waiting_lock_already_exists) {

                    lock_info = std::make_shared<LockInfoTuple>(
                        database_id,
                        resource_id,
                        resource_type,
                        transaction_id,
                        ReqStatus::Waiting,
                        lock_type
                    );
                    syslockinfo.add(lock_info);

                }

            }

        }

    } else {

        grant_lock = false;
    }

    return grant_lock;

}

void Scheduler2V2PL::apply_intention_locks(const Operation& operation, const LockType &i_lock_type) {

    std::vector<std::tuple<chronista::Granularity, unsigned int>> resource_path = operation.get_full_resource_path();

    for (size_t i = 0; i < resource_path.size() - 1; ++i) {

        unsigned int resource_id = std::get<1>(resource_path[i]);
        auto ascendant_locks = syslockinfo.get_rsc_locks(resource_id);

        bool intention_lock_exists = false;
        for (const auto& lock : ascendant_locks) {
            if (lock->get_status() == ReqStatus::Granted &&
                lock->get_lock_type() == i_lock_type &&
                lock->get_trans_id() == operation.get_transaction_id()) {
                intention_lock_exists = true;
                break;
            }
        }

        if (!intention_lock_exists) {



            syslockinfo.add(std::make_shared<LockInfoTuple>(
                std::get<1>(resource_path[0]),
                operation.get_resource(),
                resource_map.at(operation.get_granularity()),
                operation.get_transaction_id(),
                ReqStatus::Granted,
                i_lock_type
            ));
        }
    }
}


bool Scheduler2V2PL::can_be_scheduled (const Operation& operation) {

    // Gambiarra: Checks if ascendants have a lock and then apply it.
    std::vector<std::tuple<chronista::Granularity, unsigned int>> resource_path = operation.get_full_resource_path();
    std::vector<std::shared_ptr<LockInfoTuple>> ascendant_locks;
    bool lock_already_inherited;
    unsigned int database_id;
    if (!resource_path.empty()) {

        database_id = std::get<1>(resource_path[0]);

    }
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

    bool operation_can_be_scheduled = true;

    if (operation.get_operation() == OperationType::Read) {
        
        operation_can_be_scheduled = try_to_lock(
            std::get<1>(operation.get_full_resource_path()[0]),
            operation.get_resource(),
            resource_map.at(operation.get_granularity()),
            operation.get_transaction_id(),
            LockType::Read
        );

    } else if (operation.get_operation() == OperationType::Write) {

        operation_can_be_scheduled = try_to_lock(
            std::get<1>(operation.get_full_resource_path()[0]),
            operation.get_resource(),
            resource_map.at(operation.get_granularity()),
            operation.get_transaction_id(),
            LockType::Write
        );

    } else if (operation.get_operation() == OperationType::Commit) {

        std::vector<std::shared_ptr<LockInfoTuple>>
            locks_from_same_transaction = syslockinfo.get_transaction_locks(operation.get_transaction_id());

        bool lock_can_be_converted = true;

        // Try to convert all write locks to certify locks.
        for (const auto& lock : locks_from_same_transaction) {

            if (lock->get_lock_type() == LockType::Write && lock->get_status() == ReqStatus::Granted) {

                if (!this->try_to_convert_lock(lock->get_id())) {

                    operation_can_be_scheduled = false;

                }

            } else if (lock->get_lock_type() == LockType::Certify && lock->get_status() == ReqStatus::Converting) {

                lock->set_lock_type(LockType::Write);
                lock->status_granted();

                if (!try_to_convert_lock(lock->get_id())) {

                    operation_can_be_scheduled = false;

                }

            } else {

                continue;

            }

        }

    } else {

        operation_can_be_scheduled = false;

    }

    return operation_can_be_scheduled;

}

Schedule Scheduler2V2PL::schedule(Schedule incoming_schedule) {
    std::vector<Operation> operations = incoming_schedule.get_schedule();
    std::vector<Operation> waiting_list, final_operations;
    std::vector<unsigned int> deadlock_nodes;
    std::forward_list<unsigned int> chronology;
    std::unordered_set<unsigned int> waiting_transactions;

    auto it = operations.begin();
    while (it != operations.end()) {
        unsigned int current_transaction_id = it->get_transaction_id();

        // Adiciona transação na cronologia, se ainda não estiver lá
        if (std::find(chronology.begin(), chronology.end(), current_transaction_id) == chronology.end()) {
            chronology.push_front(current_transaction_id);
        }

        // Se a transação está na lista de espera, ignora a operação por agora
        if (waiting_transactions.find(current_transaction_id) != waiting_transactions.end()) {
            waiting_list.push_back(*it);
            ++it;
            continue;
        }

        // Tenta escalonar a operação
        if (this->can_be_scheduled(*it)) {
            final_operations.push_back(*it);

            // Se a operação é um Commit, remove os bloqueios e a transação do grafo
            if (it->get_operation() == OperationType::Commit) {
                wait_for_graph.remove_node(current_transaction_id);
                syslockinfo.rm_transaction_locks(current_transaction_id);
            }
            ++it;
        } else {
            // Adiciona a operação à lista de espera e marca a transação como aguardando
            waiting_list.push_back(*it);
            waiting_transactions.insert(current_transaction_id);
            ++it;
        }

        // Verifica deadlock após qualquer modificação na lista de operações
        deadlock_nodes = wait_for_graph.find_deadlock_nodes();
        if (!deadlock_nodes.empty()) {
            unsigned int aborted_transaction = incoming_schedule.remove_most_recent_transaction(deadlock_nodes, chronology);
            this->abort_transaction(aborted_transaction, final_operations, operations, waiting_list);
            it = operations.begin();  // Reinicia a iteração após o aborto de uma transação
        }

        // Verifica se há operações na lista de espera que podem ser escalonadas agora
        auto it_wl = waiting_list.begin();
        while (it_wl != waiting_list.end()) {
            if (this->can_be_scheduled(*it_wl)) {
                final_operations.push_back(*it_wl);

                // Se for commit, remove a transação do grafo de espera e os bloqueios
                if (it_wl->get_operation() == OperationType::Commit) {
                    wait_for_graph.remove_node(it_wl->get_transaction_id());
                    syslockinfo.rm_transaction_locks(it_wl->get_transaction_id());
                }

                // Remove da lista de espera e atualiza o iterador
                it_wl = waiting_list.erase(it_wl);
            } else {
                deadlock_nodes = wait_for_graph.find_deadlock_nodes();
                if (!deadlock_nodes.empty()) {
                    unsigned int aborted_transaction = incoming_schedule.remove_most_recent_transaction(deadlock_nodes, chronology);
                    this->abort_transaction(aborted_transaction, final_operations, operations, waiting_list);
                    it_wl = waiting_list.begin();  // Reinicia a iteração após o aborto de uma transação
                    it = operations.begin();       // Reinicia também a iteração nas operações principais
                } else {
                    ++it_wl;
                }
            }
        }
    }

    // Cria o cronograma final a partir das operações escalonadas
    Schedule final_schedule;
    for (auto& op : final_operations) {
        final_schedule.add(op);
    }

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
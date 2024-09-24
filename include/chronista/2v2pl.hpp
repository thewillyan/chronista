#ifndef CHRONISTA_2V2PL
#define CHRONISTA_2V2PL

#include "chronista/waitfor.hpp"
#include "chronista/lockinfo.hpp"
#include "chronista/schedule.hpp"
#include <unordered_map>

namespace chronista
{

const std::unordered_map<Granularity, RscType> resource_map = {

    {Granularity::Database, RscType::Database},
    {Granularity::Table, RscType::Table},
    {Granularity::Page, RscType::Page},
    {Granularity::Tuple, RscType::Tuple}

}; 
    
class Scheduler2V2PL {

    private:

        WaitForGraph wait_for_graph;
        LockInfo syslockinfo;

        bool try_to_convert_lock (const unsigned int lock_id);
        bool try_to_lock (const unsigned int &database_id, const unsigned int &resource_id, const RscType &resource_type, const unsigned int &transaction_id, const LockType &lock_type);
        void apply_intention_locks (const Operation& operation, const LockType &i_lock_type);
        bool can_be_scheduled (const Operation& operation);
        bool try_to_lock_old (Operation& operation);
        void abort_transaction (unsigned int transaction_id, std::vector<Operation> &final_operations, std::vector<Operation> &operations, std::vector<Operation> &waiting_list);

    public:

        Scheduler2V2PL ();
    
        Schedule schedule (Schedule incoming_schedule);
};

} // namespace chronista

#endif
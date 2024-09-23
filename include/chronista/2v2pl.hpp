#ifndef CHRONISTA_2V2PL
#define CHRONISTA_2V2PL

#include "chronista/waitfor.hpp"
#include "chronista/lockinfo.hpp"
#include "chronista/schedule.hpp"
#include <unordered_map>

namespace chronista
{

const std::unordered_map<LockType, std::unordered_map<LockType, bool>> lock_compatibility = {

    {LockType::Read, {
        {LockType::Read, true }, {LockType::Write, true }, {LockType::Certify, false}, {LockType::IRead, true }, {LockType::IWrite, true }, {LockType::ICertify, false}
    }},

    {LockType::Write, {
        {LockType::Read, true }, {LockType::Write, false}, {LockType::Certify, false}, {LockType::IRead, true }, {LockType::IWrite, false}, {LockType::ICertify, false}
    }},

    {LockType::Certify, {
        {LockType::Read, false}, {LockType::Write, false}, {LockType::Certify, false}, {LockType::IRead, false}, {LockType::IWrite, false}, {LockType::ICertify, false}
    }},

    {LockType::IRead, {
        {LockType::Read, true }, {LockType::Write, true }, {LockType::Certify, false}, {LockType::IRead, true }, {LockType::IWrite, true }, {LockType::ICertify, true }
    }},

    {LockType::IWrite, {
        {LockType::Read, true}, {LockType::Write, false}, {LockType::Certify, false}, {LockType::IRead, true }, {LockType::IWrite, true }, {LockType::ICertify, true }
    }},

    {LockType::ICertify, {
        {LockType::Read, false}, {LockType::Write, false}, {LockType::Certify, false}, {LockType::IRead, true }, {LockType::IWrite, true }, {LockType::ICertify, true }
    }}

};

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

        bool try_to_lock (Operation& operation);
        void abort_transaction (unsigned int transaction_id, std::vector<Operation> &final_operations, std::vector<Operation> &operations, std::vector<Operation> &waiting_list);

    public:

        Scheduler2V2PL ();
    
        Schedule schedule (Schedule incoming_schedule);
};

} // namespace chronista

#endif
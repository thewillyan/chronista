#ifndef CHRONISTA_WAITFOR
#define CHRONISTA_WAITFOR

#include <map>
#include <forward_list>
#include <optional>
#include <vector>

namespace chronista
{

    class WaitForGraph
    {
    private:
        std::map<unsigned int, std::forward_list<unsigned int>> adjacency_lists;

        enum NodeStatusDFS
        {
            UNVISITED,
            VISITED,
            FINISHED
        };
        struct NodeInfoDFS
        {
            NodeStatusDFS status;
            std::optional<unsigned int> parent;
        };

    public:
        WaitForGraph();

        // Return a std::vector containing nodes involved in a cycle.
        // If no cycle is detected, an empty std::vector is returned.
        std::vector<unsigned int> find_deadlock_nodes() const;
    };

}

#endif
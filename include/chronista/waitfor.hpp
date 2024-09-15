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
        // Adds a new node to the graph.
        // If the node already exists, no changes are made.
        void add_node(unsigned int node);

        // Adds a directed arc from 'source' to 'target' in the graph.
        // If either 'source' or 'target' nodes do not exist, they are automatically added.
        void add_arc(unsigned int source, unsigned int target);

        // Removes a node from the graph.
        // If the node does not exist, no changes are made.
        void remove_node(unsigned int node);

        // Removes a directed arc from 'source' to 'target' in the graph.
        // If either 'source' or 'target' nodes do not exist, no changes are made.
        void remove_arc(unsigned int source, unsigned int target);

        // Return a std::vector containing nodes involved in a cycle.
        // If no cycle is detected, an empty std::vector is returned.
        std::vector<unsigned int> find_deadlock_nodes() const;
    };

}

#endif
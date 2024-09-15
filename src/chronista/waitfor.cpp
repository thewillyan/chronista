#include "chronista/waitfor.hpp"
#include <stack>

namespace chronista
{

    WaitForGraph::WaitForGraph()
    {
        adjacency_lists[10] = {15};
        adjacency_lists[15] = {98};
        adjacency_lists[22] = {33};
        adjacency_lists[29] = {55};
        adjacency_lists[32] = {10, 29};
        adjacency_lists[55] = {98};
        adjacency_lists[98] = {22, 33};

        // Cycle
        adjacency_lists[33] = {29};
        adjacency_lists[55] = {10};
    }

    std::vector<unsigned int> WaitForGraph::find_deadlock_nodes() const
    {
        std::map<unsigned int, NodeInfoDFS> nodes_info;
        std::stack<unsigned int> dfs_stack;
        std::vector<unsigned int> cycle_nodes;

        // Reserve space for cycle nodes to optimize memory allocation.
        cycle_nodes.reserve(adjacency_lists.size());

        // Initialize nodes info.
        for (const auto &adjacency_list : adjacency_lists)
        {
            nodes_info[adjacency_list.first].status = UNVISITED;
        }

        unsigned int current_node;
        std::forward_list<unsigned int> const *current_adjacency_list;
        std::forward_list<unsigned int>::const_iterator adjacent_node_it;
        bool
            found_cycle = false,
            found_unvisited_neighbor = false;
        auto nodes_info_it = nodes_info.begin();

        // Modified Depth-First Search (DFS) algorithm.
        // Iterate through all nodes to ensure the entire graph is explored.
        while (nodes_info_it != nodes_info.end() && !found_cycle)
        {
            // If the node has not been visited, start a DFS from it.
            if (nodes_info_it->second.status == UNVISITED)
            {
                dfs_stack.push(nodes_info_it->first);
                nodes_info[nodes_info_it->first].status = VISITED;

                while (dfs_stack.size() > 0 && !found_cycle)
                {
                    found_unvisited_neighbor = false;
                    current_node = dfs_stack.top();
                    current_adjacency_list = &(adjacency_lists.at(current_node));

                    // If the current node has neighbors, explore them.
                    if (!current_adjacency_list->empty())
                    {
                        adjacent_node_it = current_adjacency_list->begin();

                        // Traverse through all adjacent nodes.
                        while (adjacent_node_it != current_adjacency_list->end() && !found_cycle)
                        {
                            // If a node already visited is found, a cycle is detected.
                            if (nodes_info[*adjacent_node_it].status == VISITED)
                            {
                                // Store the nodes involved in the cycle.
                                found_cycle = true;
                                while (current_node != *adjacent_node_it)
                                {

                                    cycle_nodes.push_back(current_node);
                                    current_node = nodes_info[current_node].parent.value();
                                }
                                cycle_nodes.push_back(*adjacent_node_it);
                            }
                            else if (!found_unvisited_neighbor && nodes_info[*adjacent_node_it].status == UNVISITED)
                            {
                                found_unvisited_neighbor = true;

                                // Push the unvisited node to the stack and mark it as visited.
                                dfs_stack.push(*adjacent_node_it);
                                nodes_info[*adjacent_node_it].status = VISITED;
                                nodes_info[*adjacent_node_it].parent = current_node;
                            }

                            adjacent_node_it++;
                        }
                    }

                    // If no unvisited neighbors were found, finish the current node.
                    if (!found_unvisited_neighbor)
                    {
                        nodes_info[current_node].status = FINISHED;
                        dfs_stack.pop();
                    }
                }
            }

            nodes_info_it++;
        }

        // Shrink the cycle nodes vector to release unused memory.
        cycle_nodes.shrink_to_fit();
        return cycle_nodes;
    }

}
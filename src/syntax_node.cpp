#include <syntax/syntax_node.hpp>

using namespace blitz_query_cpp;


bool syntax_node::add_child(syntax_node *child_node)
{
    size_t current_size = children.size();
    if (static_storage *storage = std::get_if<static_storage>(&child_storage))
    {
        if (current_size + 1 < storage->max_size())
        {
            children = node_span(storage->begin(), current_size + 1);
            children.back() = child_node;
            return true;
        }
        dynamic_storage dyn_store{storage->begin(), storage->end()};
        child_storage = std::move(dyn_store);
    }
    if (dynamic_storage *storage = std::get_if<dynamic_storage>(&child_storage))
    {
        storage->push_back(child_node);
        children = node_span(storage->begin(), storage->end());
        return true;
    }
    return false;
}
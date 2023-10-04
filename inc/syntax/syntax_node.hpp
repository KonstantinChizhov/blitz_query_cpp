#pragma once
#include <global_definitions.hpp>
#include <syntax/syntax_node_type.hpp>
#include <syntax/nullability.hpp>
#include <string_view>
#include <variant>
#include <span>
#include <array>
#include <vector>

namespace blitz_query_cpp
{
    struct syntax_node;
    using node_span = std::span<syntax_node *>;

    struct syntax_node
    {
        using static_storage = std::array<syntax_node *, MaxBuiltInChildNodes>;
        using dynamic_storage = std::vector<syntax_node *>;

        syntax_node()
        {
            static_storage *storage = std::get_if<static_storage>(&child_storage);
            children = node_span(storage->begin(), 0);
        }

        index_t pos;
        index_t size;
        syntax_node_type type;
        std::string_view content;
        std::string_view name;
        std::string_view alias;
        syntax_node *parent;
        syntax_node *selection_set;

        node_span children; // all node children
        node_span directives;
        node_span arguments_or_variables; // arguments or variables defunition depending on node type

        union
        {
            nullability_t nullability;
            operation_type_t operation_type;
        };

        bool add_child(syntax_node &child_node)
        {
            return add_child(&child_node);
        }

        bool add_child(syntax_node *child_node)
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

    private:
        std::variant<
            static_storage,
            dynamic_storage>
            child_storage;
    };
}
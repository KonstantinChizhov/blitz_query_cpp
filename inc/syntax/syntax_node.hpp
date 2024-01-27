#pragma once
#include <global_definitions.hpp>
#include <syntax/syntax_node_type.hpp>
#include <syntax/directive_target.hpp>
#include <syntax/nullability.hpp>
#include <syntax/operation_type.hpp>
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
        std::string_view description;
        syntax_node *parent = nullptr;
        node_span children; // all node children
        node_span directives;

        union
        {
            syntax_node *selection_set = nullptr;
            syntax_node *definition_type;
        };

        union
        {
            node_span arguments = node_span();
            node_span variables;
            node_span implements;
        };
        union
        {
            nullability_t nullability;
            operation_type_t operation_type;
            long long intValue = 0;
            double floatValue;
            bool boolValue;
            directive_target_t directive_target;
        };

        bool add_child(syntax_node *child_node);
        bool add_child(syntax_node &child_node) { return add_child(&child_node); }
        bool of_type(syntax_node_type t) { return has_any_flag(type, t); }

    private:
        std::variant<
            static_storage,
            dynamic_storage>
            child_storage;
    };
}
#pragma once
#include <global_definitions.hpp>
#include <syntax/syntax_node_type.hpp>
#include <syntax/nullability.hpp>
#include <vector>
#include <string_view>
#include <variant>

namespace blitz_query_cpp
{
    struct syntax_node;

    using node_list = std::vector<syntax_node*>;

    struct directive
    {
        node_list arguments;
    };

    struct inline_fragment
    {
        node_list directives;
        syntax_node *selection_set;
    };

    struct fragment_spread
    {
        node_list directives;
    };

    struct selection_set
    {
        node_list selections;
    };

    struct selection
    {
        nullability_t nullability;
        std::string_view alias;
        syntax_node *selection_set;
        node_list directives;
        node_list arguments;
    };

    struct operation
    {
        operation_type_t operation_type;
        selection_set *selection_set;
        node_list directives;
        node_list variables;
    };

    struct syntax_node
    {
        index_t pos;
        index_t size;
        syntax_node_type type;
        std::string_view content;
        syntax_node *parent;
        node_list children;
        std::string_view name;

        std::variant<
            operation,
            selection_set,
            selection,
            fragment_spread,
            inline_fragment,
            directive>
            properties;
    };
}
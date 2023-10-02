#pragma once
#include <global_definitions.hpp>
#include <syntax/operation_type.hpp>
#include <lexica/token.hpp>
#include <syntax/syntax_node.hpp>

#include <string_view>
#include <vector>

namespace blitz_query_cpp
{
    class document : public syntax_node
    {
    public:
        std::string doc_value;
        std::vector<token> tokens;
        std::vector<syntax_node> all_nodes;

    public:
        document(std::string doc)
            : syntax_node{0, doc.size(), syntax_node_type::Document},
              doc_value(std::move(doc))
        {
            syntax_node::content = doc_value;
        }

    };
}
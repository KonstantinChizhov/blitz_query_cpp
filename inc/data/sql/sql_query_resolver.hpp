#pragma once

#include <struct/query_context.hpp>

namespace blitz_query_cpp
{
    class sql_query_resolver
    {
        static constexpr std::string Key = "sql";
        bool process_query(query_context &context, syntax_node *operation);
        bool process_mutation(query_context &context, syntax_node *operation);
    public:
        bool process(query_context &context);
    };

}

#pragma once

#include <struct/query_context.hpp>

namespace blitz_query_cpp
{
    class sql_query_resolver
    {
        static constexpr std::string Key = "sql";
        static constexpr std::string_view TableDirective = "@table";
        static constexpr std::string_view ColumnDirective = "@column";
        static constexpr std::string_view IsProjected = "@always_projected";
        static constexpr std::string_view DefaultSchama = "public";

        syntax_node *_operation;
        sql::sql_expr_t _query;
        std::string_view _table_name, _schema_name;
        int alias_number = 1;
        std::string current_selection_alias;

        bool process_query(query_context &context);
        bool process_mutation(query_context &context);
        bool process_field(query_context &context, const field &field_decl);
        std::string next_alias_name()
        {
            std::string alias = "_a" + std::to_string(alias_number);
            alias_number++;
            return alias;
        }

    public:
        bool process(query_context &context);
    };

}

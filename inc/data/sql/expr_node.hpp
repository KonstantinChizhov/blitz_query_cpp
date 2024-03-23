#pragma once
#include <data/sql/sql_expr.hpp>

namespace blitz_query_cpp::sql
{

    struct expr_node
    {
        expr_node(const sql_expr_t *exp, std::string_view val = {})
            : expr{exp}, value{val}
        {
        }

        const sql_expr_t *expr;
        const std::string value;
    };
}
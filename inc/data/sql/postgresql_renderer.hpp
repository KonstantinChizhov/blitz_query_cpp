#pragma once
#include <string>
#include "data/sql/sql_expr.hpp"
#include "data/sql/expr_node.hpp"

namespace blitz_query_cpp::sql
{

    class postgresql_renderer
    {
        std::vector<expr_node> node_stack;
        std::string buffer;
        bool handle_binary_op(expr_node &current, std::string_view op);
        void visit_children(expr_node &node, std::string_view separator = {}, std::string_view end = {});
        void write_quoted_value(expr_node &node);
        void push_value(std::string_view value);
        void push_node(const sql_expr_t &node);

    public:
        bool render(const sql::sql_expr_t &expr);
        const std::string &get_string() const { return buffer; }
    };
}
#include "data/sql/sql_expr.hpp"
#include <algorithm>
#include <string_view>

namespace blitz_query_cpp::sql
{
    sql_expr_t &sql_expr_t::add_child(sql_expr_type t, std::string_view val)
    {
        return children.emplace_back(t, val);
    }

    sql_expr_t &create_or_get_child_with_type(sql_expr_t &node, sql_expr_type type, std::string_view val = {})
    {
        auto res = std::find_if(node.children.rbegin(), node.children.rend(), [type](auto &v)
                                { return v.type <= type; });

        if (res == node.children.rend() || res->type != type)
        {
            return *node.children.emplace(res.base(), type, val);
        }

        return *res;
    }

    sql_expr_t &create_child_with_type(sql_expr_t &node, sql_expr_type type, std::string_view val = {})
    {
        auto res = std::find_if(node.children.rbegin(), node.children.rend(), [type](auto &v)
                                { return v.type <= type; });

        return *node.children.emplace(res.base(), type, val);
    }

    sql_expr_t select(std::span<const std::string_view> columns)
    {
        sql_expr_t res{sql_expr_type::Select};
        auto &selection = res.add_child(sql_expr_type::SelectColumns);
        for (auto col : columns)
        {
            selection.add_child(sql_expr_type::Column, col);
        }
        return res;
    }

    sql_expr_t &operator|(sql_expr_t &expr, from_expr_t &&from_expr)
    {
        auto &res = create_or_get_child_with_type(expr, sql_expr_type::From);
        res.children.emplace_back(std::move(from_expr.expr));
        return expr;
    }

    sql_expr_t &operator|(sql_expr_t &expr, where_expr_t &&cond)
    {
        sql_expr_t &where_node = create_or_get_child_with_type(expr, sql_expr_type::Where);
        where_node.children.emplace_back(std::move(cond.expr));
        return expr;
    }

    sql_expr_t &operator|(sql_expr_t &expr, or_expr_t &&cond)
    {
        sql_expr_t &where_node = create_or_get_child_with_type(expr, sql_expr_type::Where);
        sql_expr_t *op_parent_node = &where_node;

        if (where_node.children.size() > 0)
        {
            sql_expr_t or_op{sql_expr_type::Or};
            if (where_node.children.size() > 1)
            {
                auto &and_node = or_op.add_child(sql_expr_type::And);
                and_node.children = std::move(where_node.children);
            }
            else
            {
                or_op.children = std::move(where_node.children);
            }
            op_parent_node = &where_node.children.emplace_back(std::move(or_op));
        }

        op_parent_node->children.emplace_back(std::move(cond.expr));
        return expr;
    }

    sql_expr_t binary_operation(binary_op op, sql_expr_t &&a, sql_expr_t &&b)
    {
        sql_expr_t res{get_expr_type(op)};
        res.children.emplace_back(std::move(a));
        res.children.emplace_back(std::move(b));
        return res;
    }

    sql_expr_t binary_operation(binary_op op, const sql_expr_t &a, const sql_expr_t &b)
    {
        sql_expr_t res{get_expr_type(op)};
        res.children.emplace_back(a);
        res.children.emplace_back(b);
        return res;
    }

    sql_expr_t &operator|(sql_expr_t &expr, order_by_t &&param)
    {
        sql_expr_t &order_node = create_or_get_child_with_type(expr, sql_expr_type::Order);
        order_node.children.emplace_back(param.ascending ? sql_expr_type::Asc : sql_expr_type::Desc, "", std::move(param.column));
        return expr;
    }

    sql_expr_t &operator|(sql_expr_t &expr, limit_t param)
    {
        sql_expr_t &node = create_or_get_child_with_type(expr, sql_expr_type::Limit);
        node.value = std::to_string(param.value);
        return expr;
    }

    sql_expr_t &operator|(sql_expr_t &expr, offset_t param)
    {
        sql_expr_t &node = create_or_get_child_with_type(expr, sql_expr_type::Offset);
        node.value = std::to_string(param.value);
        return expr;
    }

    sql_expr_t &operator|(sql_expr_t &expr, join_t &&param)
    {
        sql_expr_t &join_node = create_child_with_type(expr, get_expr_type(param.kind));
        join_node.children.emplace_back(std::move(param.expr));
        sql_expr_t &on_node = join_node.add_child(sql_expr_type::On);
        on_node.children.emplace_back(std::move(param.column1));
        on_node.children.emplace_back(std::move(param.column2));
        return expr;
    }

    sql_expr_t &operator|(sql_expr_t &expr, column_t &&param)
    {
        auto &columns = create_or_get_child_with_type(expr, sql_expr_type::SelectColumns);
        columns.children.emplace_back(std::move(param.expr));
        return expr;
    }

}
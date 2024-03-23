#include "data/sql/sql_expr.hpp"
#include <algorithm>

namespace blitz_query_cpp::sql
{
    sql_expr_t &sql_expr_t::add_child(sql_expr_type t, std::string_view val)
    {
        return children.emplace_back(t, val);
    }

    sql_expr_t &child_with_type(sql_expr_t &node, sql_expr_type type)
    {
        auto res = std::find_if(node.children.rbegin(), node.children.rend(), [type](auto &v)
                                { return v.type == type; });
        if (res == node.children.rend())
            return node.add_child(type, {});
        return *res;
    }

    sql_expr_t &sql_expr_t::from(std::string_view table)
    {
        add_child(sql_expr_type::From, table);
        return *this;
    }

    sql_expr_t &sql_expr_t::from(const sql_expr_t &expr)
    {
        auto &res = add_child(sql_expr_type::From, "");
        res.children.emplace_back(expr);
        return *this;
    }

    sql_expr_t &sql_expr_t::from(sql_expr_t &&expr)
    {
        auto &res = add_child(sql_expr_type::From, "");
        res.children.emplace_back(std::move(expr));
        return *this;
    }

    sql_expr_t &sql_expr_t::where(std::string_view column, binary_op op, std::string_view value)
    {
        sql_expr_type op_node_type = static_cast<sql_expr_type>(op);
        sql_expr_t op_node{op_node_type};
        op_node.add_child(sql_expr_type::Column, column);
        op_node.add_child(sql_expr_type::StringLiteral, value);
        return where(std::move(op_node));
    }

    sql_expr_t &sql_expr_t::where(std::string_view column, binary_op op, long value)
    {
        sql_expr_type op_node_type = static_cast<sql_expr_type>(op);
        sql_expr_t op_node{op_node_type};
        op_node.add_child(sql_expr_type::Column, column);
        op_node.add_child(sql_expr_type::NumberLiteral, std::to_string(value));
        return where(std::move(op_node));
    }

    sql_expr_t &sql_expr_t::where(std::string_view column, binary_op op, double value)
    {
        sql_expr_type op_node_type = static_cast<sql_expr_type>(op);
        sql_expr_t op_node{op_node_type};
        op_node.add_child(sql_expr_type::Column, column);
        op_node.add_child(sql_expr_type::NumberLiteral, std::to_string(value));
        return where(std::move(op_node));
    }

    sql_expr_t &sql_expr_t::where(const sql_expr_t &expr)
    {
        return where(sql_expr_t{expr});
    }

    sql_expr_t &sql_expr_t::where(sql_expr_t &&expr)
    {
        sql_expr_t &where_node = child_with_type(*this, sql_expr_type::Where);
        where_node.children.emplace_back(std::move(expr));
        return *this;
    }

    sql_expr_t &sql_expr_t::or_else(std::string_view column, binary_op op, std::string_view value)
    {
        sql_expr_type op_node_type = static_cast<sql_expr_type>(op);
        sql_expr_t cond{op_node_type};
        cond.add_child(sql_expr_type::Column, column);
        cond.add_child(sql_expr_type::StringLiteral, value);
        return or_else(std::move(cond));
    }

    sql_expr_t &sql_expr_t::or_else(const sql_expr_t &expr)
    {
        return or_else(sql_expr_t{expr});
    }

    sql_expr_t &sql_expr_t::or_else(sql_expr_t &&expr)
    {
        sql_expr_t &where_node = child_with_type(*this, sql_expr_type::Where);
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

        op_parent_node->children.emplace_back(std::move(expr));
        return *this;
    }

    sql_expr_t &sql_expr_t::order_by(std::string_view column, bool ascending)
    {
        sql_expr_t &order_node = child_with_type(*this, sql_expr_type::Order);
        order_node.add_child(ascending ? sql_expr_type::Asc : sql_expr_type::Desc, column);
        return *this;
    }

    sql_expr_t &sql_expr_t::limit(int value)
    {
        sql_expr_t &node = child_with_type(*this, sql_expr_type::Limit);
        node.value = std::to_string(value);
        return *this;
    }

    sql_expr_t &sql_expr_t::offset(int value)
    {
        sql_expr_t &node = child_with_type(*this, sql_expr_type::Offset);
        node.value = std::to_string(value);
        return *this;
    }

    sql_expr_t select(std::span<const std::string_view> columns)
    {
        sql_expr_t res{sql_expr_type::Select};
        auto &selection = res.add_child(sql_expr_type::SelectColumns);
        for (auto col : columns)
        {
            selection.children.emplace_back(sql_expr_type::Column, col);
        }
        return res;
    }
}
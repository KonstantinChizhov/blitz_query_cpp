#pragma once
#include <string_view>
#include <string>
#include <span>
#include <array>
#include <vector>
#include <initializer_list>
#include <data/sql/sql_expr_type.hpp>

namespace blitz_query_cpp::sql
{
    struct sql_expr_t
    {
        sql_expr_t() {}
        sql_expr_t(sql_expr_type t, std::string_view val = {}) : type{t}, value{val} {}

        sql_expr_type type;
        std::string value;
        std::vector<sql_expr_t> children;
        sql_expr_t &add_child(sql_expr_type t, std::string_view val = {});
    };

    sql_expr_t select(std::span<const std::string_view> columns);

    inline sql_expr_t select(std::initializer_list<std::string_view> columns)
    {
        return select(std::span(std::begin(columns), std::end(columns)));
    }

    //-----------------------------------
    // FROM
    //-----------------------------------
    struct from_table_t
    {
        std::string_view table;
    };

    struct from_expr_t
    {
        sql_expr_t expr;
    };

    inline from_table_t from(std::string_view table) { return from_table_t{table}; }
    inline from_expr_t from(const sql_expr_t &expr) { return from_expr_t{expr}; }       // copy expression
    inline from_expr_t from(sql_expr_t &&expr) { return from_expr_t{std::move(expr)}; } // move from temporary

    sql_expr_t &operator|(sql_expr_t &, from_table_t);
    sql_expr_t &operator|(sql_expr_t &, from_expr_t);

    inline sql_expr_t &&operator|(sql_expr_t &&expr, from_table_t table) { return std::move(expr | table); }
    inline sql_expr_t &&operator|(sql_expr_t &&expr, from_expr_t table) { return std::move(expr | table); }

    //-----------------------------------
    // WHERE
    //-----------------------------------

    struct where_expr_t
    {
        sql_expr_t expr;
    };

    inline where_expr_t where(const sql_expr_t &expr) { return where_expr_t{expr}; }       // copy expression
    inline where_expr_t where(sql_expr_t &&expr) { return where_expr_t{std::move(expr)}; } // move from temporary

    sql_expr_t &operator|(sql_expr_t &, where_expr_t);
    inline sql_expr_t &&operator|(sql_expr_t &&expr, where_expr_t cond) { return std::move(expr | cond); }

    //-----------------------------------
    // OR
    //-----------------------------------

    struct or_expr_t
    {
        sql_expr_t expr;
    };

    inline or_expr_t or_else(const sql_expr_t &expr) { return or_expr_t{expr}; }       // copy expression
    inline or_expr_t or_else(sql_expr_t &&expr) { return or_expr_t{std::move(expr)}; } // move from temporary

    sql_expr_t &operator|(sql_expr_t &, or_expr_t);
    inline sql_expr_t &&operator|(sql_expr_t &&expr, or_expr_t cond) { return std::move(expr | cond); }

    //-----------------------------------
    // operations
    //-----------------------------------

    sql_expr_t binary_operation(sql_expr_t &&a, binary_op op, sql_expr_t &&b);
    sql_expr_t binary_operation(const sql_expr_t &a, binary_op op, const sql_expr_t &b);

    template <class T>
    sql_expr_t binary_operation(std::string_view column, binary_op op, T &&value)
    {
        sql_expr_t op_node{get_expr_type(op)};
        op_node.add_child(sql_expr_type::Column, column);
        op_node.add_child(get_expr_type_for_t<T>::type, get_expr_type_for_t<T>::to_string(value));
        return op_node;
    }

    struct query_param
    {
        std::string_view name;
    };

    inline query_param param(std::string_view name) { return query_param{name}; }

    template <>
    struct get_expr_type_for_t<query_param>
    {
        static constexpr sql_expr_type type = sql_expr_type::Parameter;
        static std::string to_string(query_param value) { return std::string{value.name}; }
    };

    //-----------------------------------
    // order by
    //-----------------------------------

    struct order_by_t
    {
        std::string_view column;
        bool ascending;
    };

    inline order_by_t order_by(std::string_view column, bool ascending = true) { return order_by_t{column, ascending}; }
    sql_expr_t &operator|(sql_expr_t &, order_by_t);
    inline sql_expr_t &&operator|(sql_expr_t &&expr, order_by_t param) { return std::move(expr | param); }


    //-----------------------------------
    // limit offset
    //-----------------------------------

    struct limit_t
    {
        int value;
    };

    inline limit_t limit(int value) { return limit_t{value}; }
    sql_expr_t &operator|(sql_expr_t &, limit_t);
    inline sql_expr_t &&operator|(sql_expr_t &&expr, limit_t param) { return std::move(expr | param); }

    struct offset_t
    {
        int value;
    };

    inline offset_t offset(int value) { return offset_t{value}; }
    sql_expr_t &operator|(sql_expr_t &, offset_t);
    inline sql_expr_t &&operator|(sql_expr_t &&expr, offset_t param) { return std::move(expr | param); }

}
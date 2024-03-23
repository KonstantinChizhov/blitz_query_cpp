#pragma once
#include <string_view>
#include <string>
#include <span>
#include <array>
#include <vector>
#include <initializer_list>

namespace blitz_query_cpp
{
    namespace sql
    {
        // sql::select("u.id", "u.name", "u.title", "a.address")
        //  .from("users").as("u")
        //  .inner_join("address", "a", "id", "usr_id")
        //  .where("a.name", Like::starts_with, "Joe%")
        //  .order_by("a.name")

        enum class sql_expr_type
        {
            Column,
            TableName,
            SchemaName,
            Function,
            StringLiteral,
            NumberLiteral,
            Parameter,
            Asias,

            Plus,
            Minus,
            Multiply,
            Divide,
            And,
            Or,
            Eq, // =
            Ne, // <>
            Le, // <
            Gt, // >
            Lq, // <=
            Gq, // >=
            Like,

            Select,
            SelectDistinct,
            From,
            Insert,
            Update,
            Delete,
            SelectColumns,
            Where,
            Having,
            Group,
            Order,
            InnerJoin,
            LeftJoin,
            RightJoin,
            OuterJoin,
            On,
            Values,
            All,
            Any,
            Beetween,
            Exists,
            In,
            IsNull,
            IsNotNull,
            Limit,
            Asc,
            Desc,
            Not,
            Set,
            Union,
            UnionAll,
            Offset
        };

        enum class binary_op
        {
            Or = (int)sql_expr_type::Or,
            Eq = (int)sql_expr_type::Eq, // =
            Ne = (int)sql_expr_type::Ne, // <>
            Le = (int)sql_expr_type::Le, // <
            Gt = (int)sql_expr_type::Gt, // >
            Lq = (int)sql_expr_type::Lq, // <=
            Gq = (int)sql_expr_type::Gq, // >=
            Like = (int)sql_expr_type::Like,
        };

        struct sql_expr_t
        {
            sql_expr_t() {}
            sql_expr_t(sql_expr_type t, std::string_view val = {}) : type{t}, value{val} {}

            sql_expr_type type;
            std::string value;
            std::vector<sql_expr_t> children;
            sql_expr_t &add_child(sql_expr_type t, std::string_view val = {});

            sql_expr_t &from(std::string_view table);
            sql_expr_t &from(const sql_expr_t &expr);
            sql_expr_t &from(sql_expr_t &&expr);

            sql_expr_t &where(std::string_view column, binary_op op, std::string_view value);
            sql_expr_t &where(std::string_view column, binary_op op, long value);
            sql_expr_t &where(std::string_view column, binary_op op, int value) { return where(column, op, (long)value); }
            sql_expr_t &where(std::string_view column, binary_op op, double value);
            sql_expr_t &where(const sql_expr_t &expr);
            sql_expr_t &where(sql_expr_t &&expr);

            sql_expr_t &or_else(std::string_view column, binary_op op, std::string_view value);
            sql_expr_t &or_else(const sql_expr_t &expr);
            sql_expr_t &or_else(sql_expr_t &&expr);

            sql_expr_t &order_by(std::string_view column, bool ascending = true);
            sql_expr_t &limit(int value);
            sql_expr_t &offset(int value);
        };

        sql_expr_t select(std::span<const std::string_view> columns);

        inline sql_expr_t select(std::initializer_list<std::string_view> columns)
        {
            return select(std::span(std::begin(columns), std::end(columns)));
        }

    }
}
#pragma once

namespace blitz_query_cpp::sql
{
    enum class sql_expr_type
    {
        None = 0,
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

        // top level statements
        Select,
        SelectDistinct,
        Insert,
        Update,
        Delete,

        // second level statements. realtive order is important
        From,
        InnerJoin,
        LeftJoin,
        RightJoin,
        OuterJoin,
        Where,
        Group,
        Having,
        Order,
        Offset,
        Limit,

        On,
        SelectColumns,
        Values,
        All,
        Any,
        Beetween,
        Exists,
        In,
        IsNull,
        IsNotNull,
      
        Asc,
        Desc,
        Not,
        Set,
        // set operation
        Union,
        UnionAll,
        Intersect,
        Except,
    };

    enum class binary_op
    {
        Plus = (int)sql_expr_type::Plus,
        Minus = (int)sql_expr_type::Minus,
        Multiply = (int)sql_expr_type::Multiply,
        Divide = (int)sql_expr_type::Divide,
        Eq = (int)sql_expr_type::Eq, // =
        Ne = (int)sql_expr_type::Ne, // <>
        Le = (int)sql_expr_type::Le, // <
        Gt = (int)sql_expr_type::Gt, // >
        Lq = (int)sql_expr_type::Lq, // <=
        Gq = (int)sql_expr_type::Gq, // >=
        Like = (int)sql_expr_type::Like,
        And = (int)sql_expr_type::And,
        Or = (int)sql_expr_type::Or,
    };

    enum class join_kind
    {
        InnerJoin = (int)sql_expr_type::InnerJoin,
        LeftJoin = (int)sql_expr_type::LeftJoin,
        RightJoin = (int)sql_expr_type::RightJoin,
        OuterJoin = (int)sql_expr_type::OuterJoin,
    };

    enum class set_operation
    {
        Union = (int)sql_expr_type::Union,
        UnionAll = (int)sql_expr_type::UnionAll,
        Intersect = (int)sql_expr_type::Intersect,
        Except = (int)sql_expr_type::Except,
    };

    inline constexpr sql_expr_type get_expr_type(binary_op op)
    {
        return static_cast<sql_expr_type>(op);
    }

    template <class T>
    struct get_expr_type_for_t
    {
        static constexpr sql_expr_type type = sql_expr_type::NumberLiteral;
        static std::string to_string(T value) { return std::to_string(value); }
    };

    template <>
    struct get_expr_type_for_t<std::string_view>
    {
        static constexpr sql_expr_type type = sql_expr_type::StringLiteral;
        static std::string to_string(std::string_view value) { return std::string{value}; }
    };

    template <>
    struct get_expr_type_for_t<std::string>
    {
        static constexpr sql_expr_type type = sql_expr_type::StringLiteral;
        static std::string to_string(const std::string value) { return value; }
    };

    template <>
    struct get_expr_type_for_t<const char *>
    {
        static constexpr sql_expr_type type = sql_expr_type::StringLiteral;
        static std::string to_string(const char *value) { return value; }
    };

    template <std::size_t Size>
    struct get_expr_type_for_t<const char (&)[Size]>
    {
        static constexpr sql_expr_type type = sql_expr_type::StringLiteral;
        static std::string to_string(const char value[Size]) { return std::string(value, Size - 1); }
    };

}
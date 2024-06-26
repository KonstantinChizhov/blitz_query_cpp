#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <format>
#include <variant>

#include <type_system/schema.hpp>
#include <syntax/syntax_node.hpp>
#include <syntax/document.hpp>
#include <data/sql/sql_expr.hpp>

namespace blitz_query_cpp
{
    struct query_context
    {
        query_context(const schema_t *schema_, std::string doc)
            : schema{schema_}, document{doc}
        {
        }

        const schema_t *schema;
        document_t document;
        std::unordered_map<std::string, std::string> data;
        std::variant<sql::sql_expr_t> result;
        std::vector<std::string> error_msgs;

        bool report_error(std::string_view msg)
        {
            error_msgs.emplace_back(msg);
            return false;
        }

        template <class... Args>
        bool report_error(std::string_view fmt, Args &&...args)
        {
            std::string error_msg = std::vformat(fmt, std::make_format_args(args...));
            error_msgs.emplace_back(std::move(error_msg));
            return false;
        }
    };
}
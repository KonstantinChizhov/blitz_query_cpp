#pragma once
#include <global_definitions.hpp>
#include <string_view>
#include <lexica/token_type.hpp>

namespace blitz_query_cpp
{
    class token_t
    {
    public:
        std::string_view value;
        index_t pos;
        index_t size;
        token_type type;

        token_t(std::string_view token_value, index_t pos, index_t size, token_type type)
            : value(token_value), pos(pos), size(size), type(type)
        {
        }

        token_t(token_type type = token_type::None)
            : value(), pos(0), size(0), type(type)
        {
        }

        bool of_type(token_type t) const { return has_any_flag(type, t); }
    };
}
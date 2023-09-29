#pragma once
#include "config.hpp"
#include <string_view>
#include "token_type.hpp"

namespace blitz_query_cpp
{
    template <typename CharT>
    class token
    {
    public:
        const std::basic_string_view<CharT> value;
        const index_t pos;
        const index_t size;
        const token_type type;

        token(std::basic_string_view<CharT> token_value, index_t pos, index_t size, token_type type)
            : value(token_string), pos(pos), size(size), type(type)
        {
        }

        token(token_type type)
            : value(), pos(0), size(0), type(type)
        {
        }
    };
}
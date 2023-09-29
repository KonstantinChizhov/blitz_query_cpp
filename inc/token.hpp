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
        std::basic_string_view<CharT> value;
        index_t pos;
        index_t size;
        token_type type;
        
        token(std::basic_string_view<CharT> token_value, index_t pos, index_t size, token_type type)
            : value(token_value), pos(pos), size(size), type(type)
        {
        }

        token(token_type type)
            : value(), pos(0), size(0), type(type)
        {
        }
    };
}
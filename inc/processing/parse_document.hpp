#pragma once

#include <struct/query_context.hpp>

namespace blitz_query_cpp
{
    class parse_document
    {
    public:
        bool process(query_context &context);
    };
}

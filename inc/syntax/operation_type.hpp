#pragma once
#include <string_view>

namespace blitz_query_cpp
{
    enum class operation_type_t
    {
        None,
        Query,
        Mutation,
        Subscribtion,
    };

    inline operation_type_t parse_operation_type(std::string_view value)
    {
        if (value == "query")
            return operation_type_t::Query;
        if (value == "mutation")
            return operation_type_t::Mutation;
        if (value == "subscribtion")
            return operation_type_t::Subscribtion;
        return operation_type_t::None;
    }
}
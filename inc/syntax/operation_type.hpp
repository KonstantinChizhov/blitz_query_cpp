#pragma once

namespace blitz_query_cpp
{
    enum class operation_type_t
    {
        None,
        Query,
        Mutation,
        Subscribtion,
    };
}
#pragma once

namespace blitz_query_cpp
{
    enum value_kind
    {
        None,
        Boolean,
        Enum,
        Float,
        Integer,
        List,
        Null,
        Object,
        String,
    };
}
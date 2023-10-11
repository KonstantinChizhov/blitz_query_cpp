#pragma once
#include <util/enum.hpp>


namespace blitz_query_cpp
{
    enum class type_kind
    {
        None = 0,
        Directive = 1,
        Enum = 2,
        InputObject = 4,
        Interface = 8,
        List = 16,
        NonNull = 32,
        Object = 64,
        Scalar = 128,
        Union = 256,
    };

    DECLARE_ENUM_OPERATIONS(type_kind);
}
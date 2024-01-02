#pragma once
#include <util/enum.hpp>
#include <string>

namespace blitz_query_cpp
{
    enum class type_kind
    {
        None = 0,
        Enum = 1,
        InputObject = 2,
        Interface = 4,
        List = 8,
        NonNull = 16,
        Object = 32,
        Scalar = 64,
        Union = 128,
    };

    DECLARE_ENUM_OPERATIONS(type_kind);

    std::string enum_name(type_kind);
}
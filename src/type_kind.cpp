#include <type_system/type_kind.hpp>

namespace blitz_query_cpp
{
    std::string enum_name(type_kind kind)
    {
        switch (kind)
        {
        case type_kind::Enum:
            return "ENUM";
        case type_kind::InputObject:
            return "INPUT_OBJECT";
        case type_kind::Interface:
            return "INTERFACE";
        case type_kind::List:
            return "LIST";
        case type_kind::NonNull:
            return "NON_NULL";
        case type_kind::Object:
            return "OBJECT";
        case type_kind::Scalar:
            return "SCALAR";
        case type_kind::Union:
            return "UNION";
        case type_kind::None:
        default:
            break;
        }
        return "UNKNOWN";
    }
}
#pragma once
#include <global_definitions.hpp>
#include <string>
#include <unordered_map>
#include <optional>
#include <optional>
#include <type_system/type_system_classes.hpp>

namespace blitz_query_cpp
{
    class schema
    {
    public:
        named_collection<object_type> types;
        named_collection<directive_type> directive_types;
        std::vector<directive> directives;

        std::string query_type_name;
        std::string mutation_type_name;

    public:

    };
}
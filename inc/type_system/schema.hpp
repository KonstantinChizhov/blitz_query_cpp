#pragma once
#include <global_definitions.hpp>
#include <string>
#include <util/named_list.hpp>
#include <util/named_storage.hpp>
#include <type_system/type_system_classes.hpp>

namespace blitz_query_cpp
{
    class schema
    {
    public:
        named_storage<output_type> types;
        named_storage<interface_type> interfaces;
        named_storage<input_type> input_types;
        named_storage<enum_type> enums;
        named_storage<directive_type> directives;
        named_storage<scalar_type> scalars;
        std::string query_type_name;
        std::string mutation_type_name;
    public:

    };
}
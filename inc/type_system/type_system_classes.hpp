#pragma once
#include <global_definitions.hpp>
#include <type_system/type_kind.hpp>
#include <type_system/value_kind.hpp>
#include <unordered_map>
#include <optional>
#include <vector>
#include <string_view>
#include <string>


namespace blitz_query_cpp
{

    struct type_system_object
    {
        type_kind kind;
        std::string name;
        std::string description;
        bool is_deprecated;
        std::string deprecation_reason;
    };

    struct directive_type : type_system_object
    {
        static constexpr type_kind static_type_kind = type_kind::Directive;
        directive_type() { kind = static_type_kind; }
        bool is_repeatable;
        bool is_executable;
        bool is_type_system;
        bool is_public;
        std::unordered_map<std::string, struct field> arguments;
    };

    struct directive_parameter
    {
        value_kind value_type;
        std::string name;
        std::string description;
        std::string string_value;
        long long int_value = 0;
        double float_value = 0;
        bool bool_value = false;
        std::vector<directive_parameter> fields;
    };

    struct directive
    {
        std::string name;
        directive_type *directive_type;
        std::vector<directive_parameter> parameters;
    };

    struct type_system_object_with_directives : type_system_object
    {
        std::vector<directive> directives;

        directive &add_directive(std::string_view name)
        {
            directive &item = directives.emplace_back();
            item.name = name;
            return item;
        }
    };

    struct field : type_system_object_with_directives
    {
        bool is_optional;
        int index;
        std::string default_value;
        std::string declaring_type_name;
        std::string field_type_name;
    };

    struct object_type : type_system_object_with_directives
    {
        std::unordered_map<std::string, field> fields;
        std::vector<std::string> implements;
    };

}
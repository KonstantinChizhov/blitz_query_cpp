#pragma once
#include <global_definitions.hpp>
#include <type_system/type_kind.hpp>
#include <type_system/value_kind.hpp>
#include <syntax/directive_target.hpp>
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

    using argument_collection = std::unordered_map<std::string, struct input_value>;

    struct directive_type : type_system_object
    {
        static constexpr type_kind static_type_kind = type_kind::Directive;
        directive_type() { kind = static_type_kind; }
        directive_target_t target = directive_target_t::None;
        argument_collection arguments;
    };

    struct parameter_value
    {
        value_kind value_type;
        std::string name;
        std::string description;
        std::string string_value;
        long long int_value = 0;
        double float_value = 0;
        bool bool_value = false;
        std::vector<parameter_value> fields;
    };

    struct directive
    {
        std::string name;
        directive_type *directive_type;
        std::vector<parameter_value> parameters;
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

    struct type_reference
    {
        std::string name;
        struct object_type *type;
    };

    struct input_value: type_system_object_with_directives
    {
        int index = 0;
        parameter_value default_value;
        type_reference declaring_type;
        type_reference field_type;
        uint32_t field_type_nullability = 0;
        int list_nesting_depth = 0;
    };

    struct field : type_system_object_with_directives
    {
        bool is_optional;
        int index;
        type_reference declaring_type;
        type_reference field_type;
        argument_collection arguments;
    };

    struct object_type : type_system_object_with_directives
    {
        std::unordered_map<std::string, field> fields;
        std::unordered_map<std::string, type_reference> implements;
    };

}
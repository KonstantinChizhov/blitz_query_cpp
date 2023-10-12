#pragma once
#include <global_definitions.hpp>
#include <type_system/type_kind.hpp>
#include <string_view>
#include <string>
#include <vector>
#include <unordered_map>

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
        bool is_repeatable;
        bool is_executable;
        bool is_type_system;
        bool is_public;
        std::vector<struct input_field *> arguments;
    };

    struct directive :directive_type
    {
        
    };

    struct type_system_object_with_directives : type_system_object
    {
        std::vector<directive *> directives;
    };

    struct enum_value : type_system_object_with_directives
    {
        std::string value;
    };

    struct enum_type : type_system_object_with_directives
    {
        
    };

    struct field : type_system_object_with_directives
    {
        bool is_optional;
        int index;
        struct object_type *declaring_type;
        struct object_type *field_type;
    };

    struct input_field : field
    {
        struct input_type *declaring_type;
        struct input_type *field_type;
        std::string default_value;
    };

    struct object_field : field
    {
        std::vector<input_field *> arguments;
    };

    struct object_type : type_system_object_with_directives
    {
        type_kind kind;
    };

    struct input_type : object_type
    {
    };

    struct output_type : object_type
    {
        std::vector<output_type *> implements;
        std::vector<object_field* > fields;
    };
}
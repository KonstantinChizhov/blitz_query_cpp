#pragma once
#include <global_definitions.hpp>
#include <type_system/type_kind.hpp>
#include <util/named_list.hpp>
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
        directive_type() { kind = type_kind::Directive; }
        bool is_repeatable;
        bool is_executable;
        bool is_type_system;
        bool is_public;
        named_list<struct input_field> arguments;
    };

    struct directive 
    {
        std::string name;
        directive_type *directive_type;
        
    };

    struct type_system_object_with_directives : type_system_object
    {
        named_storage<directive> directives;
    };

    struct enum_value : type_system_object_with_directives
    {
        std::string value;
    };

    struct enum_type : type_system_object_with_directives
    {
        named_list<enum_value> values;
        enum_type() { kind = type_kind::Enum; }
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
        named_list<input_field *> arguments;
    };

    struct object_type : type_system_object_with_directives
    {
    };

    struct input_type : object_type
    {
        input_type() { kind = type_kind::InputObject; }
        named_storage<input_field> fields;
    };

    struct output_type : object_type
    {
        output_type() { kind = type_kind::Object; }
        named_list<output_type> implements;
        named_storage<object_field> fields;
    };

    struct interface_type : output_type
    {
         interface_type() { kind = type_kind::Interface; }
    };

    struct scalar_type : type_system_object_with_directives
    {
        scalar_type() { kind = type_kind::Scalar; }
    };
}
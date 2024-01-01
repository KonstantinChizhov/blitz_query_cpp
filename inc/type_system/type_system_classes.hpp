#pragma once
#include <global_definitions.hpp>
#include <type_system/type_kind.hpp>
#include <type_system/value_kind.hpp>
#include <syntax/directive_target.hpp>
#include <unordered_set>
#include <optional>
#include <vector>
#include <string_view>
#include <string>
#include <utility>

namespace blitz_query_cpp
{
    struct name_hash
    {
        using hash_type = std::hash<std::string_view>;
        using is_transparent = void;

        std::size_t operator()(const char *str) const noexcept { return hash_type{}(str); }
        std::size_t operator()(std::string_view str) const noexcept { return hash_type{}(str); }
        std::size_t operator()(std::string const &str) const noexcept { return hash_type{}(str); }
        template <class T>
        std::size_t operator()(const T &named_obj) const noexcept { return hash_type{}(named_obj.name); }
    };

    template <class T>
    struct name_compare
    {
        using is_transparent = void;

        bool operator()(const T &a, const std::string_view &name) const noexcept { return a.name == name; }
        bool operator()(const std::string_view &name, const T &a) const noexcept { return a.name == name; }
        bool operator()(const T &a, const T &b) const noexcept { return a.name == b.name; }
    };

    template <class T>
    using named_collection = std::unordered_set<T, name_hash, name_compare<T>>;

    struct type_system_object
    {
        type_system_object() = default;
        type_system_object(std::string_view name_, std::string_view description_) : name{name_}, description{description_} {}

        std::string name;
        std::string description;
    };

    struct directive_type : type_system_object
    {
        using type_system_object::type_system_object;
        directive_type(std::string_view name_, std::string_view description_, directive_target_t target_)
            : type_system_object{name_, description_}, target{target_}
        {
        }

        directive_target_t target = directive_target_t::None;
        named_collection<struct input_value> arguments;
    };

    struct parameter_value : type_system_object
    {
        using type_system_object::type_system_object;

        value_kind value_type = value_kind::None;
        std::string string_value;
        long long int_value = 0;
        double float_value = 0;
        bool bool_value = false;
        named_collection<parameter_value> fields;
    };

    struct directive
    {
        directive() = default;
        directive(std::string_view name_) : name{name_} {}

        std::string name;
        directive_type *directive_type = nullptr;
        named_collection<parameter_value> parameters;
    };

    struct type_system_object_with_directives : type_system_object
    {
        using type_system_object::type_system_object;
        type_system_object_with_directives() = default;
        std::vector<directive> directives;
        bool is_deprecated;
        std::string deprecation_reason;
    };

    struct type_reference
    {
        type_reference() = default;
        type_reference(std::string_view name_) : name(name_) {}
        std::string name;
        struct object_type *type = nullptr;
    };

    struct input_value : type_system_object_with_directives
    {
        using type_system_object_with_directives::type_system_object_with_directives;

        int index = 0;
        parameter_value default_value;
        type_reference declaring_type;
        type_reference field_type;
        uint32_t field_type_nullability = 0;
        int list_nesting_depth = 0;
    };

    struct field : public type_system_object_with_directives
    {
        field() = default;
        using type_system_object_with_directives::type_system_object_with_directives;

        bool is_optional;
        int index;
        type_reference declaring_type;
        type_reference field_type;
        named_collection<input_value> arguments;
    };

    struct object_type : type_system_object_with_directives
    {
        using type_system_object_with_directives::type_system_object_with_directives;
        object_type(type_kind kind_, std::string_view name_, std::string_view description_)
            : type_system_object_with_directives(name_, description_), kind(kind_)
        {
        }

        type_kind kind;
        named_collection<field> fields;
        named_collection<type_reference> implements;
    };
}
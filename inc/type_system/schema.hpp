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
        std::unordered_map<std::string, object_type> types;
        std::unordered_map<std::string, directive_type> directives;

        std::string query_type_name;
        std::string mutation_type_name;

    public:
        std::optional<object_type> create_type(type_kind kind, std::string_view name)
        {
            if (name.size() < 1)
            {
                return std::nullopt;
                ;
            }
            auto res = types.try_emplace(std::string(name));
            if (!res.second)
            {
                return std::nullopt;
            }
            object_type &obj = res.first->second;
            obj.kind = kind,
            obj.name = name;
            return obj;
        }
    };
}
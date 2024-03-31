#pragma once
#include <global_definitions.hpp>
#include <string>
#include <unordered_map>
#include <optional>
#include <type_system/type_system_classes.hpp>

namespace blitz_query_cpp
{
    class schema_t
    {
    public:
        const object_type *query_type = nullptr;
        const object_type *mutation_type = nullptr;

    public:
        named_collection<object_type> types;
        named_collection<directive_type> directive_types;
        std::vector<directive> directives;

        std::string query_type_name;
        std::string mutation_type_name;

    public:
        const directive_type *find_directive_type(std::string_view name) const
        {
            auto dir = std::find_if(directive_types.begin(), directive_types.end(),
                                    [name](auto &dir)
                                    {
                                        return dir.name == name;
                                    });
            if (dir == directive_types.end())
                return nullptr;
            return &*dir;
        }

        const object_type *find_type(std::string_view name) const
        {
            auto type = types.find(name);
            if (type == types.end())
                return nullptr;
            return &*type;
        }

        const object_type *get_query_type() const
        {
            return query_type;
        }
        const object_type *get_mutation_type() const
        {
            return mutation_type;
        }
    };
}
#pragma once
#include <global_definitions.hpp>
#include <string>
#include <vector>
#include <unordered_map>

namespace blitz_query_cpp
{
    template <class T>
    class named_list
    {
        std::unordered_map<std::string, T *> _name_map;
        std::vector<T> _values;

    public:
        named_list()
        {
            _values.reserve(SchemaChildListReserve);
            _name_map.reserve(SchemaChildListReserve);
        }

        void clear()
        {
            _values.clear();
            _name_map.clear();
        }

        T &add_new(std::string name)
        {
            auto &result = _values.emplace_back(name);
            _name_map.insert_or_assign(name, &result);
            return result;
        }

        const std::vector<T> &items() const { return _values; }

        T *find(std::string name)
        {
            auto res = _name_map.find(name);
            if (res == _name_map.end())
            {
                return nullptr;
            }
            return res->second;
        }
    };
}
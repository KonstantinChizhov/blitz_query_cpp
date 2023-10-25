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
        std::vector<T*> _values;

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

        void insert(T *item)
        {
            _values.push_back(item);
            _name_map.insert_or_assign(item->name, item);
        }

        const auto &items() const { return _values; }

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
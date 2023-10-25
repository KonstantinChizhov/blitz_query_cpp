#pragma once
#include <global_definitions.hpp>
#include <string>
#include <vector>
#include <unordered_map>

namespace blitz_query_cpp
{
    template <class T>
    class named_storage
    {
        std::unordered_map<std::string, T *> _name_map;
        std::vector<T> _values;

    public:
        named_storage()
        {
            _values.reserve(SchemaChildListReserve);
            _name_map.reserve(SchemaChildListReserve);
        }

        void clear()
        {
            _values.clear();
            _name_map.clear();
        }

        T* emplace(std::string_view name)
        {
            if(_name_map.contains(name))
            {
                return nullptr;
            }
            T& item = _values.emplace_back();
            item.name = name;
            _name_map.insert_or_assign(item.name, &item);
            return &item;
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
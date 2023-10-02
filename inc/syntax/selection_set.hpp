#pragma once
#include <global_definitions.hpp>
#include <syntax/selection.hpp>
#include <string_view>
#include <vector>

namespace blitz_query_cpp
{
    template <typename CharT>
    class selection_set :public syntax_node<CharT>
    {
    public:
        
        std::vector<selection<CharT>*> selections;
    };
}
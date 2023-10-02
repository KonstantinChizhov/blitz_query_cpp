#pragma once
#include <syntax/syntax_node.hpp>


namespace blitz_query_cpp
{
    template <typename CharT>
    class directive_node :public syntax_node<CharT>
    {
    public:
        std::basic_string_view<CharT> name;
        
    };
}
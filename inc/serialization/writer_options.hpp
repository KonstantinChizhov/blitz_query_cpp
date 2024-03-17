#pragma once
#include <stddef.h>

namespace blitz_query_cpp
{
   struct writer_options
    {
        size_t initial_capacity = 4096 * 16;
        bool format = false;
        int tab_spaces = 2;
        bool use_tabs = false;
    };
}
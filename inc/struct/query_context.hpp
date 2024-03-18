#pragma once

#include <type_system/schema.hpp>
#include <syntax/syntax_node.hpp>
#include <syntax/document.hpp>


namespace blitz_query_cpp
{
    struct query_context
    {
        const schema *schema;
        document document;
    };
}
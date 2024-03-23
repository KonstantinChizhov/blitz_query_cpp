#include "data/sql/sql_query_resolver.hpp"
#include <algorithm>
#include <ranges>

using namespace blitz_query_cpp;
namespace ranges = std::ranges;

bool sql_query_resolver::process(query_context &context)
{
    if (context.data.contains(Key))
        return true;
    auto operationIter = ranges::find_if(
        context.document.children,
        [](auto c)
        {
            return c->of_type(syntax_node_type::OperationDefinition);
        });
    if (operationIter == context.document.children.end())
        return context.report_error("Operation definition is missing");
    syntax_node *operation = *operationIter;
    switch (operation->operation_type)
    {
    case operation_type_t::Query:
        return process_query(context, operation);
    case operation_type_t::Mutation:
        return process_mutation(context, operation);
    default:
        break;
    }
    return true;
}

bool sql_query_resolver::process_query(query_context &context, syntax_node *operation)
{

    return true;
}

bool sql_query_resolver::process_mutation(query_context &context, syntax_node *operation)
{
    return true;
}
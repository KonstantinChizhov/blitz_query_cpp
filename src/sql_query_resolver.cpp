#include "processing/sql_query_resolver.hpp"
#include "data/sql/sql_expr.hpp"
#include <algorithm>
#include <ranges>

using namespace blitz_query_cpp;
using namespace blitz_query_cpp::sql;

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
    _operation = *operationIter;
    switch (_operation->operation_type)
    {
    case operation_type_t::Query:
        return process_query(context);
    case operation_type_t::Mutation:
        return process_mutation(context);
    default:
        break;
    }
    return true;
}

bool sql_query_resolver::process_field(query_context &, const field &field_decl)
{
    std::string_view column_name = field_decl.name;
    if (auto column_dir = field_decl.find_directive(ColumnDirective))
    {
        auto column_param = column_dir->parameters.find("name");
        if (column_param != column_dir->parameters.end())
            column_name = column_param->string_value;
    }

    _query |= column(_schema_name, _table_name, column_name);
    return true;
}

bool sql_query_resolver::process_query(query_context &context)
{
    if (_operation->selection_set->children.size() != 1)
        return context.report_error("Query should have only one selection set");
    auto &object = _operation->selection_set->children[0];

    auto query_type = context.schema->get_query_type();
    if (!query_type)
        return context.report_error("Query type is not defined in schema");

    auto root_selection_field = query_type->fields.find(object->name);
    if (root_selection_field == query_type->fields.end())
        return context.report_error("Field '{}' is not declared in type '{}'", object->name, query_type->name);

    const auto type = root_selection_field->field_type.type;
    if (type == nullptr)
        return context.report_error("Type of field '{}' in object '{}' is not defined", object->name, query_type->name);

    auto table_schema_dir = type->find_directive(TableDirective);

    // not SQL type, handle else where
    if (!table_schema_dir)
        return true;

    auto table_param = table_schema_dir->parameters.find("table");
    if (table_param != table_schema_dir->parameters.end())
        _table_name = table_param->string_value;
    if (_table_name.empty())
        _table_name = object->name;

    auto schema_param = table_schema_dir->parameters.find("schema");
    if (schema_param != table_schema_dir->parameters.end())
        _schema_name = schema_param->string_value;
    if (_schema_name.empty())
        _schema_name = DefaultSchama;

    _query = select();
    auto selection_set = object->selection_set;

    for (auto field_node : selection_set->children)
    {
        auto field_decl = type->fields.find(field_node->name);
        if (field_decl == type->fields.end())
            return context.report_error("Field '{}' is not found in object '{}'", field_node->name, object->name);
        if(!process_field(context, *field_decl))
            return false;
    }

    for (auto &field_decl : type->fields)
    {
        if (auto projected_dir = field_decl.find_directive(IsProjected))
        {
            auto projected = projected_dir->parameters.find("projected");
            if (projected == projected_dir->parameters.end() || projected->bool_value == true)
            {
                if(!process_field(context, field_decl))
                    return false;
            }
        }
    }

    _query |= from(table(_schema_name, _table_name));

    context.result = std::move(_query);

    return true;
}

bool sql_query_resolver::process_mutation(query_context &)
{
    return true;
}
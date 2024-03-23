#include "data/sql/postgresql_renderer.hpp"
#include <algorithm>
#include <vector>
#include <ranges>

namespace blitz_query_cpp::sql
{

    void postgresql_renderer::visit_children(expr_node &node, std::string_view separator, std::string_view end)
    {
        bool last = true;
        for (auto &&child : std::ranges::reverse_view(node.expr->children))
        {
            if (last)
            {
                if (!end.empty())
                    node_stack.emplace_back(nullptr, end);
            }
            else
            {
                if (!separator.empty())
                    node_stack.emplace_back(nullptr, separator);
            }
            node_stack.emplace_back(&child);
            last = false;
        }
    }

    void postgresql_renderer::push_value(std::string_view value)
    {
        node_stack.emplace_back(nullptr, value);
    }

    void postgresql_renderer::push_node(const sql_expr_t &node)
    {
        node_stack.emplace_back(&node);
    }

    bool postgresql_renderer::handle_binary_op(expr_node &current, std::string_view op)
    {
        if (current.expr->children.size() < 2)
            return false;
        push_value(") ");
        push_node(current.expr->children[1]);
        push_value(op);
        push_node(current.expr->children[0]);
        buffer.append(1, '(');

        return true;
    }

    void postgresql_renderer::write_quoted_value(expr_node &current)
    {
        buffer.append(1, '"');
        buffer.append(current.expr->value);
        buffer.append(1, '"');
    }

    bool postgresql_renderer::render(const sql_expr_t &expr)
    {
        push_node(expr);

        while (!node_stack.empty())
        {
            expr_node current = node_stack.back();
            node_stack.pop_back();
            if (current.expr == nullptr)
            {
                buffer.append(current.value);
                continue;
            }

            switch (current.expr->type)
            {
            case sql_expr_type::Column:
                write_quoted_value(current);
                break;
            case sql_expr_type::TableName:
                write_quoted_value(current);
                break;
            case sql_expr_type::SchemaName:
                buffer.append(1, '"');
                buffer.append(current.expr->value);
                buffer.append("\".");
                break;
            case sql_expr_type::Function:
                buffer.append(current.expr->value);
                buffer.append(1, '(');
                visit_children(current, ", ", ")");
                break;
            case sql_expr_type::StringLiteral:
                buffer.append(1, '\'');
                buffer.append(current.expr->value);
                buffer.append(1, '\'');
                break;
            case sql_expr_type::NumberLiteral:
                buffer.append(current.expr->value);
                break;
            case sql_expr_type::Parameter:
                buffer.append(1, '$');
                buffer.append(current.expr->value);
                break;
            case sql_expr_type::Asias:
            {
                std::string alias;
                alias.reserve(current.expr->value.size() + 8);
                if (current.expr->children.size() > 1)
                {
                    alias.append(")");
                    buffer.append("(");
                }
                alias.append(" as \"");
                alias.append(current.expr->value);
                alias.append(1, '"');
                visit_children(current, ", ", alias);
            }
            break;
            case sql_expr_type::Plus:
                if (!handle_binary_op(current, " + "))
                    return false;
                break;
            case sql_expr_type::Minus:
                if (!handle_binary_op(current, " - "))
                    return false;
                break;
            case sql_expr_type::Multiply:
                if (!handle_binary_op(current, " * "))
                    return false;
                break;
            case sql_expr_type::Divide:
                if (!handle_binary_op(current, " / "))
                    return false;
                break;
            case sql_expr_type::And:
                if (!handle_binary_op(current, " AND "))
                    return false;
                break;
            case sql_expr_type::Or:
                if (!handle_binary_op(current, " OR "))
                    return false;
                break;
            case sql_expr_type::Eq: // =
                if (!handle_binary_op(current, " = "))
                    return false;
                break;
            case sql_expr_type::Ne: // <>
                if (!handle_binary_op(current, " <> "))
                    return false;
                break;
            case sql_expr_type::Le: // <
                if (!handle_binary_op(current, " < "))
                    return false;
                break;
            case sql_expr_type::Gt: // >
                if (!handle_binary_op(current, " > "))
                    return false;
                break;
            case sql_expr_type::Lq: // <=
                if (!handle_binary_op(current, " <= "))
                    return false;
                break;
            case sql_expr_type::Gq: // >=
                if (!handle_binary_op(current, " >= "))
                    return false;
                break;
            case sql_expr_type::Like:
                if (!handle_binary_op(current, " LIKE "))
                    return false;
                break;

            case sql_expr_type::Select:
                buffer.append("SELECT ");
                visit_children(current);
                break;
            case sql_expr_type::SelectDistinct:
                buffer.append("SELECT DISTINCT ");
                visit_children(current);
                break;
            case sql_expr_type::From:
                buffer.append(" FROM ");
                if (current.expr->children.size() == 0)
                {
                    buffer.append(1, '"');
                    buffer.append(current.expr->value);
                    buffer.append(1, '"');
                }
                else
                    visit_children(current);
                break;
            case sql_expr_type::Insert:
                break;
            case sql_expr_type::Update:
                break;
            case sql_expr_type::Delete:
                break;
            case sql_expr_type::SelectColumns:
                visit_children(current, ", ");
                break;
            case sql_expr_type::Where:
                buffer.append(" WHERE ");
                visit_children(current, " AND ");
                break;
            case sql_expr_type::Having:
                break;
            case sql_expr_type::Group:
                break;
            case sql_expr_type::Order:
                buffer.append(" ORDER BY ");
                visit_children(current, ", ");
                break;
            case sql_expr_type::InnerJoin:
                break;
            case sql_expr_type::LeftJoin:
                break;
            case sql_expr_type::RightJoin:
                break;
            case sql_expr_type::OuterJoin:
                break;
            case sql_expr_type::On:
                break;
            case sql_expr_type::Values:
                break;
            case sql_expr_type::All:
                break;
            case sql_expr_type::Any:
                break;
            case sql_expr_type::Beetween:
                break;
            case sql_expr_type::Exists:
                break;
            case sql_expr_type::In:
                break;
            case sql_expr_type::IsNull:
                break;
            case sql_expr_type::IsNotNull:
                break;
            case sql_expr_type::Limit:
                buffer.append(" LIMIT ");
                buffer.append(current.expr->value);
                break;
            case sql_expr_type::Asc:
                buffer.append(1, '"');
                buffer.append(current.expr->value);
                buffer.append(1, '"');
                buffer.append(" ASC ");
                break;
            case sql_expr_type::Desc:
                buffer.append(1, '"');
                buffer.append(current.expr->value);
                buffer.append(1, '"');
                buffer.append(" DESC ");
                break;
            case sql_expr_type::Not:
                break;
            case sql_expr_type::Set:
                break;
            case sql_expr_type::Union:
                break;
            case sql_expr_type::UnionAll:
                break;
            case sql_expr_type::Offset:
                buffer.append(" OFFSET ");
                buffer.append(current.expr->value);
                break;
            default:
                return false;
            }
        }
        return true;
    }
}
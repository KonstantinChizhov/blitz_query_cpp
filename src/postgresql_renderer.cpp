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
        bool need_paren = current.expr->children[0].children.size() > 0 || current.expr->children[1].children.size() > 0;
        if (need_paren)
            push_value(")");
        push_node(current.expr->children[1]);
        push_value(op);
        push_node(current.expr->children[0]);
        if (need_paren)
            buffer.append(1, '(');

        return true;
    }

    bool postgresql_renderer::handle_join(expr_node &current, std::string_view join_type)
    {
        buffer.append(join_type);
        visit_children(current);
        return true;
    }

    bool pg_need_to_qoute(std::string_view value)
    {
        return !std::all_of(value.begin(), value.end(), [](char c)
                            { return (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_'; });
    }

    void postgresql_renderer::write_quoted_value(expr_node &current)
    {
        bool quote = pg_need_to_qoute(current.expr->value);
        if (quote)
            buffer.append(1, '"');
        buffer.append(current.expr->value);
        if (quote)
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
                // write column ref if any
                if (current.expr->children.size() > 0)
                {
                    buffer.append(1, '.');
                    push_node(current.expr->children[0]);
                }
                break;
            case sql_expr_type::SchemaName:
            {
                bool quote = pg_need_to_qoute(current.expr->value);
                if (quote)
                    buffer.append(1, '"');
                buffer.append(current.expr->value);
                if (quote)
                    buffer.append(1, '"');
                buffer.append(1, '.');
                if (current.expr->children.size() == 0)
                    return false;
                push_node(current.expr->children[0]);
            }
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
                alias.append(" as ");
                bool quote = pg_need_to_qoute(current.expr->value);
                if (quote)
                    alias.append(1, '"');
                alias.append(current.expr->value);
                if (quote)
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
                {
                    std::string_view end, sep = ", ";
                    if (current.expr->children[0].type == sql_expr_type::Select || current.expr->children[0].type == sql_expr_type::SelectDistinct)
                    {
                        buffer.append(1, '(');
                        end = ")";
                        sep = "), (";
                    }
                    visit_children(current, sep, end);
                }
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
                visit_children(current, " AND ");
                break;
            case sql_expr_type::Group:
                buffer.append(" GROUP BY ");
                visit_children(current, ", ");
                break;
            case sql_expr_type::Order:
                buffer.append(" ORDER BY ");
                visit_children(current, ", ");
                break;
            case sql_expr_type::InnerJoin:
                if (!handle_join(current, " INNER JOIN "))
                    return false;
                break;
            case sql_expr_type::LeftJoin:
                if (!handle_join(current, " LEFT JOIN "))
                    return false;
                break;
            case sql_expr_type::RightJoin:
                if (!handle_join(current, " RIGHT JOIN "))
                    return false;
                break;
            case sql_expr_type::OuterJoin:
                if (!handle_join(current, " FULL OUTER JOIN "))
                    return false;
                break;
            case sql_expr_type::On:
                if (current.expr->children.size() != 2)
                    return false;
                buffer.append(" ON ");
                push_node(current.expr->children[1]);
                push_value(" = ");
                push_node(current.expr->children[0]);
                break;
            case sql_expr_type::Values:
                break;
            case sql_expr_type::All:
                break;
            case sql_expr_type::Any:
                break;
            case sql_expr_type::Beetween:
                buffer.append("BEETWEEN ");
                if (current.expr->children.size() != 2)
                    return false;
                visit_children(current, " AND ");
                break;
            case sql_expr_type::Exists:
                buffer.append("EXISTS(");
                if (current.expr->children.size() != 1)
                    return false;
                visit_children(current, "", ") ");
                break;
            case sql_expr_type::In:
                buffer.append("IN( ");
                visit_children(current, ", ", ") ");
                break;
            case sql_expr_type::IsNull:
                buffer.append(" IS NULL");
                break;
            case sql_expr_type::IsNotNull:
                buffer.append(" NOT IS NULL");
                break;
            case sql_expr_type::Limit:
                buffer.append(" LIMIT ");
                buffer.append(current.expr->value);
                break;
            case sql_expr_type::Asc:
                if (current.expr->children.size() == 0)
                {
                    buffer.append(1, '"');
                    buffer.append(current.expr->value);
                    buffer.append(1, '"');
                    buffer.append(" ASC");
                }
                else
                {
                    push_value(" ASC");
                    visit_children(current);
                }
                break;
            case sql_expr_type::Desc:
                if (current.expr->children.size() == 0)
                {
                    buffer.append(1, '"');
                    buffer.append(current.expr->value);
                    buffer.append(1, '"');
                    buffer.append(" DESC");
                }
                else
                {
                    push_value(" DESC");
                    visit_children(current);
                }
                break;
            case sql_expr_type::Not:
                buffer.append(" NOT ");
                if (current.expr->children.size() != 1)
                    return false;
                push_node(current.expr->children[0]);
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
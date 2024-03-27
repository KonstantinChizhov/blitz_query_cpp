
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "data/sql/sql_expr.hpp"
#include "data/sql/postgresql_renderer.hpp"

using namespace blitz_query_cpp::sql;

std::string render(const sql_expr_t &expr)
{
    postgresql_renderer renderer;
    renderer.render(expr);
    std::string result = renderer.get_string();
    return result;
}

TEST(Sql, SelectFromTable)
{
    EXPECT_EQ(render(
                  select({"id", "name", "age"}) | from("users")),
              "SELECT \"id\", \"name\", \"age\" FROM \"users\"");

    auto query2 = select({"id", "name"});
    query2 = query2 | from("users");
    EXPECT_EQ(render(query2), "SELECT \"id\", \"name\" FROM \"users\"");
}

TEST(Sql, SelectFromSubQuery)
{
    auto query =
        select({"id", "name"}) | from(
                                     select({"id", "name", "age"}) | from("users"));

    EXPECT_EQ(render(query), "SELECT \"id\", \"name\" FROM (SELECT \"id\", \"name\", \"age\" FROM \"users\")");
}

TEST(Sql, SelectWhere)
{
    auto query1 = select({"id", "name"}) | from("users") | where(binary_operation("age", binary_op::Gt, 18));
    EXPECT_EQ(render(query1), "SELECT \"id\", \"name\" FROM \"users\" WHERE \"age\" > 18");

    auto query2 = select({"id", "name"}) | from("users") | where(binary_operation("name", binary_op::Like, "M%"));
    EXPECT_EQ(render(query2), "SELECT \"id\", \"name\" FROM \"users\" WHERE \"name\" LIKE 'M%'");
}

TEST(Sql, SelectOffsetLimit)
{
    auto query1 = select({"id", "name"}) 
        | from("users") 
        | offset(10)
        | limit(20);

    EXPECT_EQ(render(query1), "SELECT \"id\", \"name\" FROM \"users\" OFFSET 10 LIMIT 20");
}

TEST(Sql, SelectOrderBy)
{
    auto query1 = select({"id", "name"}) 
        | from("users") 
        | order_by("name", false);

    EXPECT_EQ(render(query1), "SELECT \"id\", \"name\" FROM \"users\" ORDER BY \"name\" DESC");
}

TEST(Sql, SelectCombined)
{
    auto query1 = select({"id", "name"}) 
        | from("users") 
        | where(binary_operation("age", binary_op::Gt, 18))
        | or_else(binary_operation("name", binary_op::Like, "M%"))
        | offset(10)
        | limit(20)
        | order_by("name", false)
        | order_by("age", true)
        ;

    EXPECT_EQ(render(query1), "SELECT \"id\", \"name\" FROM \"users\" WHERE (\"age\" > 18 OR \"name\" LIKE 'M%') ORDER BY \"name\" DESC, \"age\" ASC OFFSET 10 LIMIT 20");
}
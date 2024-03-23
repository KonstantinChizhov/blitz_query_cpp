
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "data/sql/sql_expr.hpp"
#include "data/sql/postgresql_renderer.hpp"

using namespace blitz_query_cpp::sql;

TEST(Sql, SimpleSelect)
{
    auto res = select({"id", "name", "age"})
    .from("users")
    .where("age", binary_op::Gt, 18L)
    .where("name", binary_op::Like, "M%")
    .order_by("name")
    .limit(10)
    .offset(20);


    postgresql_renderer renderer;
    renderer.render(res);
    std::cout << "SQL: " << renderer.get_string() << std::endl;
}


  

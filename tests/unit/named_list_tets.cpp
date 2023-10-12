#include <util/named_list.hpp>
#include <gtest/gtest.h>
#include <gmock/gmock.h>


using namespace blitz_query_cpp;

struct test_entity
{
   std::string name;
   int value;
};

TEST(NamedList, Use)
{
   named_list<test_entity> entity_list;
   test_entity & o1 = entity_list.add_new("foo");
   test_entity & o2 = entity_list.add_new("bar");

   o1.value = 42;
   o2.value = 333;

   auto val = entity_list.find("bar");
   ASSERT_NE(val, nullptr);
   EXPECT_EQ(val->value, 333);

   val = entity_list.find("not exists");
   EXPECT_EQ(val, nullptr);

   ASSERT_EQ( entity_list.items().size(), 2);
   EXPECT_EQ(entity_list.items()[0].name, "foo");
   EXPECT_EQ(entity_list.items()[1].name, "bar");
   
}
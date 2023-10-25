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

   test_entity o1{"foo", 42};
   entity_list.insert(&o1);
   test_entity o2 {"bar", 333};
   entity_list.insert(&o2);

   auto val = entity_list.find("bar");
   ASSERT_NE(val, nullptr);
   EXPECT_EQ(val->value, 333);

   val = entity_list.find("not exists");
   EXPECT_EQ(val, nullptr);

   ASSERT_EQ( entity_list.items().size(), 2);
   EXPECT_EQ(entity_list.items()[0]->name, "foo");
   EXPECT_EQ(entity_list.items()[1]->name, "bar");
   
}
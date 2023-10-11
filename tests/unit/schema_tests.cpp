#include <type_system/type_system_classes.hpp>
#include <gtest/gtest.h>
#include <gmock/gmock.h>


using namespace blitz_query_cpp;

TEST(Schema, NameHash)
{
   type_system_object o1;
   type_system_object o2;
   o1.name = "Foo";
   o2.name = "Bar";
   
}
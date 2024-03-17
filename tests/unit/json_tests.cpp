#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "serialization/buffer_writer.hpp"
#include "serialization/json/json_serializer.hpp"

using namespace blitz_query_cpp;

TEST(Json, BasicWrite)
{
    writer_options options;
    options.format = true;
    json_writer<buffer_writer> writer(options);
    writer.begin_object();
    writer.write_value("int_value", 123);
    writer.write_value("float_value", 123.456);
    writer.write_str_value("str_value", "hello\r\nworld!!");
    writer.write_array("int_array", {1, 2, 3, 4, 5, 6});
    writer.write_array<std::string_view>("str_array", {"a", "b", "c"});
    writer.begin_object("obj_value");
    writer.write_value("int_value", 42);
    writer.end_object();
    writer.write_value("bool_value", true);
    writer.end_object();

    std::cout << writer.get_string() << std::endl;
}
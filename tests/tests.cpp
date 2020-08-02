#include "../include/ghassanpl/error_handler.h"
#include "../include/ghassanpl/multi_handler.h"

#include <string>
#include <gtest/gtest.h>

using namespace ghassanpl::error_handling;

void read(int from, error_handler_handle<int> handler = {})
{
	handler.error(from);
}

TEST(error_handler, return_values)
{
	error_values<int> ret;
	read(5, ret);
	EXPECT_EQ(std::get<0>(ret.values), 5);
}

TEST(error_handler, throws)
{
	EXPECT_THROW(read(5), generic_value_exception<int>);
}


void read(int from, std::string str, multi_error_handler_handle<error_handler<int>, error_handler<std::string>> handler = {})
{
	handler.error(from);
	handler.error("hello");
	handler.error(typeid(from));
}

TEST(multi_error_handler, throws)
{
	
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
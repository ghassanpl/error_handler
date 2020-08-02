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
	EXPECT_EQ(get<0>(ret), 5);
}

TEST(error_handler, throws)
{
	EXPECT_THROW(read(5), generic_value_exception<int>);
}

TEST(error_handler, chainable)
{
	auto read1 = [](error_handler_handle<int> handler) {
		handler.error(10);
	};
	auto read2 = [&read1](error_handler_handle<int> handler) {
		read1(handler);
	};
	EXPECT_THROW(read2({}), generic_value_exception<int>);
}

TEST(multi_error_handler, throws)
{
	auto read = [](int from, std::string str, multi_error_handler_handle<error_handler<int>, error_handler<std::string>> handler = {}) {
		if (from == 0)
			handler.error(from);
		else
			handler.error("hello");
	};
	EXPECT_THROW(read(0, "hello"), generic_value_exception<int>);
	EXPECT_THROW(read(5, "hello"), generic_value_exception<std::string>);

	error_values<int> e1;
	error_values<std::string> e2;
	read(0, "hello", { e1, e2 });
	EXPECT_EQ(get<0>(e1), 0);
	read(5, "hello", { e1, e2 });
	EXPECT_EQ(get<0>(e2), "hello");
}

TEST(multi_error_handler, chainable)
{
	using handle_type = multi_error_handler_handle<error_handler<int>, error_handler<std::string>>;
	auto read1 = [](handle_type handler) {
		handler.error(10);
	};
	auto read2 = [&read1](handle_type handler) {
		handler.error("hello");
		read1(handler);
	};
	error_values<int> e1;
	error_values<std::string> e2;
	read2({ e1, e2 });
	EXPECT_EQ(get<0>(e1), 10);
	EXPECT_EQ(get<0>(e2), "hello");
}

TEST(multi_error_handler, functions_convert_arguments)
{
	using multi_handler_type = multi_error_handler_handle<error_handler<int>, error_handler<std::string>>;

	EXPECT_TRUE((can_error_with<multi_handler_type, int>));
	EXPECT_TRUE((can_error_with<multi_handler_type, double>));
	EXPECT_TRUE((can_error_with<multi_handler_type, decltype("hello")>));
}

TEST(multi_error_handler, invalid_arguments_are_invalid)
{
	using multi_handler_type = multi_error_handler_handle<error_handler<int>, error_handler<std::string>>;
	EXPECT_FALSE((can_error_with<multi_handler_type, std::type_info>));
}



int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
//
// Created by Isa on 4/2/21.
//

#include "test_utils.h"

#define tag "[emplace]"

pair<string const, string> rand_pair_ss(string no = string("this will never be inserted"))
{
	string s1 = rand_string();
	string s2 = rand_string();
	while(1)
	{
		if(s1 != no && s2 != no)
		{
			break;
		}
		s1 = rand_string();
		s2 = rand_string();
	}

	return make_pair(std::move(s1), std::move(s2));
}

TEST_CASE("emplace single element", tag)
{
	SECTION("from pair lvalue")
	{
		my_map<int, string> act;
		std_map<int, string> exp;

		pair<int const, string> const val = rand_pair();

		auto p1 = act.emplace(val);
		auto p2 = exp.emplace(val);

		REQUIRE(p1.first == act.begin());
		REQUIRE(p2.first == exp.begin());
		REQUIRE(p1.second == p2.second);

		REQUIRE(*act.begin() == val);
		REQUIRE(*exp.begin() == val);

		MAPS_REQUIRE_EQUAL(act, exp);
	}
	SECTION("from pair rvalue")
	{
		CREATE_MAPS_INT_STRING(act, exp);

		pair<int const, string> val1(rand_int(1001, 2000), rand_string());
		pair<int const, string> val2 = val1;

		auto p1 = act.emplace(std::move(val1));
		auto p2 = exp.emplace(std::move(val2));

		REQUIRE(*p1.first == *p2.first);
		REQUIRE(p1.second == p2.second);
		REQUIRE(val1 == val2);

		MAPS_REQUIRE_EQUAL(act, exp);
	}
	SECTION("from pair ctor rvalue args")
	{
		CREATE_MAPS_INT_STRING(act, exp);

		pair<int, string> val1(rand_int(1200, 2000), rand_string());
		pair<int, string> val2 = val1;

		auto p1 = act.emplace(std::move(val1.first), std::move(val1.second));
		auto p2 = exp.emplace(std::move(val2.first), std::move(val2.second));

		REQUIRE(*p1.first == *p2.first);
		REQUIRE(p1.second == p2.second);
		REQUIRE(p1.second == true);
		REQUIRE(val1 == val2);

		MAPS_REQUIRE_EQUAL(act, exp);
	}
	SECTION("piecewise_construct")
	{
		CREATE_MAPS_INT_STRING(act, exp);

		pair<int, string> val1(rand_int(1200, 2000), rand_string());
		pair<int, string> val2 = val1;

		auto p1 = act.emplace(piecewise_construct, forward_as_tuple(std::move(val1.first)), forward_as_tuple(std::move(val1.second)));
		auto p2 = exp.emplace(piecewise_construct, forward_as_tuple(std::move(val2.first)), forward_as_tuple(std::move(val2.second)));

		REQUIRE(*p1.first == *p2.first);
		REQUIRE(p1.second == p2.second);
		REQUIRE(p1.second == true);
		REQUIRE(val1 == val2);

		MAPS_REQUIRE_EQUAL(act, exp);
	}
	SECTION("from pair's elements ctor args")
	{
		string key = "string as key";
		string value = "string as value";

		initializer_list<pair<string const, string>> il = {rand_pair_ss(key), rand_pair_ss(key), rand_pair_ss(key), rand_pair_ss(key), rand_pair_ss(key), rand_pair_ss(key), rand_pair_ss(key)};
		my_map<string, string> act = il;
		std_map<string, string> exp = il;

		pair<string, string> val1(key, value);
		pair<string, string> val2 = val1;

		auto p1 = act.emplace(std::move(val1.first), val1.second.c_str());
		auto p2 = exp.emplace(std::move(val2.first), val2.second.c_str());

		REQUIRE(*p1.first == *p2.first);
		REQUIRE(p1.second == p2.second);
		REQUIRE(val1 == val2);

		MAPS_REQUIRE_EQUAL(act, exp);
	}
}

TEST_CASE("emplace with hint", tag)
{

}
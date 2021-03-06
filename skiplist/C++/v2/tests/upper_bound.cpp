//
// Created by Isa on 4/2/21.
//

#include "test_utils.h"

#define tag "[upper_bound]"

TEST_CASE("uppper bound search", tag)
{
	my_map<string, int> const act(vec_pairs.begin(), vec_pairs.end());
	std_map<string, int> const exp(vec_pairs.begin(), vec_pairs.end());

	SECTION("key does not exist and is greater than all existing")
	{
		auto it1 = act.upper_bound("zz");
		auto it2 = exp.upper_bound("zz");

#ifdef TEST_CMP_GREATER
		auto expected_iterator1 = act.begin();
		auto expected_iterator2 = exp.begin();
#else
		auto expected_iterator1 = act.end();
		auto expected_iterator2 = exp.end();
#endif
		REQUIRE(it1 == expected_iterator1);
		REQUIRE(it2 == expected_iterator2);
		MAPS_REQUIRE_EQUAL(act, exp);
	}
	SECTION("key does not exists but it is not max")
	{
		auto it1 = act.upper_bound("aa");
		auto it2 = exp.upper_bound("aa");

#ifdef TEST_CMP_GREATER
		REQUIRE(it1 == act.end());
		REQUIRE(it2 == exp.end());
#else
		REQUIRE(*it1 == vec_pairs[5]);
		REQUIRE(*it1 == *it2);
#endif

		MAPS_REQUIRE_EQUAL(act, exp);
	}
	SECTION("key exists then return it's next")
	{
		auto it1 = act.upper_bound("ab");
		auto it2 = exp.upper_bound("ab");

#ifdef TEST_CMP_GREATER
		REQUIRE(it1 == act.end());
		REQUIRE(it2 == exp.end());
#else
		REQUIRE(*it1 == *it2);
#endif

		MAPS_REQUIRE_EQUAL(act, exp);
	}
	SECTION("key exists and is max")
	{
		auto it1 = act.upper_bound((--act.end())->first);
		auto it2 = exp.upper_bound((--exp.end())->first);

		REQUIRE(it1 == act.end());
		REQUIRE(it2 == exp.end());
		MAPS_REQUIRE_EQUAL(act, exp);
	}

}
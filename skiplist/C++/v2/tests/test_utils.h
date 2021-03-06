//
// Created by Isa on 3/30/21.
//

#ifndef _TEST_UTILS_H
#define _TEST_UTILS_H
#define CATCH_CONFIG_ENABLE_BENCHMARKING

#include <ctime>
#include <cstdlib>
#include <climits>


//#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch2/catch.hpp"
#include <string>
#include <initializer_list>

#include <iostream>
#include <stack>
#include <map>
#include <smap>
#include <set>

#define TEST_CMP_GREATER
#define TEST_STD_ALLOCATOR

template<typename T>
#ifdef TEST_CMP_GREATER
using Test_Comparator = std::greater<T>;
#else
using Test_Comparator = std::less<T>;
#endif

template<typename T>
using Test_Allocator = std::allocator<T>;

template<typename Key, typename Tp, typename Compar = Test_Comparator<Key>, typename Alloc = Test_Allocator<std::pair<Key const, Tp>>>
using my_map = isa::smap<Key, Tp, Compar, Alloc>;

template<typename Key, typename Tp, typename Compar = Test_Comparator<Key>, typename Alloc = Test_Allocator<std::pair<Key const, Tp>>>
using std_map = std::map<Key, Tp, Compar, Alloc>;

using namespace std;

// INPUTS
int rand_int(int min, int max);
char rand_char();
std::string rand_string();
void do_random_insertions_and_deletions(my_map<int, std::string>& my_map, std_map<int, std::string>& std_map);

template<typename I>
void print(I f, I l)
{
	int i = 0;
	while(f != l)
	{
		std::cout << i << "\t:\t" << "(H = ";
#ifdef SKIPLIST_DEBUG_INFO
		std::cout << f.nodeptr->height;
#endif
		std::cout << ", " << f->first << ", " << f->second << ")" << std::endl;
		++f;
		++i;
	}
}

int print(my_map<int, std::string>& m);


#define rand_pair() std::make_pair<int const, std::string>(rand_int(0, 1000), rand_string())
#define rand_pairs {rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair()}
#define rand_pairs_L {rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), \
					 rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair(), rand_pair()}

#define CREATE_MAPS_INT_STRING(name1, name2) \
	std::initializer_list<std::pair<int, string>> ___init_list_input___##name1##name2 = rand_pairs; \
	my_map<int, string> name1(___init_list_input___##name1##name2.begin(), ___init_list_input___##name1##name2.end()); \
	std_map<int, string> name2(___init_list_input___##name1##name2.begin(), ___init_list_input___##name1##name2.end()); \
	MAPS_REQUIRE_EQUAL(name1, name2)

#define MAPS_REQUIRE_EQUAL(my_map, std_map) (verify((my_map), (std_map)))
#define CHECK_MAPS_NEQ(gcc_list, std_list) (check_neq((gcc_list), (std_list)))
#define DO_RANDOM_OPERATIONS(my_map, std_map) (do_random_insertions_and_deletions((my_map), (std_map)))

extern const std::initializer_list<std::pair<int const, std::string>> init_list_1;
extern const std::initializer_list<std::pair<int const, std::string>> init_list_2;
extern const std::initializer_list<std::pair<int const, std::string>> init_list_L;

extern const std::vector<std::pair<std::string const, int>> vec_pairs;

extern const std::initializer_list<std::pair<int const, std::string>> initializer_list_pairs_L;
extern const std::initializer_list<std::pair<int const, std::string>> initializer_list_pairs;

std::set<std::pair<int const, std::string>> rand_pairs_of_len(int len);


template<typename Key, typename Tp, typename Cmp, typename Alloc>
void verify(my_map<Key, Tp, Cmp, Alloc> const& my_map, std_map<Key, Tp, Cmp, Alloc> const& std_map)
{
	REQUIRE(my_map.size() == std_map.size());
#ifdef TEST_STD_ALLOCATOR
	REQUIRE(my_map.get_allocator() == std_map.get_allocator());
#else
#endif
//	REQUIRE(my_map.max_size() == std_map.max_size());

	REQUIRE(my_map.empty() == std_map.empty());

	auto act = my_map.begin();
	auto exp = std_map.begin();
	auto end = std_map.end();

	while(act != my_map.end())
	{
		REQUIRE(act->first == exp->first);
		REQUIRE(act->second == exp->second);

		++act;
		++exp;
	}

	auto ract = my_map.rbegin();
	auto rexp = std_map.rbegin();

	while(ract != my_map.rend())
	{
		REQUIRE(ract->first == rexp->first);
		REQUIRE(ract->second == rexp->second);

		++ract;
		++rexp;
	}
}

template<typename Key, typename Tp, typename Cmp, typename Alloc>
void verify(my_map<Key, Tp, Cmp, Alloc>& my_map, ::my_map<Key, Tp, Cmp, Alloc>& my_map_2)
{
	REQUIRE(my_map.size() == my_map_2.size());
#ifdef TEST_STD_ALLOCATOR
	REQUIRE(my_map.get_allocator() == my_map_2.get_allocator());
#else
#endif
//	REQUIRE(my_map.max_size() == my_map_2.max_size());

	auto act = my_map.begin();
	auto exp = my_map_2.begin();

	while(act != my_map.end())
	{
		REQUIRE(act->first == exp->first);
		REQUIRE(act->second == exp->second);

		++act;
		++exp;
	}

	auto ract = my_map.rbegin();
	auto rexp = my_map_2.rbegin();

	while(ract != my_map.rend())
	{
		REQUIRE(ract->first == rexp->first);
		REQUIRE(ract->second == rexp->second);

		++ract;
		++rexp;
	}
}


template<typename Key, typename Tp, typename Cmp, typename Alloc>
bool check_neq(my_map<Key, Tp, Cmp, Alloc>& my_map, std_map<Key, Tp, Cmp, Alloc>& std_map)
{
	if(my_map.size() != std_map.size())
		return true;

	auto act = my_map.begin();
	auto exp = std_map.begin();

	bool full_match = true;
	while(act != my_map.end())
	{
		if(act->first != exp->first || act->second != exp->second)
		{
			full_match = false;
			break;
		}

		++act;
		++exp;
	}

	CHECK(not full_match);
	return not full_match;
}



#endif //_TEST_UTILS_H

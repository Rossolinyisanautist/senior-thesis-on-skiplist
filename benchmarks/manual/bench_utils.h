//
// Created by Isa on 4/9/21.
//

#ifndef _BENCH_UTILS_H
#define _BENCH_UTILS_H

//#include <benchmark/benchmark.h>

#define TMP_NAMESPACE_FIX

#include <map>
#include <smap>


template<typename K>
using Bench_Comparator = std::less<K>;

template<typename T>
using Bench_Allocator = std::allocator<T>;

template<typename K, typename V, typename K_Cmp>
struct pair_cmp
{
	K_Cmp key_comp;

	bool operator()(std::pair<K const, V> const& a, std::pair<K const, V> const& b)
	{
		return key_comp(a.first, b.first);
	}
};

template<typename I>
I advance(I iter, I max, int n)
{
	while (n-- && iter != max)
	{
		++iter;
	}
	return iter;
}

static void escape(void* p)
{
	asm volatile("" : : "g"(p) : "memory");
}

static void escape(void const* p)
{
	asm volatile("" : : "g"(const_cast<void*> (p)) : "memory");
}

static void clobber()
{
	asm volatile("" : : : "memory");
}

using ss_pair_cmp = pair_cmp<std::string, std::string, Bench_Comparator<std::string>>;


int rand_int(int min, int max);

char rand_char();

std::string rand_string(int min_len, int max_len);

std::vector<std::pair<std::string, std::string const>> generate_unsorted_range(int N, int strlen);

std::vector<std::pair<std::string const, std::string const>> generate_sorted_range(int N, int strlen);

enum {
	_THOUSAND = 1000,
	_10_THOUSANDS = 10000,
	_100_THOUSANDS = 100000,
	_MILLION = 1000000,
	_10_MILLIONS = 10000000
};

#define Send_Args() DenseRange(1, _MILLION + 1, _10_THOUSANDS)
//#define Send_Args() RangeMultiplier(2)->Range(8, 8<<11)->Iterations(150)

#endif //_BENCH_UTILS_H

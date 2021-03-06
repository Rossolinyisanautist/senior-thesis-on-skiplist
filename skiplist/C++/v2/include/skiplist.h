//
// Created by Isa on 3/26/21.
//
#pragma once
#ifndef _SKIPLIST_H
#define _SKIPLIST_H

#include <iterator>
#include <algorithm>
#include "skiplist_base.h"
#include "skiplist_iterator.h"
#include "container_utils.h"

namespace isa
{

	template
		<
			typename Key,
			typename Tp,
			typename Compare = std::less<Key>,
			typename Alloc = std::allocator<std::pair<Key const, Tp>>
		>
	class smap : public smap_base<Key, Tp, Alloc, Compare>
	{
		using base = smap_base<Key, Tp, Alloc, Compare>;

		using node_alloc_type = typename base::node_alloc_type;
		using node_alloc_traits = typename base::node_alloc_traits;

		using node = typename base::node;
		using node_pointer = typename base::node_pointer;
		using node_const_pointer = typename base::node_const_pointer;

		using pair_type = typename base::pair_type;
		using mutable_key_pair = typename base::mutable_key_pair;
		using pair_compare_type = typename base::pair_comparator_type;

	public:
		using key_type = Key;
		using mapped_type = Tp;
		using value_type = pair_type;
		using key_compare = Compare;
		using value_compare = pair_compare_type;
		using allocator_type = Alloc;

		using reference = value_type&;
		using const_referece = value_type const&;

		using pointer = typename std::allocator_traits<allocator_type>::pointer; // ptr to pair_type
		using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;

		using iterator = skiplist_iterator<Key, Tp, Compare, Alloc>;
		using const_iterator = skiplist_const_iterator<Key, Tp, Compare, Alloc>;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;
	protected:
		using insert_return_type = std::pair<iterator, bool>;

	public:

		using difference_type = typename std::iterator_traits<iterator>::difference_type;
		using size_type = std::size_t;

		// iterator getters
		iterator begin() noexcept
		{
			return iterator(base::m_head.m_next[0]);
		}

		const_iterator begin() const noexcept
		{
			return const_iterator(base::m_head.m_next[0]);
		}

		iterator end() noexcept
		{
			return iterator(&this->base::m_head);
		}

		const_iterator end() const noexcept
		{
			return const_iterator(&this->base::m_head);
		}

		reverse_iterator rbegin() noexcept
		{
			return reverse_iterator(end());
		}

		const_reverse_iterator rbegin() const noexcept
		{
			return const_reverse_iterator(end());
		}

		reverse_iterator rend() noexcept
		{
			return reverse_iterator(begin());
		}

		const_reverse_iterator rend() const noexcept
		{
			return const_reverse_iterator(begin());
		}

		const_iterator cbegin() const noexcept
		{
			return const_iterator(base::m_head.m_next[0]);
		}

		const_iterator cend() const noexcept
		{
			return const_iterator(&this->base::m_head);
		}

		const_reverse_iterator crbegin() const noexcept
		{
			return const_reverse_iterator(end());
		}

		const_reverse_iterator crend() const noexcept
		{
			return const_reverse_iterator(begin());
		}

		// constructors

		// empty ctors
		smap() = default;

		explicit smap(key_compare const& comp)
			: base(comp)
		{
		}

		explicit smap(allocator_type const& alloc)
			: base(alloc)
		{
		}

		smap(key_compare const& comp, allocator_type const& alloc)
			: base(comp, alloc)
		{
		}

		// copy ctors
		smap(smap const& other)
			: base(other.get_pair_comparator(), node_alloc_traits::select_on_container_copy_construction(other.get_node_allocator()))
		{
			// 100% sorted
			_p_range_construct(other.begin(), other.end());
		}

		smap(smap const& other, allocator_type const& alloc)
			: base(other.get_pair_comparator(), alloc)
		{
			// 100% sorted
			_p_range_construct(other.begin(), other.end());
		}

		// move ctors
		smap(smap&& rval) = default;

	protected:
		smap(smap&& rval, allocator_type const& alloc, std::true_type)
			: base(std::move(rval), alloc)
		{
		}

		smap(smap&& rval, allocator_type const& alloc, std::false_type)
			: base(alloc)
		{
			if(rval.get_node_allocator() == this->get_node_allocator())
			{
				base::move_head(std::move(rval));
			}
			else
			{
				auto mbegin = utils::make_move_iterator_if_noexcept(rval.begin());
				auto mend = utils::make_move_iterator_if_noexcept(rval.end());

				_p_range_construct(mbegin, mend);
			}
		}

	public:
		smap(smap&& rval, allocator_type const& alloc)
			: smap(std::move(rval), alloc, typename node_alloc_traits::is_always_equal())
		{
		}

		// TODO: default args must be default constructed (not default constructed and then copied)
		template<typename Input_iterator, typename = utils::require_input_iter<Input_iterator>>
		smap(Input_iterator first, Input_iterator last, key_compare const& comp = key_compare(), allocator_type const& alloc = allocator_type())
			: base(comp, alloc)
		{
			// not 100% sorted
			_p_range_construct(first, last);
		}

		// initializer list
		smap(std::initializer_list<value_type> il, key_compare const& comp, allocator_type const& alloc)
			: base(comp, alloc)
		{
			_p_range_construct(il.begin(), il.end());
		}

		smap(std::initializer_list<value_type> il, key_compare const& comp)
			: base(comp)
		{
			_p_range_construct(il.begin(), il.end());
		}

		smap(std::initializer_list<value_type> il)
			: base()
		{
			_p_range_construct(il.begin(), il.end());
		}

		// assignment operators
		smap& operator=(smap const& other)
		{
			if(this != std::addressof(other))
			{
				if(base::node_alloc_traits::propagate_on_container_copy_assignment::value)
				{
					node_alloc_type this_alloc = this->get_node_allocator();
					node_alloc_type other_alloc = other.get_node_allocator();

					if(!base::node_alloc_traits::is_always_equal::value &&
						this_alloc != other_alloc)
					{
						// replacement allocator cannot free existing storage
						this->clear();
					}
					utils::copy_alloc_if_pocca(this_alloc, other_alloc);
				}
				base::m_pair_comparator = other.get_pair_comparator();
				_p_range_assign_dispatch(other.begin(), other.end(), other.size());
			}

			return *this;
		}

		smap& operator=(smap&& rval)
		{
			base::m_pair_comparator = std::move(rval.get_pair_comparator());
			constexpr bool steal_nodes = base::node_alloc_traits::propagate_on_container_move_assignment::value || base::node_alloc_traits::is_always_equal::value;
			_p_move_assign(std::move(rval), std::integral_constant<bool, steal_nodes>());
			return *this;
		}

		smap& operator=(std::initializer_list<value_type> il)
		{
			smap tmp(il, base::get_key_comparator(), base::get_node_allocator()); // O(N log(N)) and heap allocations. O(N) if sorted
			if(!tmp.empty())
			{
				*this = std::move(tmp); // O(N) deallocations
			}

			return *this;
		}

		// subscript operator
		mapped_type& operator[](key_type const& key)
		{
			return base::find_or_insert(key);
		}

		mapped_type& operator[](key_type&& key)
		{
			return base::find_or_insert(std::move(key));
		}

		mapped_type& at(key_type const& key)
		{
			node_pointer node = base::find_node(key);
			if(node->baseptr() == base::m_head.npos() || !base::equals(*node->keyptr(), key))
			{
				throw std::out_of_range("smap::at: key not found");
			}

			return node->dataptr()->second;
		}

		mapped_type const& at(key_type const& key) const
		{
			node_const_pointer node = base::find_node(key);
			if(node->baseptr() == base::m_head.npos() || !base::equals(*node->keyptr(), key))
			{
				throw std::out_of_range("smap::at: key not found");
			}

			return node->dataptr()->second;
		}

		// insert functions
		insert_return_type insert(value_type const& element)
		{
			typename base::insert_return_t res = base::append_or_insert(element);
			return insert_return_type (iterator(res.first), res.second);
		}

		template <class Pair,
				typename = utils::require_constructible<value_type, Pair>
			>
		insert_return_type insert(Pair&& val)
		{
			typename base::insert_return_t res = base::append_or_insert(std::forward<Pair>(val));
			return insert_return_type (iterator(res.first), res.second);
		}

		// insert with hint
		iterator insert(const_iterator hint, value_type const& val)
		{
			// TODO: figure out how to implement
		}

		template<typename Pair,
				typename = utils::require_constructible<value_type, Pair>
			>
		iterator insert(const_iterator hint, Pair&& val)
		{
			// TODO: figure out how to implement
		}

		template<typename Input_iterator, typename = utils::require_input_iter<Input_iterator>>
		void insert(Input_iterator first, Input_iterator last)
		{
			_p_range_insert(first, last);
		}

		void insert(std::initializer_list<value_type> il)
		{
			_p_range_insert(il.begin(), il.end());
		}


		// erase functions
		iterator erase(const_iterator position)
		{
			return iterator(base::remove_node(position.nodeptr));
		}

		size_type erase(key_type const& key)
		{
			return base::remove_key(key);
		}

		// if first == last then is it UB?
		iterator erase(const_iterator first, const_iterator last)
		{
			if(first == begin() && last == end())
			{
				clear();
				return end();
			}
			if(last == end())
			{
				return iterator(base::truncate_tail(first.nodeptr));
			}
			else
			{
				return iterator(base::remove_range(first.nodeptr, last.nodeptr));
			}
		}

		template<typename... Args>
		insert_return_type emplace(Args&&... args)
		{
			typename base::insert_return_t res = base::append_or_insert_from_args(std::forward<Args>(args)...);
			return insert_return_type(iterator(res.first), res.second);
		}

		template<typename... Args>
		iterator emplace_hint(const_iterator hint, Args&&... args)
		{
			// TODO: implement
		}

		// search
		iterator find(key_type const& key)
		{
			node_pointer pos = base::find_node(key);
			if(base::equals(*pos->keyptr(), key))
			{
				return iterator(pos);
			}
			return end();
		}

		const_iterator find(key_type const& key) const
		{
			node_const_pointer pos = base::find_node(key);
			if(base::equals(*pos->keyptr(), key))
			{
				return const_iterator(pos);
			}
			return cend();
		}

		size_type count(key_type const& key) const
		{
			return base::count_key(key);
		}

		iterator lower_bound(key_type const& key)
		{
			return iterator(base::find_node(key));
		}

		const_iterator lower_bound(key_type const& key) const
		{
			return const_iterator(base::find_node(key));
		}

		iterator upper_bound(key_type const& key)
		{
			node_pointer node = base::find_node(key);
			if(node->baseptr() != base::m_head.npos() && base::equals(*node->keyptr(), key))
			{
				return iterator(node->m_next[0]);
			}

			return iterator(node);
		}

		const_iterator upper_bound(key_type const& key) const
		{
			node_const_pointer node = base::find_node(key);
			if(node->baseptr() != base::m_head.npos() && base::equals(*node->keyptr(), key))
			{
				return const_iterator(node->m_next[0]);
			}

			return const_iterator(node);
		}

		std::pair<iterator, iterator> equal_range(key_type const& key)
		{
			return std::pair<iterator, iterator> (lower_bound(key), upper_bound(key));
		}

		std::pair<const_iterator, const_iterator> equal_range(key_type const& key) const
		{
			return std::pair<const_iterator, const_iterator> (lower_bound(key), upper_bound(key));
		}

		void swap(smap& other)
		{
			if(this != std::addressof(other))
			{
				base::swap_head(other);
				std::swap(this->m_pair_comparator, other.m_pair_comparator);
				utils::swap_alloc_if_pocs(this->m_node_allocator, other.m_node_allocator);
			}
		}

		// alloc and compar
		key_compare key_comp() const noexcept
		{
			return base::get_key_comparator();
		}

		value_compare value_comp() const noexcept
		{
			return base::get_pair_comparator();
		}

		allocator_type get_allocator() const noexcept
		{
			return base::get_node_allocator();
		}

		// capacity
		size_type max_size() const
		{
			return node_alloc_traits::max_size(base::get_node_allocator());
		}

		constexpr size_type size() const
		{
			return base::length();
		}

		bool empty() const
		{
			return base::is_empty();
		}

		// dtor
		~smap() noexcept = default;

		void clear() noexcept
		{
			base::clear_nodes();
			base::init_head();
		}

	protected:

		template<typename Input_iterator>
		void _p_range_insert(Input_iterator first, Input_iterator last)
		{
			while(first != last)
			{
				base::append_or_insert(*first);
				++first;
			}
		}

		template<typename Input_iterator>
		inline void _p_range_construct(Input_iterator first, Input_iterator last)
		{
			// in case when N is known we may balance H to be exactly logN
			// and also perfectly distribute nodes across levels
			// but it can lead to performance degradation
			// in further insert/delete operations
			_p_range_insert(first, last);
		}

		template<typename Input_iterator>
		void _p_range_assign_dispatch(Input_iterator first, Input_iterator last, size_type const count)
		{
/*			constexpr bool const shorter = count < this->size();
			_p_range_assign(first, last, count, std::integral_constant<bool, shorter>());*/

			if(count < size())
			{
				_p_range_assign_shorter(first, last, count);
			}
			else
			{
				_p_range_assign_longer_or_equal(first, last);
			}
		}

		template<typename Input_iterator>
		void _p_range_assign_equal(Input_iterator& first, Input_iterator& last)
		{
			iterator this_first = this->begin();
			iterator this_last = this->end();

			while(first != last &&
			this_first != this_last)
			{
				base::assign_pair(this_first.nodeptr, *first);

				++first;
				++this_first;
			}
		}

		template<typename Input_iterator>
		inline void _p_range_assign_longer_or_equal(Input_iterator first, Input_iterator last)
		{
			_p_range_assign_equal(first, last);
			_p_range_insert(first, last);
		}

		template<typename Input_iterator>
		void _p_range_assign_shorter(Input_iterator first, Input_iterator last, size_type new_len)
		{
			iterator this_first = begin();
			std::advance(this_first, new_len);

			base::truncate_tail(this_first.nodeptr);

			_p_range_assign_equal(first, last);
		}

		// allocators are equal or curr alloc can free stolen nodes or alloc is moved.
		void _p_move_assign(smap&& rval, std::true_type pocma_or_equal)
		{
			clear();
			base::move_head(std::move(rval));
			utils::move_alloc_if_pocma(base::m_node_allocator, rval.m_node_allocator);
		}

		void _p_move_assign(smap&& rval, std::false_type pocma_or_equal)
		{
			if(base::m_node_allocator == rval.m_node_allocator)
			{
				_p_move_assign(std::move(rval), std::true_type());
			}
			else
			{
				auto mbegin = utils::make_move_iterator_if_noexcept(rval.begin());
				auto mend = utils::make_move_iterator_if_noexcept(rval.end());

				_p_range_assign_dispatch(mbegin, mend, rval.size());
			}

		}

	};

}

namespace std
{
	template<typename Key, typename Tp, typename Comp, typename Alloc>
	inline void swap(isa::smap<Key, Tp, Comp, Alloc>& a, isa::smap<Key, Tp, Comp, Alloc>& b) NOEXCEPT_IF(noexcept(a.swap(b)))
	{
		a.swap(b);
	}
}

#endif // _SKIPLIST_H








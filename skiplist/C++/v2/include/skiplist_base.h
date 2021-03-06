//
// Created by Isa on 3/29/21.
//

#ifndef _SKIPLIST_BASE_H
#define _SKIPLIST_BASE_H
#define USE_C_RAND

#include <random>
#include "skiplist_node.h"

namespace isa
{

	template<typename Key, typename Tp, typename Alloc, typename Comparator>
	class smap_base
	{
		enum
		{
			MAX_ADDITIONAL_LEVELS = detail::skiplist_node_base::MAX_ADDITIONAL_LEVELS
		};
	protected:
		using node_base = detail::skiplist_node_base;
		using node_header = detail::skiplist_impl<Key, Tp>;
		using node = detail::skiplist_node<Key, Tp>;

		using node_alloc_type = typename std::allocator_traits<Alloc>::template rebind_alloc<node>;
		using node_alloc_traits = std::allocator_traits<node_alloc_type>;
		using node_pointer = typename node_alloc_traits::pointer;
		using node_const_pointer = typename node_alloc_traits::const_pointer;

		template<typename K, typename V>
		using pair_t = std::pair<K, V>;
		using insert_return_t = typename node_header::insert_return_t;

		using pair_type = pair_t<Key const, Tp>;
		using mutable_key_pair = pair_t<Key, Tp>;

		struct pair_comparator_type
		{
			Comparator m_key_comparator;

			pair_comparator_type() = default;

			explicit pair_comparator_type(Comparator const& key_comparator)
				: m_key_comparator(key_comparator)
			{
			}

			inline bool operator()(pair_type const& a, pair_type const& b)
			{
				return m_key_comparator(a.first, b.first);
			}

		};

		node_alloc_type m_node_allocator;
		pair_comparator_type m_pair_comparator;
		node_header m_head;

		// COMPARE HELPERS
		inline static Key const& _s_get_key(node_base const* node)
		{
			return *static_cast<node_const_pointer> (node)->keyptr();
		}

		int less_count = 0;
		inline bool less(Key const& a, Key const& b) const
		{
			++const_cast<smap_base*> (this)->less_count;
			return m_pair_comparator.m_key_comparator(a, b);
		}

		inline bool greater(Key const& a, Key const& b) const
		{
			return m_pair_comparator.m_key_comparator(b, a);
		}

		inline bool equals(Key const& a, Key const& b) const
		{
//			return !less(a, b) && !greater(a, b);
			return std::equal_to<Key>()(a, b);
		}

		inline bool not_equals(Key const& a, Key const& b) const
		{
			return std::not_equal_to<Key>()(a, b);
		}

		// MEMBER GETTERS
		inline pair_comparator_type& get_pair_comparator()
		{
			return m_pair_comparator;
		}

		inline pair_comparator_type const& get_pair_comparator() const
		{
			return m_pair_comparator;
		}

		inline Comparator& get_key_comparator()
		{
			return m_pair_comparator.m_key_comparator;
		}

		inline Comparator const& get_key_comparator() const
		{
			return m_pair_comparator.m_key_comparator;
		}

		inline node_alloc_type& get_node_allocator()
		{
			return m_node_allocator;
		}

		inline node_alloc_type const& get_node_allocator() const
		{
			return m_node_allocator;
		}

		inline constexpr size_t length() const
		{
			return m_head.m_length;
		}

		// NODE OPERATIONS
		int create_node_count = 0;
		template<typename... Args>
		node_pointer create_node(Args&& ... args)
		{
			++const_cast<smap_base*> (this)->create_node_count;
			node_pointer ptr = node_alloc_traits::allocate(m_node_allocator, 1);
			node_alloc_traits::construct(m_node_allocator, ptr->mutable_dataptr(), std::forward<Args>(args)...);
			return ptr;
		}

		inline void delete_node(node_base* ptr)
		{
			delete_node(static_cast<node_pointer> (ptr));
		}

		void delete_node(node_pointer ptr)
		{
			node_alloc_traits::destroy(m_node_allocator, ptr->mutable_dataptr());
			node_alloc_traits::deallocate(m_node_allocator, ptr, 1);
		}

		// INSERT OPETATIONS
		template<typename... Args>
		node_pointer do_append(Args&&... args)
		{
			node_pointer new_node = create_node(std::forward<Args>(args)...);
			size_t node_height = random_level();
#ifdef SKIPLIST_DEBUG_INFO
			new_node->height = node_height;
#endif
			m_head.append_node(new_node, node_height);

			return new_node;
		}

		template<typename... Args>
		insert_return_t insert_node(Key const& key, Args&&... args)
		{
			node_base* update[1 + MAX_ADDITIONAL_LEVELS];

			insert_return_t res = m_head.find_insert_position(key, get_key_comparator(), update);

			if(res.second == true /*|| not_equals(_s_get_key(pos), key)*/)
			{
				node_pointer new_node = create_node(std::forward<Args>(args)...);
				size_t node_height = random_level();
#ifdef SKIPLIST_DEBUG_INFO
				new_node->height = node_height;
#endif
				m_head.insert_node(new_node, node_height, update);

				return insert_return_t(new_node, true);
			}

			return res;
		}

		template <class T>
		struct unconstref {
			using type = typename std::remove_const<typename std::remove_reference<T>::type>::type;
		};


		template
			<
				typename Pair,
//				typename = typename std::enable_if<!std::is_same<typename unconstref<Pair>::type, pair_type>::value && std::is_constructible<pair_type, Pair>::value>::type
				typename = typename std::enable_if<std::is_constructible<pair_type, Pair>::value>::type
			>
		insert_return_t append_or_insert(Pair&& data)
		{
			Key const& last = _s_get_key(m_head.m_tail[0]);
			Key const& key = data.first;

			if(m_head.m_tail[0] == m_head.npos() || less(last, key))
			{
				node_pointer new_node = do_append(std::forward<Pair> (data));
				return insert_return_t(new_node, true);
			}

			if(greater(last, key))
			{
				return insert_node(key, std::forward<Pair> (data));
			}

			return insert_return_t(static_cast<node_pointer> (m_head.m_tail[0]), false); // key already exists and it is at last position
		}

		template<typename... Args>
		insert_return_t append_or_insert_from_args(Args&&... args)
		{
			// TODO: remove extra copy and make perfect forwarding
			pair_type tmp_copy(std::forward<Args>(args)...);
			return append_or_insert(std::move(tmp_copy));
		}

		// SEARCH OPERATIONS
		template<typename K>
		Tp& find_or_insert(K&& key)
		{
			insert_return_t res = append_or_insert(std::make_pair(std::forward<K>(key), Tp()));
			return res.first->dataptr()->second;
		}

		node_pointer find_node(Key const& key)
		{
			return m_head.find_node(key, get_key_comparator());
		}

		node_const_pointer find_node(Key const& key) const
		{
			return m_head.find_node(key, get_key_comparator());
		}

		// this is fair although it's faster to just return 1 or 0
		size_t count_key(Key const& key) const
		{
			node_base* pos = m_head.find_node(key, get_key_comparator());
			size_t count = 0;

			while(pos != m_head.npos() && equals(_s_get_key(pos), key))
			{
				pos = pos->m_next[0];
				++count;
			}

			return count;
		}

		// REMOVE OPERAITONS
		size_t remove_key(Key const& key)
		{
			node_base* update[1 + MAX_ADDITIONAL_LEVELS];
			node_base* pos = m_head.find_remove_position(key, get_key_comparator(), update);

			size_t count = 0;
			if(pos != m_head.npos() && equals(_s_get_key(pos), key))
			{
				node_base* next = pos->m_next[0];
				m_head.remove_node(pos, update);
				delete_node(pos);
				pos = next;
				++count;
			}

			return count;
		}

		node_base* remove_node(node_base const* node)
		{
			node_base* update[1 + MAX_ADDITIONAL_LEVELS];
			node_base* pos = m_head.find_remove_position(_s_get_key(node), get_key_comparator(), update);

			if(equals(_s_get_key(node), _s_get_key(pos)))
			{
				node_base* next = pos->m_next[0];
				m_head.remove_node(pos, update);
				delete_node(pos);

				return next;
			}
			return const_cast<node_base*> (node);
		}

		node_base* truncate_tail(node_base const* begin)
		{
			node_base* update[1 + MAX_ADDITIONAL_LEVELS];
			node_base* first = m_head.find_remove_position(_s_get_key(begin), get_key_comparator(), update);
			node_base* last = std::addressof(m_head);

			if(equals(_s_get_key(begin), _s_get_key(first)))
			{
				m_head.remove_range(first, last, update, m_head.m_tail);

				while(first != last)
				{
					node_base* next = first->m_next[0];
					delete_node(first);
					first = next;
					--m_head.m_length;
				}
				return first;
			}
			return const_cast<node_base*> (begin);
		}

		// begin and end must never be equal to head, it's caller's responsibility to check it
		node_base* remove_range(node_base const* begin, node_base const* end)
		{
			node_base* update1[1 + MAX_ADDITIONAL_LEVELS];
			node_base* update2[1 + MAX_ADDITIONAL_LEVELS];

			node_base* first = m_head.find_remove_position(_s_get_key(begin), get_key_comparator(), update1);
			node_base* last = m_head.find_remove_position(_s_get_key(end), get_key_comparator(), update2);

			if((equals(_s_get_key(begin), _s_get_key(first)) && equals(_s_get_key(begin), _s_get_key(first))))
			{
				m_head.remove_range(first, last, update1, update2);
				while(first != last)
				{
					auto next = first->m_next[0];
					delete_node(static_cast<node*> (first));
					first = next;
					--m_head.m_length;
				}
				return first;
			}
			return const_cast<node_base*> (begin);
		}

		// ELEMENT ASSIGN
		template<typename... Args>
		inline void assign_pair(node_base* node, Args&&... args) const
		{
			static_cast<node_pointer> (node)->mutable_dataptr()->operator=(std::forward<Args>(args)...);
		}

		// HEADER OPERATIONS
		void move_head(smap_base&& rval)
		{
			m_head = std::move(rval.m_head);
		}

		void swap_head(smap_base& other)
		{
			m_head.swap(other.m_head);
		}

		void init_head() noexcept
		{
			m_head.init_full();
		}

		inline bool is_empty() const
		{
			return m_head.m_next[0] == m_head.npos();
		}

		void clear_nodes() noexcept
		{
			node_pointer curr = static_cast<node_pointer> (m_head.m_next[0]);
			while(curr != m_head.npos())
			{
				node_pointer nod = curr;
				curr = static_cast<node_pointer> (curr->m_next[0]);

				delete_node(nod);
			}
		}


	public:
		smap_base() = default;

		explicit smap_base(Comparator const& key_comp)
			: m_pair_comparator(key_comp)
		  	, m_head()
		{
		}

		explicit smap_base(Alloc const& alloc)
			: m_node_allocator(alloc)
		  	, m_head()
		{
		}

		smap_base(Comparator const& key_comp, Alloc const& alloc)
			: m_node_allocator(alloc)
			, m_pair_comparator(key_comp)
			, m_head()
		{
		}

		smap_base(pair_comparator_type const& pair_comp, node_alloc_type&& alloc)
			: m_node_allocator(std::move(alloc))
		  	, m_pair_comparator(pair_comp)
		  	, m_head()
		{
		}

		smap_base(pair_comparator_type const& pair_comp, Alloc const& alloc)
			: m_node_allocator(alloc)
		  	, m_pair_comparator(pair_comp)
		  	, m_head()
		{
		}

		smap_base(smap_base&& rval) = default;

		smap_base(smap_base&& rval, Alloc const& alloc)
			: m_node_allocator(alloc)
		  	, m_pair_comparator(std::move(rval.m_pair_comparator))
		  	, m_head(std::move(rval.m_head))
		{
		}

		~smap_base() noexcept
		{
			clear_nodes();
		}

		// RANDOMIZATION
		size_t random_level()
		{
#ifndef USE_C_RAND
			float p = 0.5f;
			std::random_device seed;
			std::default_random_engine e(seed());
			std::geometric_distribution<size_type> d(p);

			return d(e);
#else
			size_t res = 0;
			while(res < MAX_ADDITIONAL_LEVELS && rand() < RAND_MAX / 2)
			{
				res++;
			}

			return res;
#endif
		}
	};

}


#endif //_SKIPLIST_BASE_H

//
// Created by Isa on 3/15/21.
//

#ifndef CONTAINER_UTILS_H
#define CONTAINER_UTILS_H


#define NOEXCEPT_IF(...) noexcept(__VA_ARGS__)

#include <iterator>
#include <memory>

namespace isa
{
	namespace utils
	{

		template<typename Input_iterator>
		using require_input_iter =
			typename std::enable_if<
				std::is_convertible<
					typename std::iterator_traits<Input_iterator>::iterator_category,
					std::input_iterator_tag
				>::value
			>::type;

		template<typename Tp, typename From>
		using require_constructible =
			typename std::enable_if<
				std::is_constructible<Tp, From>::value
			>::type;

		// if iterator is noexcept_move_constructible or only_move_type then make move_iterator else iterator
		template<
			typename Iterator,
			typename Return_type =
				typename std::conditional<
					!std::is_nothrow_move_constructible<Iterator>::value && std::is_copy_constructible<Iterator>::value,
					Iterator,
					std::move_iterator<Iterator>
				>::type
			>
		inline constexpr Return_type make_move_iterator_if_noexcept(Iterator i)
		{
			return Return_type(i);
		}

		// Overload for pointers that matches std::move_if_noexcept more closely,
		// returning a constant iterator when we don't want to move.
		template<
			typename Iterator_maybe_ptr,
			typename Return_type =
				typename std::conditional<
					!std::is_nothrow_move_constructible<Iterator_maybe_ptr>::value && std::is_copy_constructible<Iterator_maybe_ptr>::value,
					const Iterator_maybe_ptr*,
					std::move_iterator<Iterator_maybe_ptr*>
				>::type
			>
		inline constexpr Return_type make_move_iterator_if_noexcept(Iterator_maybe_ptr* i)
		{
			return Return_type(i);
		}

		// Optimize for stateless allocators
		template<typename Alloc, bool = std::is_empty<Alloc>::value>
		struct alloc_not_equal
		{
			static bool _so_do_it(const Alloc&, const Alloc&)
			{
				return false;
			}
		};

		// Stateful allocators
		template<typename Alloc>
		struct alloc_not_equal<Alloc, false>
		{
			static bool _so_do_it(const Alloc& one, const Alloc& two)
			{
				return one != two;
			}
		};


		// move assign allocator if possible
		template<typename Alloc>
		inline void do_move_alloc_if_pocma(Alloc& a, Alloc& b, std::true_type)
		{
			// pocma true -> move assign allocators
			a = std::move(b);
		}

		template<typename Alloc>
		inline void do_move_alloc_if_pocma(Alloc& a, Alloc& b, std::false_type)
		{
			// pocma false -> do nothing
			return;
		}

		template<typename Alloc>
		inline void move_alloc_if_pocma(Alloc& a, Alloc& b)
		{
			using pocma = typename std::allocator_traits<Alloc>::propagate_on_container_move_assignment;
			do_move_alloc_if_pocma(a, b, pocma());
		}


		// copy assign allocator if possible
		template<typename Alloc>
		inline void do_copy_if_pocca(Alloc& a, const Alloc& b, std::true_type)
		{
			a = b;
		}

		template<typename Alloc>
		inline void do_copy_if_pocca(Alloc& a, const Alloc& b, std::false_type)
		{
			return;
		}

		template<typename Alloc>
		inline void copy_alloc_if_pocca(Alloc& a, const Alloc& b)
		{
			using pocca = typename std::allocator_traits<Alloc>::propagate_on_container_copy_assignment;

			do_copy_if_pocca(a, b, pocca());
		}


		template<typename Alloc>
		inline void swap_alloc_if_pocs(Alloc& a, Alloc& b, std::true_type)
		{
			std::swap(a, b);
		}

		template<typename Alloc>
		inline void swap_alloc_if_pocs(Alloc& a, Alloc& b, std::false_type)
		{
			return;
		}

		template<typename Alloc>
		inline void swap_alloc_if_pocs(Alloc& a, Alloc& b)
		{
			typedef std::allocator_traits<Alloc> traits;
			typedef typename traits::propagate_on_container_swap pocs;
			swap_alloc_if_pocs(a, b, pocs());
		}

		template<bool B>
		using bool_constant = std::integral_constant<bool, B>;

	}
}

#endif //CONTAINER_UTILS_H

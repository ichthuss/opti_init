/* Efficient and comprehensive MCU peripheral configuration library
 * Copyright (c) 2019 Anton Konkevych.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef OPTI_INIT_INTERNALS_H
#define OPTI_INIT_INTERNALS_H

namespace opti_init
{

	// TODO: add checks for compiler other than gcc
	#if defined(__AVR__)
		typedef uint8_t peripheral_register_t;
		typedef uint16_t pointer_int_t;
	#elif defined(__arm__)
		typedef uint32_t peripheral_register_t;
		typedef uint32_t pointer_int_t;
	#else
		typedef uint8_fast_t peripheral_register_t;
		typedef intptr_t pointer_int_t;
	#endif

	template <pointer_int_t ptr, peripheral_register_t mask, peripheral_register_t val>
	struct modifier {
		modifier(){this->perform();}

		static void perform() {
			uint8_t value = (*reinterpret_cast<peripheral_register_t*>(ptr) & ~mask) | val;
			*reinterpret_cast<peripheral_register_t*>(ptr) = value;
		}
	};

	template<typename ...T>
	struct list {};

	namespace detail {

		template<typename H, typename T>
		struct prepend {};

		template<>
		template<typename H, typename ...T>
		struct prepend<H,list<T...>> {
			typedef list<H,T...> type;
		};


		template<bool do_prepend,typename H, typename T>
		struct conditional_prepend {};

		template<>
		template<typename H, typename T>
		struct conditional_prepend<true,H,T> {
			typedef typename prepend<H, T>::type type;
		};

		template<>
		template<typename H, typename T>
		struct conditional_prepend<false, H, T> {
			typedef T type;
		};

		template<typename L, typename R>
		struct concat {};

		template<>
		template<typename ...T>
		struct concat<list<>,list<T...>> {
			typedef list<T...> type;
		};

		template<>
		template<typename H, typename ...T>
		struct concat<H,list<T...>> {
			typedef typename concat<typename H::head, list<typename H::last, T...>>::type type;
		};

		template<typename ...T>
		struct flatten {};

		template<>
		struct flatten<> {
			typedef list<> type;
		};

		template<>
		template<typename ...T>
		struct flatten<list<T...>> {
			typedef typename flatten<T...>::type type;
		};

		template<>
		template<typename T>
		struct flatten<T> {
			typedef list<T> type;
		};

		template<>
		template<typename H, typename ...T>
		struct flatten<H, T...> {
			typedef typename concat<typename flatten<H>::type,typename flatten<T...>::type>::type type;
		};

		template<typename predicate, typename ...T>
		struct filter {};

		template<>
		template<typename predicate>
		struct filter<predicate> {
			typedef list<> type;
		};

		template<>
		template<typename predicate, typename H, typename ...T>
		struct filter<predicate, H, T...> {
			typedef typename conditional_prepend<predicate::template apply<H>::value,H,typename filter<predicate,T...>::type>::type type;
		};


		template<typename reducer, typename H, typename ...T>
		struct fold {};

		template<>
		template<typename reducer, typename A>
		struct fold<reducer, A> {
			typedef A type;
		};

		template<>
		template<typename reducer, typename A, typename H, typename ...T>
		struct fold<reducer, A, H, T...> {
			typedef typename fold<reducer,typename reducer::template apply<A,H>::type,T...>::type type;
		};

		struct modifier_combine {
			template <typename A, typename B>
			struct apply {};
		};

		template<>
		template<pointer_int_t ptr1, pointer_int_t ptr2, peripheral_register_t mask1, peripheral_register_t mask2, peripheral_register_t val1, peripheral_register_t val2>
		struct modifier_combine::apply<modifier<ptr1, mask1, val1>, modifier<ptr2, mask2, val2>>
		{
			typedef modifier<ptr1, (ptr1==ptr2)?(mask1|mask2):mask1, (ptr1==ptr2)?((val1 & ~(mask2))|val2):val1> type;
		};

		template<typename A>
		struct not_equal {
			template <typename B>
			struct apply {};
		};

		template<>
		template<pointer_int_t ptr1, peripheral_register_t mask1, peripheral_register_t val1>
		struct not_equal<modifier<ptr1, mask1, val1 >>
		{
			template <typename A, typename D = void>
			struct apply_internal {};

			template <pointer_int_t ptr2, peripheral_register_t mask2, peripheral_register_t val2, typename D>
			struct apply_internal<modifier<ptr2, mask2, val2 >, D> {
					static const bool value = ptr1 != ptr2;
			};

			template <typename A>
			using apply = apply_internal<A>;
		};


		template<typename ...T>
		struct distinct_folded {};

		template <typename T>
		struct fold_distinct {};

		template<>
		template<typename H, typename ...T>
		struct distinct_folded<H, T...> {
			typedef typename flatten<
						typename fold<modifier_combine,H,T...>::type ,
						typename fold_distinct<typename filter<not_equal<H>,T...>::type>::type
				 >::type type;
		};

		template<>
		struct distinct_folded<> {
			typedef list<> type;
		};

		template <>
		template <typename ...T>
		struct fold_distinct<list<T...>> {
			typedef typename distinct_folded<T...>::type type;
		};


	} // namespace detail

	template<>
	template<typename H>
	struct list<H> {
		list(){this->perform();}

		static void perform_raw() {
			H::perform();
		}

		static void perform(){
			detail::fold_distinct<typename detail::flatten<H>::type>::type::perform_raw();
		}

		typedef list<> head;
		typedef H last;
	};

	template<>
	template<typename H, typename ...T>
	struct list<H, T...> {
		list(){this->perform();}

		static void perform_raw() {
			H::perform();
			list<T...>::perform_raw();
		}

		static void perform(){
			detail::fold_distinct<typename detail::flatten<H,T...>::type>::type::perform_raw();
		}

		typedef typename detail::prepend<H,typename list<T...>::head>::type head;
		typedef typename list<T...>::last last;
	};

	/*
	By now, initializer and settings are synonyms. Later, presumed register
	value semantics is going to be implemented with initializer class, so
	that it may be efficiently used for first-time initialization, and
	settings will be generic current-state-independent way of writing
	registers
	*/
	template <typename ...T>
	using initializer = list<T...>;

	template <typename ...T>
	using settings = list<T...>;
}

#endif

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

#ifndef OPTI_INIT_HARDWARE_H
#define OPTI_INIT_HARDWARE_H

#include "internals.h"

namespace opti_init
{
	namespace hardware
	{
		template <pointer_int_t ptr, int index>
		struct peripheral_register_bit {

			static constexpr int bit_index = index;
			static constexpr int bit_mask = (1 << index);

			static constexpr peripheral_register_t * pointer() {
				return reinterpret_cast<peripheral_register_t *>(ptr);
			}

			using type = peripheral_register_bit<ptr, index>;

			typedef modifier<ptr, bit_mask ,0> low;
			typedef modifier<ptr, bit_mask, bit_mask> high;

			static bool get_f() { return !!(*pointer() & bit_mask); }
			static void set_f(bool value) { *pointer() = value? *pointer() | bit_mask : *pointer() & ~bit_mask; }

			template <int value>
			using set = modifier<ptr, bit_mask, value?bit_mask:0>;
		};

		template <pointer_int_t ptr>
		struct peripheral_register {
			static constexpr peripheral_register_t * pointer() {
				return reinterpret_cast<peripheral_register_t *>(ptr);
			}

			template <int index>
			using bit = peripheral_register_bit<ptr, index>;

			template <int index, bool value>
			using set = typename peripheral_register_bit<ptr, index>::template set<value>;
		};

		#ifdef OPTI_INIT_TESTS
		namespace test
		{

			using namespace opti_init::test::utils;

			//try to instantiate some types
			using reg = peripheral_register< 1 >;
			using reg_bit = peripheral_register_bit< 1, 0 >;

			static_assert(reg::pointer() == (peripheral_register_t *)(1), "register pointer invalid");
			static_assert(reg_bit::pointer() == (peripheral_register_t *)(1), "register bit pointer invalid");
			static_assert(reg_bit::bit_index == 0, "register bit index invalid");
			static_assert(reg_bit::bit_mask == (1 << 0), "register bit mask invalid");

			using reg_bit2 = reg::bit<0>;
			using reg_bit3 = reg::bit<0>::type;

			using reg_mod_low = modifier<1, (1 << 0), 0>;
			using reg_mod_high = modifier<1, (1 << 0), (1 << 0)>;

			static_assert(is_same<reg_bit, reg_bit2>::value, "peripheral register bit resolution is not consistent");
			static_assert(is_same<reg_bit, reg_bit3>::value, "peripheral register bit resolution is not consistent");

			static_assert(is_same<reg_bit::set<0>, reg_mod_low>::value, "peripheral register bit setting type invalid");
			static_assert(is_same<reg_bit::set<1>, reg_mod_high>::value, "peripheral register bit setting type invalid");
		}
		#endif // OPTI_INIT_TESTS

	} // namespace hardware

} // namespace opti_init

#ifdef __AVR__
	#include "avr.h"
#elif defined(STM32F0) || defined(STM32F1)
	//TODO
#endif


#endif // OPTI_INIT_HARDWARE_H

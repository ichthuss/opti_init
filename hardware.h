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
			typedef modifier<ptr, 1<<index ,0> low;
			typedef modifier<ptr, 1<<index, 1 << index> high;

			template <int value>
			struct set {
				typedef modifier<ptr, 1<<index, value?(1<<index):0> type;
			};
		};

		template <pointer_int_t ptr>
		struct peripheral_register {
			template <int index>
			struct bit {
				typedef peripheral_register_bit<ptr, index> type;

				template <int value>
				struct set {
					typedef modifier<ptr, 1<<index, value?(1<<index):0> type;
				};
			};
		};

	} // namespace hardware

} // namespace opti_init

#ifdef __AVR__
	#include "avr.h"
#elif defined(STM32F0) || defined(STM32F1)
	//TODO
#endif


#endif // OPTI_INIT_HARDWARE_H

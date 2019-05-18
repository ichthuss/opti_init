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

#ifndef OPTI_INIT_AVR_H
#define OPTI_INIT_AVR_H

namespace opti_init
{
	namespace hardware
	{
		template<pointer_int_t ptr_direction, pointer_int_t ptr_value, peripheral_register_t bit>
		struct gpio {
			using input_any  = modifier< ptr_direction, (1<<bit), 0 >;
			using output_any = modifier< ptr_direction, (1<<bit), (1<<bit) >;
			using low  = modifier< ptr_value, (1<<bit), 0 >;
			using high = modifier< ptr_value, (1<<bit), (1<<bit) >;

			using output_low = list< low, output_any >;
			using output_high = list< high, output_any >;

			using input_floating = list< low, input_any>;
			using input_pullup =  list< high, input_any>;

			using input = input_floating;
			using output = output_any;
		};

		#define __AVR_PORT(X) \
				template <int index> \
				struct port##X : gpio<(pointer_int_t)(&DDR##X), (pointer_int_t)(&PORT##X), index> {};

		#ifdef PORTA
			__AVR_PORT(A);
		#endif

		#ifdef PORTB
			__AVR_PORT(B);
		#endif

		#ifdef PORTC
			__AVR_PORT(C);
		#endif

		#ifdef PORTD
			__AVR_PORT(D);
		#endif

		#ifdef PORTE
			__AVR_PORT(E);
		#endif

		#ifdef PORTF
			__AVR_PORT(F);
		#endif

		#ifdef PORTG
			__AVR_PORT(G);
		#endif

		#ifdef PORTH
			__AVR_PORT(H);
		#endif

		#ifdef PORTI
			__AVR_PORT(I);
		#endif

		#ifdef PORTJ
			__AVR_PORT(J);
		#endif

		#ifdef PORTK
			__AVR_PORT(K);
		#endif

		#ifdef PORTL
			__AVR_PORT(L);
		#endif

		#undef __AVR_PORT

#ifdef ARDUINO
		/* OK OK, it may look stupid to fight for bytes while using Arduino platform with its
		 * ugly digitalRead/digitalWrite etc, but one may write efficient code even for
		 * Arduino. There is a pretty good digital[Read|Write]Fast library code which returns
		 * you to efficiency land, and empty Arduino program is not so far from empty bare
		 * avr-gcc program. So, if you're picky enough about libraries  to use (both standard
		 * or 3rd party), you may have very effective code in spite of using Arduino codebase.
		 */

		// These defines are taken from digitalWriteFast.h library - https://codebender.cc/library/digitalWriteFast#bonus%2FdigitalWrite%2Fdigital_write_macros.h
		#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
			// Arduino Mega Pins
			#define digitalPinToPortReg(P) \
			(((P) >= 22 && (P) <= 29) ? &PORTA : \
			((((P) >= 10 && (P) <= 13) || ((P) >= 50 && (P) <= 53)) ? &PORTB : \
			(((P) >= 30 && (P) <= 37) ? &PORTC : \
			((((P) >= 18 && (P) <= 21) || (P) == 38) ? &PORTD : \
			((((P) >= 0 && (P) <= 3) || (P) == 5) ? &PORTE : \
			(((P) >= 54 && (P) <= 61) ? &PORTF : \
			((((P) >= 39 && (P) <= 41) || (P) == 4) ? &PORTG : \
			((((P) >= 6 && (P) <= 9) || (P) == 16 || (P) == 17) ? &PORTH : \
			(((P) == 14 || (P) == 15) ? &PORTJ : \
			(((P) >= 62 && (P) <= 69) ? &PORTK : &PORTL))))))))))

			#define digitalPinToDDRReg(P) \
			(((P) >= 22 && (P) <= 29) ? &DDRA : \
			((((P) >= 10 && (P) <= 13) || ((P) >= 50 && (P) <= 53)) ? &DDRB : \
			(((P) >= 30 && (P) <= 37) ? &DDRC : \
			((((P) >= 18 && (P) <= 21) || (P) == 38) ? &DDRD : \
			((((P) >= 0 && (P) <= 3) || (P) == 5) ? &DDRE : \
			(((P) >= 54 && (P) <= 61) ? &DDRF : \
			((((P) >= 39 && (P) <= 41) || (P) == 4) ? &DDRG : \
			((((P) >= 6 && (P) <= 9) || (P) == 16 || (P) == 17) ? &DDRH : \
			(((P) == 14 || (P) == 15) ? &DDRJ : \
			(((P) >= 62 && (P) <= 69) ? &DDRK : &DDRL))))))))))

			#define digitalPinToPINReg(P) \
			(((P) >= 22 && (P) <= 29) ? &PINA : \
			((((P) >= 10 && (P) <= 13) || ((P) >= 50 && (P) <= 53)) ? &PINB : \
			(((P) >= 30 && (P) <= 37) ? &PINC : \
			((((P) >= 18 && (P) <= 21) || (P) == 38) ? &PIND : \
			((((P) >= 0 && (P) <= 3) || (P) == 5) ? &PINE : \
			(((P) >= 54 && (P) <= 61) ? &PINF : \
			((((P) >= 39 && (P) <= 41) || (P) == 4) ? &PING : \
			((((P) >= 6 && (P) <= 9) || (P) == 16 || (P) == 17) ? &PINH : \
			(((P) == 14 || (P) == 15) ? &PINJ : \
			(((P) >= 62 && (P) <= 69) ? &PINK : &PINL))))))))))

			#define __digitalPinToBit(P) \
			(((P) >=  7 && (P) <=  9) ? (P) - 3 : \
			(((P) >= 10 && (P) <= 13) ? (P) - 6 : \
			(((P) >= 22 && (P) <= 29) ? (P) - 22 : \
			(((P) >= 30 && (P) <= 37) ? 37 - (P) : \
			(((P) >= 39 && (P) <= 41) ? 41 - (P) : \
			(((P) >= 42 && (P) <= 49) ? 49 - (P) : \
			(((P) >= 50 && (P) <= 53) ? 53 - (P) : \
			(((P) >= 54 && (P) <= 61) ? (P) - 54 : \
			(((P) >= 62 && (P) <= 69) ? (P) - 62 : \
			(((P) == 0 || (P) == 15 || (P) == 17 || (P) == 21) ? 0 : \
			(((P) == 1 || (P) == 14 || (P) == 16 || (P) == 20) ? 1 : \
			(((P) == 19) ? 2 : \
			(((P) == 5 || (P) == 6 || (P) == 18) ? 3 : \
			(((P) == 2) ? 4 : \
			(((P) == 3 || (P) == 4) ? 5 : 7)))))))))))))))
		#else

			// Standard Arduino Pins
			#define digitalPinToPortReg(P) \
			(((P) >= 0 && (P) <= 7) ? &PORTD : (((P) >= 8 && (P) <= 13) ? &PORTB : &PORTC))
			#define digitalPinToDDRReg(P) \
			(((P) >= 0 && (P) <= 7) ? &DDRD : (((P) >= 8 && (P) <= 13) ? &DDRB : &DDRC))
			#define digitalPinToPINReg(P) \
			(((P) >= 0 && (P) <= 7) ? &PIND : (((P) >= 8 && (P) <= 13) ? &PINB : &PINC))
			#define __digitalPinToBit(P) \
			(((P) >= 0 && (P) <= 7) ? (P) : (((P) >= 8 && (P) <= 13) ? (P) - 8 : (P) - 14))
		#endif

		template <int number>
		using digitalPin = typename gpio<(pointer_int_t)( digitalPinToDDRReg(number)),(pointer_int_t)( digitalPinToPortReg(number)),__digitalPinToBit(number)>;

#endif // ARDUINO

		#ifdef OPTI_INIT_TESTS
		namespace test
		{
			// instantiate some classes
			using my_gpio = gpio<1, 2, 0>;
		}
		#endif // OPTI_INIT_TEST

	}
}
#endif

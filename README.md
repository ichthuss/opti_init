# OptiInit - efficient and comprehensive MCU peripheral configuration library
When doing peripheral configuration on MCU, you often need to do several
settings that are logically separated but belong to one register. In C, you
may do it with either efficient but messy code:
```
	DDRA = (1 << 6) | (1 << 4) | (1 << 3); // set pins 3,4,6 to output, 0,1,2,5,7 to input
	PORTA = (1 << 5) | (1 << 3)); // set pin 3 high, enable pullup on pin 5, all other are low / floating
```
Or you may use clear and comprehensive but uneffective code:
```
	setPin(PORTA, 0, INPUT_FLOATING);
	setPin(PORTA, 1, INPUT_FLOATING);
	setPin(PORTA, 2, INPUT_FLOATING);
	setPin(PORTA, 3, OUTPUT_HIGH);
	setPin(PORTA, 4, OUTPUT_LOW);
	setPin(PORTA, 5, INPUT_PULLUP);
	setPin(PORTA, 6, OUTPUT_LOW);
	setPin(PORTA, 7, INPUT_FLOATING);
```
With C++, you can have both comprehensive and efficient code for register
settings. This header-only library uses C++ template metaprogramming dark
magic to do both. E.g., code above becomes as easy as
```
using namespace opti_init::hardware;
void init_pins() {
	initializer<
		portA<0>::input_floating,
		portA<1>::input_floating,
		portA<2>::input_floating,
		portA<3>::output_high,
		portA<4>::output_low,
		portA<5>::input_pullup,
		portA<6>::output_low,
		portA<7>::input_floating
	>::perform();
}
```
This code is as clear as second sample above and as effective as first sample.

You may intermix different register access:
```
initializer<
	portA<0>::output,
	portB<1>::input_pullup,
	portA<3>::output_low
	>::perform();
```
This still will be performed in an efficient way: all register accesses will be
gathered, and only one read-modify-write sequence is performed for any register
accessed.

You may use any configuration option as a standalone initializer:
```
	portC<5>::output_low::perform();
```

You may combine different options and make your own initializers:
```
using namespace opti_init;
using namespace opti_init::hardware;
template <int MISO, int MOSI, int CLK>
struct softwareSPI {
	typedef list< digitalPin<MISO>::input_floating, digitalPin<MOSI>::output_high, digitalPin<CLK>::output_high > init;
};
```
And use it as standalone option or combining with other initialization:
```
softwareSPI<2,3,4>::init::perform();

// or

initializer<
	... some stuff ...
	softwareSPI<2,3,4>::init,
	... some other stuff
	>::perform();
```

For those who noticed - yes, this library may be used with avr-based
Arduinos. While it may look like a stupid idea to fight for bytes with
Arduino IDE, you sometimes may find yourself (like I do) writing a code
for some tiny chip like attin85 in Arduino IDE just because it's how
your client may flash it easily having no experience at all. And I don't
like writing ineficcient code unless there are really good reasons to have
one.

This library isn't limited to GPIO settings. You may set any memory-
mapped register, e.g.:
```
struct timer0 {

	// wrap TCCR0B register
	struct  tccr0b: peripheral_register<(pointer_int_t)(&TCCR0B)>{};

	typedef list<
		tccr0b::bit<CS02>::set<0>::type,
		tccr0b::bit<CS01>::set<1>::type,
		tccr0b::bit<CS00>::set<1>::type
	> clk_divider_64;
};

timer0::clk_divider_64::perform();
```

## Known issues
Registers are modified in a "first mentioned" order. It may cause an issue
if two initializers that require opposite order of register modification
are combined into single initializer. This issue may be addressed, but it
would require not only template metaprogramming black magic, but rather
cruel necromancy to implement it, so I'm unsure if I going to do it any
time soon.

There is no way to assign presumed values to bits. It means that, unless you
set all bits of the register, it will be modified with read-modify-write
sequence instead of single write operation. It's what's needed when you
modify register during program execution, but is suboptimal when used
for initial settings where register value is known in advance.

Code is written in architecture-independent way, but the only peripheral
device that has helpers implemented are AVR attiny/atmega GPIO ports.

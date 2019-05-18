#ifndef OPTI_INIT_TESTS_H
#define OPTI_INIT_TESTS_H

namespace opti_init
{
	namespace test
	{
		namespace utils
		{
			template<class A, class B>
			struct is_same {
				static constexpr bool value = false;
			};

			template<class A>
			struct is_same<A, A> {
				static constexpr bool value = true;
			};
		}
	}
}

#endif

#include <iostream>
#include <limits>

#include "BorshCpp.hpp"

int main()
{
	bool cArray[2] = {true, false};
	auto initList = {"1", "2"};

	std::vector<uint8_t> dynamicArray = {
		34,
		85,
		255
	};

	BorshEncoder encoder;
	encoder
		.Encode(
			/* Integers */
			(uint8_t)0xff,
			(uint16_t)0xffff,
			(uint32_t)0xffffffff,
			
			/* Floats */
			230.121312f,
			230.121312,

			/* Bools */
			true,

			/* Strings */
			"Hola mundo!!!", // suppport ascii
			u8"Hola mundo!!!🤓", // and utf8 only as literals!!!!!

			/* C Array */
			std::pair{cArray, 2},

			/* Initializer List */
			initList,

			/* Vectors */
			dynamicArray
		);

	for (auto c : encoder.GetBuffer())
	{
		printf("%d ", c);
	}
	printf("\n");

}
#include <iostream>
#include <limits>

#include "BorshCpp.hpp"

int main()
{
	auto arr1 = {1, 2};
	auto arr2 = {3.0, 4.0};
	auto arr3 = {"1", "2"};

	BorshEncoder encoder;
	encoder
		.EncodeFixArray(arr1)
		.EncodeFixArray(arr2)
		.EncodeFixArray(arr3)
		.EncodeInteger(true)
		.EncodeInteger(false)
		.EncodeInteger((uint8_t)0xff)
		.EncodeInteger((uint16_t)0xffff)
		.EncodeInteger((uint32_t)0xfffffff)
		.EncodeInteger((uint64_t)0xffffffffffffffff)
		.EncodeInteger((int8_t)0x7d)
		.EncodeInteger((int16_t)0x7fff)
		.EncodeInteger((int32_t)0x7ffffff)
		.EncodeInteger((int64_t)0x7fffffffffffffff)
		.EncodeString("Hola mundo!!!") // suppport ascii
		.EncodeString(u8"Hola mundo!!!🤓"); // and utf8 only as literals!!!!!

	for (auto c : encoder.GetBuffer())
	{
		printf("%d ", c);
	}
}
#include <iostream>
#include <limits>

// #include "BorshCpp.hpp"
#include "BorshCpp2.hpp"

struct Person {
  std::string name;
  uint16_t age;
  std::vector<std::vector<std::string>> aliases;

  template<class T>
  void pack(T &pack) {
    pack(name, age, aliases);
  }
};


int main()
{
	uint32_t arr1[2] = {1, 2};
	float arr2[2] = {3.0, 4.0};
	auto arr3 = {"1", "2"};

	std::vector<uint8_t> dynamicArray = {
		34,
		85,
		255
	};

	auto person = Person{"John", 22, {{"Ripper", "Silverhand"}, {"Ripper", "Silverhand"}}};
	auto data = borshcpp::pack(person);

	// BorshEncoder encoder;
	// encoder
		// .EncodeDynamicArray(dynamicArray)
		// .EncodeFixArray(arr1, 2)
		// .EncodeFixArray(arr2, 2)
		// .EncodeFixArray(arr3)
		// .EncodeInteger(true)
		// .EncodeInteger(false)
		// .EncodeInteger((uint8_t)0xff)
		// .EncodeInteger((uint16_t)0xffff)
		// .EncodeInteger((uint32_t)0xfffffff)
		// .EncodeInteger((uint64_t)0xffffffffffffffff)
		// .EncodeInteger((int8_t)0x7d)
		// .EncodeInteger((int16_t)0x7fff)
		// .EncodeInteger((int32_t)0x7ffffff)
		// .EncodeInteger((int64_t)0x7fffffffffffffff)
		// .EncodeString("Hola mundo!!!") // suppport ascii
		// .EncodeString(u8"Hola mundo!!!🤓"); // and utf8 only as literals!!!!!

	// for (auto c : encoder.GetBuffer())
	// {
	// 	printf("%d ", c);
	// }
	printf("\n");
}
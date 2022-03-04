# Borsh in C++ &emsp; [![C++ 17 Ref badge]][c++ 17] [![License Apache-2.0 badge]][license apache-2.0]

[borsh]: https://borsh.io
[license apache-2.0 badge]: https://img.shields.io/badge/license-Apache2.0-blue.svg
[license apache-2.0]: https://opensource.org/licenses/Apache-2.0
[c++ 17]: https://en.wikipedia.org/wiki/C%2B%2B17
[c++ 17 ref badge]: https://img.shields.io/badge/C%2B%2B-17-blue.svg

**borsh-cpp** is a single header C++ implementation of the [Borsh] binary serialization format.

Borsh stands for _Binary Object Representation Serializer for Hashing_. It is meant to be used in
security-critical projects as it prioritizes [consistency, safety, speed][borsh], and comes with a
strict [specification](https://github.com/near/borsh#specification).

## Example

```cpp
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
```

For now only support serialization

## Types

| Borsh            | C++                        |
| ---------------- | -------------------------- |
| `bool`           | `true or false`            |
| `u8` integer     | `uint8_t`                  |
| `u16` integer    | `uint16_t`                 |
| `u32` integer    | `uint32_t`                 |
| `u64` integer    | `uint64_t`                 |
| `u128` integer   | `Not supported`            |
| `i8` integer     | `int8_t`                   |
| `i16` integer    | `int16_t`                  |
| `i32` integer    | `int32_t`                  |
| `i64` integer    | `int64_t`                  |
| `i128` integer   | `Not supported`            |
| `f32` float      | `float`                    |
| `f64` float      | `double`                   |
| fixed-size array | `std::initializer_list<T>` |
| fixed-size array | `T*, size_t size`          |
| dynamic array    | `std::vector<T>`           |
| UTF-8 string     | `std::string(u8"🤓")`      |
| UTF-8 string     | `u8"🤓"`                   |
| option           | `Not supported`            |
| set              | `Not supported`            |
| map              | `Not supported`            |
| structs          | `Not supported`            |

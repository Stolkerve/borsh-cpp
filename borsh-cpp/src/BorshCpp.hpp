#include <vector>
#include <string>
#include <cassert>

namespace BorshCpp
{
	template <typename T>
	struct is_string
	{
		static const bool value = false;
	};

	template <class T, class Traits, class Alloc>
	struct is_string<std::basic_string<T, Traits, Alloc>>
	{
		static const bool value = true;
	};
}

class BorshEncoder {
public:
	template<typename T>
	constexpr BorshEncoder& EncodeInteger(const T integer)
	{
		static_assert(std::is_integral<T>::value, "Integer value only");

		const size_t typeSize = sizeof T;
		uint8_t offset = 0;

		for (size_t i = 0; i < typeSize; i++)
		{
			m_Buffer.push_back((integer >> offset) & 0xff);
			offset += 8;
		}

		return *this;
	}

	template<typename T>
	constexpr BorshEncoder& EncodeFloatingPoint(const T floatingPoint)
	{
		static_assert(std::is_floating_point<T>::value, "Floating point value only");

		assert(!std::isnan(floatingPoint), "NaN value found");

		// From https://github.com/naphaso/cbor-cpp/blob/master/src/encoder.cpp
		const void* punny = &floatingPoint;

		for (size_t i = 0; i < sizeof T; i++)
		{
			m_Buffer.push_back(*((uint8_t*)punny + i));
		}

		return *this;
	}

	BorshEncoder& EncodeString(const std::string& str)
	{
		// Write the size of the string as an u32 integer
		EncodeInteger(static_cast<uint32_t>(str.size()));

		m_Buffer.insert(m_Buffer.begin() + m_Buffer.size(), str.begin(), str.end());

		return *this;
	}
	

	BorshEncoder& EncodeString(const char* str)
	{
		const uint32_t size = std::strlen(str);
		// Write the size of the string as an u32 integer
		EncodeInteger(size);

		auto bytes = reinterpret_cast<const uint8_t*>(str);

		m_Buffer.insert(m_Buffer.begin() + m_Buffer.size(), bytes, bytes + size);

		return *this;
	}

	template<typename T>
	constexpr BorshEncoder& EncodeFixArray(const std::initializer_list<T>& initList)
	{
		return EncodeFixArray<T>(initList.begin(), initList.size());
	}

	template<typename T>
	constexpr BorshEncoder& EncodeFixArray(const T* array, size_t size)
	{
		if constexpr (std::is_integral<T>::value)
		{
			for (size_t i = 0; i < size; ++i)
			{
				EncodeInteger(*array);
				++array;
			}
		}
		else if constexpr (std::is_floating_point<T>::value)
		{
			for (size_t i = 0; i < size; ++i)
			{
				EncodeFloatingPoint(*array);
				++array;
			}
		}
		else if constexpr  (BorshCpp::is_string<T>::value)
		{
			for (size_t i = 0; i < size; ++i)
			{
				EncodeString(*array);
				++array;
			}
		}
		else if constexpr  (std::is_same<const char*, T>::value)
		{
			for (size_t i = 0; i < size; ++i)
			{
				EncodeString(*array);
				++array;
			}
		}
		else
		{
			assert(false, "The type of the array is not supported");
		}

		return *this;
	}

	template<typename T>
	constexpr BorshEncoder& EncodeDynamicArray(const std::vector<T>& vector)
	{
		EncodeInteger((uint32_t)m_Buffer.size());
		EncodeFixArray(vector.data(), vector.size());

		return *this;
	}

	const std::vector<uint8_t>& GetBuffer() const
	{
		return m_Buffer;
	}

private:
	std::vector<uint8_t> m_Buffer;
};
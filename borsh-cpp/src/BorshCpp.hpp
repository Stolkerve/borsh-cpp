#include <vector>
#include <string>
#include <list>
#include <functional>
#include <unordered_map>
#include <map>
#include <set>
#include <assert.h>
#include <cmath>
#include <cstring>
#include <type_traits>
#include <typeinfo>

namespace BorshCppInternals
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

	template <class T>
	struct is_container
	{
		static const bool value = false;
	};

	template <class T, class Alloc>
	struct is_container<std::vector<T, Alloc>>
	{
		static const bool value = true;
	};

	template <class T, class Alloc>
	struct is_container<std::list<T, Alloc>>
	{
		static const bool value = true;
	};

	template <class T, class Alloc>
	struct is_container<std::map<T, Alloc>>
	{
		static const bool value = true;
	};

	template <class T, class Alloc>
	struct is_container<std::unordered_map<T, Alloc>>
	{
		static const bool value = true;
	};

	template <class T, class Alloc>
	struct is_container<std::set<T, Alloc>>
	{
		static const bool value = true;
	};

	template <class T>
	struct is_stdarray
	{
		static const bool value = false;
	};

	template <class T, std::size_t N>
	struct is_stdarray<std::array<T, N>>
	{
		static const bool value = true;
	};

	template <class T>
	struct is_map
	{
		static const bool value = false;
	};

	template <class T, class Alloc>
	struct is_map<std::map<T, Alloc>>
	{
		static const bool value = true;
	};

	template <class T, class Alloc>
	struct is_map<std::unordered_map<T, Alloc>>
	{
		static const bool value = true;
	};

}

class BorshEncoder
{
public:
	template <typename ... T>
	constexpr BorshEncoder& Encode(const T& ... types) {
	    // Encode((types...));
		(Encode(types), ...);
		// printf("asd");
	    return *this;
	}

	template <typename T>
	constexpr BorshEncoder &Encode(const T value)
	{
		if constexpr (std::is_integral<T>::value)
		{
			const size_t typeSize = sizeof(T);
			uint8_t offset = 0;

			for (size_t i = 0; i < typeSize; i++)
			{
				m_Buffer.push_back((value >> offset) & 0xff);
				offset += 8;
			}
		}

		if constexpr (std::is_floating_point<T>::value)
		{
			assert(!std::isnan(value) || "NaN value found");

			// From https://github.com/naphaso/cbor-cpp/blob/master/src/encoder.cpp
			const void *punny = &value;

			for (size_t i = 0; i < sizeof(T); i++)
			{
				m_Buffer.push_back(*((uint8_t *)punny + i));
			}
		}

		return *this;
	}

	BorshEncoder &Encode(const std::string &str)
	{
		// Write the size of the string as an u32 integer
		Encode(static_cast<uint32_t>(str.size()));

		m_Buffer.insert(m_Buffer.begin() + m_Buffer.size(), str.begin(), str.end());

		return *this;
	}

	BorshEncoder &Encode(const char *str)
	{
		const uint32_t size = std::strlen(str);
		// Write the size of the string as an u32 integer
		Encode(size);

		auto bytes = reinterpret_cast<const uint8_t *>(str);

		m_Buffer.insert(m_Buffer.begin() + m_Buffer.size(), bytes, bytes + size);

		return *this;
	}

	template <typename T>
	constexpr BorshEncoder &Encode(const std::initializer_list<T> &initList)
	{
		return Encode(std::pair{initList.begin(), initList.size()});
	}

	template <typename T, typename U>
	constexpr BorshEncoder &Encode( const std::pair<T *, U> cArray)
	{
		return Encode(std::pair{(const T*)cArray.first, cArray.second});
	}

	template <typename T, typename U>
	constexpr BorshEncoder &Encode( const std::pair<const T *, U> cArray)
	{
		auto [array, size] = cArray;

		static_assert(std::is_integral<U>::value || (size > 0), "The size of the c array must be a unsigned integer value");

		if constexpr (std::is_array<T>::value)
		{
			printf("Es array");
		}

		if constexpr (std::is_integral<T>::value)
		{
			for (size_t i = 0; i < size; ++i)
			{
				Encode(*array);
				++array;
			}
		}
		else if constexpr (std::is_floating_point<T>::value)
		{
			for (size_t i = 0; i < size; ++i)
			{
				Encode(*array);
				++array;
			}
		}
		else if constexpr (BorshCppInternals::is_string<T>::value)
		{
			for (size_t i = 0; i < size; ++i)
			{
				Encode(*array);
				++array;
			}
		}
		else if constexpr (std::is_same<const char *, T>::value)
		{
			for (size_t i = 0; i < size; ++i)
			{
				Encode(*array);
				++array;
			}
		}
		else
		{
			assert(false || "The type of the array is not supported");
		}

		return *this;
	}

	template <typename T>
	constexpr BorshEncoder &Encode(const std::vector<T> &vector)
	{
		Encode((uint32_t)vector.size());
		Encode(std::pair{vector.data(), vector.size()});

		return *this;
	}

	const std::vector<uint8_t> &GetBuffer() const
	{
		return m_Buffer;
	}

private:
	std::vector<uint8_t> m_Buffer;
};

class BorshDecoder
{
public:
	template <typename ... Types>
	std::tuple<Types...> Decode(const uint8_t *bufferBegin)
	{
		uint8_t *offset = (uint8_t *)bufferBegin;
		// (DecodeInternal<Types>(f, &offset), ...);
		assert(false || "BROKEN!!!!");
		return std::make_tuple(DecodeInternal<Types>(&offset)...);
	}

private:
	template <typename T>
	T DecodeInternal(uint8_t **offset)
	{
		if constexpr (std::is_integral<T>::value || std::is_floating_point<T>::value)
		{
			T data = *((T *)*(offset));

			(*offset) += sizeof(T);

			return data;
			// f(std::move(data));
		}
		else if constexpr (BorshCppInternals::is_string<T>::value)
		{
			uint32_t strSize = *((uint32_t *)*(offset));
			std::cout << typeid(T).name() << "\n";
			(*offset) += 4;

			auto data = std::string((*offset), ((*offset) + strSize));

			(*offset) += strSize;

			return data;
			// f(std::move(data));
		}
		else
		{
			assert(false || "The type of the array is not supported");
		}
	}

private:
	std::vector<uint8_t> m_Buffer;
};
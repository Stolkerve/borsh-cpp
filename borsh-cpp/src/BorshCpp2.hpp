//
// Created by Mike Loomis on 6/22/2019.
//

// https://github.com/mikeloomisgg/cppack

/*
 * Esta es una implementacion de borsh de la implementacion de mspack
 * de https://github.com/mikeloomisgg/cppack
 */

#ifndef BORSH_CPP_HPP
#define BORSH_CPP_HPP

#include <vector>
#include <set>
#include <list>
#include <map>
#include <unordered_map>
#include <array>
#include <chrono>
#include <cmath>
#include <bitset>
#include <system_error>

namespace borshcpp
{

  template <class T>
  struct IsContainer
  {
    static const bool value = false;
  };

  template <class T, class Alloc>
  struct IsContainer<std::vector<T, Alloc>>
  {
    static const bool value = true;
  };

  template <class T, class Alloc>
  struct IsContainer<std::list<T, Alloc>>
  {
    static const bool value = true;
  };

  template <class T, class Alloc>
  struct IsContainer<std::map<T, Alloc>>
  {
    static const bool value = true;
  };

  template <class T, class Alloc>
  struct IsContainer<std::unordered_map<T, Alloc>>
  {
    static const bool value = true;
  };

  template <class T, class Alloc>
  struct IsContainer<std::set<T, Alloc>>
  {
    static const bool value = true;
  };

  template <class T>
  struct IsStdArray
  {
    static const bool value = false;
  };

  template <class T, std::size_t N>
  struct IsStdArray<std::array<T, N>>
  {
    static const bool value = true;
  };

  template <class T>
  struct IsMap
  {
    static const bool value = false;
  };

  template <class T, class Alloc>
  struct IsMap<std::map<T, Alloc>>
  {
    static const bool value = true;
  };

  template <class T, class Alloc>
  struct IsMap<std::unordered_map<T, Alloc>>
  {
    static const bool value = true;
  };

  class Packer
  {
  public:
    template <class... Types>
    void operator()(const Types &...args)
    {
      (PackType(std::forward<const Types &>(args)), ...);
    }

    template <class... Types>
    void process(const Types &...args)
    {
      (PackType(std::forward<const Types &>(args)), ...);
    }

    const std::vector<uint8_t> &GetBuffer() const
    {
      return m_Buffer;
    }

    void Clear()
    {
      m_Buffer.clear();
    }

  private:
    std::vector<uint8_t> m_Buffer;

    template <class T>
    void PackType(const T &value)
    {
      if constexpr (IsMap<T>::value)
      {
        PackMap(value);
      }
      else if constexpr (IsContainer<T>::value || IsStdArray<T>::value)
      {
        PackArray(value);
      }
      else
      {
        auto recursive_packer = Packer{};
        const_cast<T &>(value).pack(recursive_packer);
        PackType(recursive_packer.vector());
      }
    }

    template <class T>
    void PackArray(const T &array)
    {
    }

    template <class T>
    void PackMap(const T &map)
    {
    }

    template <class T>
    void PackInteger(const T &integer)
    {
    }

    template <class T>
    void PackFloatingPoint(const T &integer)
    {
    }
  };

  template <>
  inline void Packer::PackType(const std::nullptr_t & /*value*/)
  {
    // serialized_object.emplace_back(nil);
  }

  template <>
  inline void Packer::PackType(const std::string &value)
  {
    for (char i : value)
    {
      m_Buffer.emplace_back(static_cast<uint8_t>(i));
    }
  }

  // class Unpacker {
  //  public:
  //   Unpacker() : data_pointer(nullptr), data_end(nullptr) {};

  //   Unpacker(const uint8_t *data_start, std::size_t bytes)
  //       : data_pointer(data_start), data_end(data_start + bytes) {};

  //   template<class ... Types>
  //   void operator()(Types &... args) {
  //     (UnPackType(std::forward<Types &>(args)), ...);
  //   }

  //   template<class ... Types>
  //   void process(Types &... args) {
  //     (UnPackType(std::forward<Types &>(args)), ...);
  //   }

  //   void set_data(const uint8_t *pointer, std::size_t size) {
  //     data_pointer = pointer;
  //     data_end = data_pointer + size;
  //   }

  //   std::error_code ec{};

  //  private:
  //   const uint8_t *data_pointer;
  //   const uint8_t *data_end;

  //   uint8_t safe_data() {
  //     if (data_pointer < data_end)
  //       return *data_pointer;
  //     ec = UnpackerError::OutOfRange;
  //     return 0;
  //   }

  //   void safe_increment(int64_t bytes = 1) {
  //     if (data_end - data_pointer >= 0) {
  //       data_pointer += bytes;
  //     } else {
  //       ec = UnpackerError::OutOfRange;
  //     }
  //   }

  //   template<class T>
  //   void UnPackType(T &value) {
  //     if constexpr(IsMap<T>::value) {
  //       unpack_map(value);
  //     } else if constexpr (IsContainer<T>::value) {
  //       unPackArray(value);
  //     } else if constexpr (IsStdArray<T>::value) {
  //       unpack_stdarray(value);
  //     } else {
  //       auto recursive_data = std::vector<uint8_t>{};
  //       UnPackType(recursive_data);

  //       auto recursive_unpacker = Unpacker{recursive_data.data(), recursive_data.size()};
  //       value.pack(recursive_unpacker);
  //       ec = recursive_unpacker.ec;
  //     }
  //   }

  //   template<class Clock, class Duration>
  //   void UnPackType(std::chrono::time_point<Clock, Duration> &value) {
  //     using RepType = typename std::chrono::time_point<Clock, Duration>::rep;
  //     using DurationType = Duration;
  //     using TimepointType = typename std::chrono::time_point<Clock, Duration>;
  //     auto placeholder = RepType{};
  //     UnPackType(placeholder);
  //     value = TimepointType(DurationType(placeholder));
  //   }

  //   template<class T>
  //   void unPackArray(T &array) {
  //     using ValueType = typename T::value_type;
  //     if (safe_data() == array32) {
  //       safe_increment();
  //       std::size_t array_size = 0;
  //       for (auto i = sizeof(uint32_t); i > 0; --i) {
  //         array_size += uint32_t(safe_data()) << 8 * (i - 1);
  //         safe_increment();
  //       }
  //       std::vector<uint32_t> x{};
  //       for (auto i = 0U; i < array_size; ++i) {
  //         ValueType val{};
  //         UnPackType(val);
  //         array.emplace_back(val);
  //       }
  //     } else if (safe_data() == array16) {
  //       safe_increment();
  //       std::size_t array_size = 0;
  //       for (auto i = sizeof(uint16_t); i > 0; --i) {
  //         array_size += uint16_t(safe_data()) << 8 * (i - 1);
  //         safe_increment();
  //       }
  //       for (auto i = 0U; i < array_size; ++i) {
  //         ValueType val{};
  //         UnPackType(val);
  //         array.emplace_back(val);
  //       }
  //     } else {
  //       std::size_t array_size = safe_data() & 0b00001111;
  //       safe_increment();
  //       for (auto i = 0U; i < array_size; ++i) {
  //         ValueType val{};
  //         UnPackType(val);
  //         array.emplace_back(val);
  //       }
  //     }
  //   }

  //   template<class T>
  //   void unpack_stdarray(T &array) {
  //     using ValueType = typename T::value_type;
  //     auto vec = std::vector<ValueType>{};
  //     unpack_array(vec);
  //     std::copy(vec.begin(), vec.end(), array.begin());
  //   }

  //   template<class T>
  //   void unpack_map(T &map) {
  //     using KeyType = typename T::key_type;
  //     using MappedType = typename T::mapped_type;
  //     if (safe_data() == map32) {
  //       safe_increment();
  //       std::size_t map_size = 0;
  //       for (auto i = sizeof(uint32_t); i > 0; --i) {
  //         map_size += uint32_t(safe_data()) << 8 * (i - 1);
  //         safe_increment();
  //       }
  //       std::vector<uint32_t> x{};
  //       for (auto i = 0U; i < map_size; ++i) {
  //         KeyType key{};
  //         MappedType value{};
  //         UnPackType(key);
  //         UnPackType(value);
  //         map.insert_or_assign(key, value);
  //       }
  //     } else if (safe_data() == map16) {
  //       safe_increment();
  //       std::size_t map_size = 0;
  //       for (auto i = sizeof(uint16_t); i > 0; --i) {
  //         map_size += uint16_t(safe_data()) << 8 * (i - 1);
  //         safe_increment();
  //       }
  //       for (auto i = 0U; i < map_size; ++i) {
  //         KeyType key{};
  //         MappedType value{};
  //         UnPackType(key);
  //         UnPackType(value);
  //         map.insert_or_assign(key, value);
  //       }
  //     } else {
  //       std::size_t map_size = safe_data() & 0b00001111;
  //       safe_increment();
  //       for (auto i = 0U; i < map_size; ++i) {
  //         KeyType key{};
  //         MappedType value{};
  //         UnPackType(key);
  //         UnPackType(value);
  //         map.insert_or_assign(key, value);
  //       }
  //     }
  //   }
  // };

  // template<>
  // inline
  // void Unpacker::UnPackType(int8_t &value) {
  //   if (safe_data() == int8) {
  //     safe_increment();
  //     value = safe_data();
  //     safe_increment();
  //   } else {
  //     value = safe_data();
  //     safe_increment();
  //   }
  // }

  // template<>
  // inline
  // void Unpacker::UnPackType(int16_t &value) {
  //   if (safe_data() == int16) {
  //     safe_increment();
  //     std::bitset<16> bits;
  //     for (auto i = sizeof(uint16_t); i > 0; --i) {
  //       bits |= uint16_t(safe_data()) << 8 * (i - 1);
  //       safe_increment();
  //     }
  //     if (bits[15]) {
  //       value = -1 * (uint16_t((~bits).to_ulong()) + 1);
  //     } else {
  //       value = uint16_t(bits.to_ulong());
  //     }
  //   } else if (safe_data() == int8) {
  //     int8_t val;
  //     UnPackType(val);
  //     value = val;
  //   } else {
  //     value = safe_data();
  //     safe_increment();
  //   }
  // }

  // template<>
  // inline
  // void Unpacker::UnPackType(int32_t &value) {
  //   if (safe_data() == int32) {
  //     safe_increment();
  //     std::bitset<32> bits;
  //     for (auto i = sizeof(uint32_t); i > 0; --i) {
  //       bits |= uint32_t(safe_data()) << 8 * (i - 1);
  //       safe_increment();
  //     }
  //     if (bits[31]) {
  //       value = -1 * ((~bits).to_ulong() + 1);
  //     } else {
  //       value = bits.to_ulong();
  //     }
  //   } else if (safe_data() == int16) {
  //     int16_t val;
  //     UnPackType(val);
  //     value = val;
  //   } else if (safe_data() == int8) {
  //     int8_t val;
  //     UnPackType(val);
  //     value = val;
  //   } else {
  //     value = safe_data();
  //     safe_increment();
  //   }
  // }

  // template<>
  // inline
  // void Unpacker::UnPackType(int64_t &value) {
  //   if (safe_data() == int64) {
  //     safe_increment();
  //     std::bitset<64> bits;
  //     for (auto i = sizeof(value); i > 0; --i) {
  //       bits |= std::bitset<8>(safe_data()).to_ullong() << 8 * (i - 1);
  //       safe_increment();
  //     }
  //     if (bits[63]) {
  //       value = -1 * ((~bits).to_ullong() + 1);
  //     } else {
  //       value = bits.to_ullong();
  //     }
  //   } else if (safe_data() == int32) {
  //     int32_t val;
  //     UnPackType(val);
  //     value = val;
  //   } else if (safe_data() == int16) {
  //     int16_t val;
  //     UnPackType(val);
  //     value = val;
  //   } else if (safe_data() == int8) {
  //     int8_t val;
  //     UnPackType(val);
  //     value = val;
  //   } else {
  //     value = safe_data();
  //     safe_increment();
  //   }
  // }

  // template<>
  // inline
  // void Unpacker::UnPackType(uint8_t &value) {
  //   if (safe_data() == uint8) {
  //     safe_increment();
  //     value = safe_data();
  //     safe_increment();
  //   } else {
  //     value = safe_data();
  //     safe_increment();
  //   }
  // }

  // template<>
  // inline
  // void Unpacker::UnPackType(uint16_t &value) {
  //   if (safe_data() == uint16) {
  //     safe_increment();
  //     for (auto i = sizeof(uint16_t); i > 0; --i) {
  //       value += safe_data() << 8 * (i - 1);
  //       safe_increment();
  //     }
  //   } else if (safe_data() == uint8) {
  //     safe_increment();
  //     value = safe_data();
  //     safe_increment();
  //   } else {
  //     value = safe_data();
  //     safe_increment();
  //   }
  // }

  // template<>
  // inline
  // void Unpacker::UnPackType(uint32_t &value) {
  //   if (safe_data() == uint32) {
  //     safe_increment();
  //     for (auto i = sizeof(uint32_t); i > 0; --i) {
  //       value += safe_data() << 8 * (i - 1);
  //       safe_increment();
  //     }
  //   } else if (safe_data() == uint16) {
  //     safe_increment();
  //     for (auto i = sizeof(uint16_t); i > 0; --i) {
  //       value += safe_data() << 8 * (i - 1);
  //       safe_increment();
  //     }
  //   } else if (safe_data() == uint8) {
  //     safe_increment();
  //     value = safe_data();
  //     safe_increment();
  //   } else {
  //     value = safe_data();
  //     safe_increment();
  //   }
  // }

  // template<>
  // inline
  // void Unpacker::UnPackType(uint64_t &value) {
  //   if (safe_data() == uint64) {
  //     safe_increment();
  //     for (auto i = sizeof(uint64_t); i > 0; --i) {
  //       value += uint64_t(safe_data()) << 8 * (i - 1);
  //       safe_increment();
  //     }
  //   } else if (safe_data() == uint32) {
  //     safe_increment();
  //     for (auto i = sizeof(uint32_t); i > 0; --i) {
  //       value += uint64_t(safe_data()) << 8 * (i - 1);
  //       safe_increment();
  //     }
  //     data_pointer++;
  //   } else if (safe_data() == uint16) {
  //     safe_increment();
  //     for (auto i = sizeof(uint16_t); i > 0; --i) {
  //       value += uint64_t(safe_data()) << 8 * (i - 1);
  //       safe_increment();
  //     }
  //   } else if (safe_data() == uint8) {
  //     safe_increment();
  //     value = safe_data();
  //     safe_increment();
  //   } else {
  //     value = safe_data();
  //     safe_increment();
  //   }
  // }

  // template<>
  // inline
  // void Unpacker::UnPackType(std::nullptr_t &/*value*/) {
  //   safe_increment();
  // }

  // template<>
  // inline
  // void Unpacker::UnPackType(bool &value) {
  //   value = safe_data() != 0xc2;
  //   safe_increment();
  // }

  // template<>
  // inline
  // void Unpacker::UnPackType(float &value) {
  //   if (safe_data() == float32) {
  //     safe_increment();
  //     uint32_t data = 0;
  //     for (auto i = sizeof(uint32_t); i > 0; --i) {
  //       data += safe_data() << 8 * (i - 1);
  //       safe_increment();
  //     }
  //     auto bits = std::bitset<32>(data);
  //     auto mantissa = 1.0f;
  //     for (auto i = 23U; i > 0; --i) {
  //       if (bits[i - 1]) {
  //         mantissa += 1.0f / (1 << (24 - i));
  //       }
  //     }
  //     if (bits[31]) {
  //       mantissa *= -1;
  //     }
  //     uint8_t exponent = 0;
  //     for (auto i = 0U; i < 8; ++i) {
  //       exponent += bits[i + 23] << i;
  //     }
  //     exponent -= 127;
  //     value = ldexp(mantissa, exponent);
  //   } else {
  //     if (safe_data() == int8 || safe_data() == int16 || safe_data() == int32 || safe_data() == int64) {
  //       int64_t val = 0;
  //       UnPackType(val);
  //       value = float(val);
  //     } else {
  //       uint64_t val = 0;
  //       UnPackType(val);
  //       value = float(val);
  //     }
  //   }
  // }

  // template<>
  // inline
  // void Unpacker::UnPackType(double &value) {
  //   if (safe_data() == float64) {
  //     safe_increment();
  //     uint64_t data = 0;
  //     for (auto i = sizeof(uint64_t); i > 0; --i) {
  //       data += uint64_t(safe_data()) << 8 * (i - 1);
  //       safe_increment();
  //     }
  //     auto bits = std::bitset<64>(data);
  //     auto mantissa = 1.0;
  //     for (auto i = 52U; i > 0; --i) {
  //       if (bits[i - 1]) {
  //         mantissa += 1.0 / (uint64_t(1) << (53 - i));
  //       }
  //     }
  //     if (bits[63]) {
  //       mantissa *= -1;
  //     }
  //     uint16_t exponent = 0;
  //     for (auto i = 0U; i < 11; ++i) {
  //       exponent += bits[i + 52] << i;
  //     }
  //     exponent -= 1023;
  //     value = ldexp(mantissa, exponent);
  //   } else {
  //     if (safe_data() == int8 || safe_data() == int16 || safe_data() == int32 || safe_data() == int64) {
  //       int64_t val = 0;
  //       UnPackType(val);
  //       value = float(val);
  //     } else {
  //       uint64_t val = 0;
  //       UnPackType(val);
  //       value = float(val);
  //     }
  //   }
  // }

  // template<>
  // inline
  // void Unpacker::UnPackType(std::string &value) {
  //   std::size_t str_size = 0;
  //   if (safe_data() == str32) {
  //     safe_increment();
  //     for (auto i = sizeof(uint32_t); i > 0; --i) {
  //       str_size += uint32_t(safe_data()) << 8 * (i - 1);
  //       safe_increment();
  //     }
  //   } else if (safe_data() == str16) {
  //     safe_increment();
  //     for (auto i = sizeof(uint16_t); i > 0; --i) {
  //       str_size += uint16_t(safe_data()) << 8 * (i - 1);
  //       safe_increment();
  //     }
  //   } else if (safe_data() == str8) {
  //     safe_increment();
  //     for (auto i = sizeof(uint8_t); i > 0; --i) {
  //       str_size += uint8_t(safe_data()) << 8 * (i - 1);
  //       safe_increment();
  //     }
  //   } else {
  //     str_size = safe_data() & 0b00011111;
  //     safe_increment();
  //   }
  //   if (data_pointer + str_size <= data_end) {
  //     value = std::string{data_pointer, data_pointer + str_size};
  //     safe_increment(str_size);
  //   } else {
  //     ec = UnpackerError::OutOfRange;
  //   }
  // }

  // template<>
  // inline
  // void Unpacker::UnPackType(std::vector<uint8_t> &value) {
  //   std::size_t bin_size = 0;
  //   if (safe_data() == bin32) {
  //     safe_increment();
  //     for (auto i = sizeof(uint32_t); i > 0; --i) {
  //       bin_size += uint32_t(safe_data()) << 8 * (i - 1);
  //       safe_increment();
  //     }
  //   } else if (safe_data() == bin16) {
  //     safe_increment();
  //     for (auto i = sizeof(uint16_t); i > 0; --i) {
  //       bin_size += uint16_t(safe_data()) << 8 * (i - 1);
  //       safe_increment();
  //     }
  //   } else {
  //     safe_increment();
  //     for (auto i = sizeof(uint8_t); i > 0; --i) {
  //       bin_size += uint8_t(safe_data()) << 8 * (i - 1);
  //       safe_increment();
  //     }
  //   }
  //   if (data_pointer + bin_size <= data_end) {
  //     value = std::vector<uint8_t>{data_pointer, data_pointer + bin_size};
  //     safe_increment(bin_size);
  //   } else {
  //     ec = UnpackerError::OutOfRange;
  //   }
  // }

  // template<class PackableObject>
  // std::vector<uint8_t> pack(PackableObject &obj) {
  //   auto packer = Packer{};
  //   obj.pack(packer);
  //   return packer.vector();
  // }

  // template<class PackableObject>
  // std::vector<uint8_t> pack(PackableObject &&obj) {
  //   auto packer = Packer{};
  //   obj.pack(packer);
  //   return packer.vector();
  // }

  // template<class UnpackableObject>
  // UnpackableObject unpack(const uint8_t *data_start, const std::size_t size, std::error_code &ec) {
  //   auto obj = UnpackableObject{};
  //   auto unpacker = Unpacker(data_start, size);
  //   obj.pack(unpacker);
  //   ec = unpacker.ec;
  //   return obj;
  // }

  // template<class UnpackableObject>
  // UnpackableObject unpack(const uint8_t *data_start, const std::size_t size) {
  //   std::error_code ec{};
  //   return unpack<UnpackableObject>(data_start, size, ec);
  // }

  // template<class UnpackableObject>
  // UnpackableObject unpack(const std::vector<uint8_t> &data, std::error_code &ec) {
  //   return unpack<UnpackableObject>(data.data(), data.size(), ec);
  // }

  // template<class UnpackableObject>
  // UnpackableObject unpack(const std::vector<uint8_t> &data) {
  //   std::error_code ec;
  //   return unpack<UnpackableObject>(data.data(), data.size(), ec);
  // }
}

#endif // BORSH_CPP_HPP

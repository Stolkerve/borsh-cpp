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
  enum class UnpackerError
  {
    OutOfRange = 1
  };

  struct UnpackerErrCategory : public std::error_category
  {
  public:
    const char *name() const noexcept override
    {
      return "unpacker";
    };

    std::string message(int ev) const override
    {
      switch (static_cast<borshcpp::UnpackerError>(ev))
      {
      case borshcpp::UnpackerError::OutOfRange:
        return "tried to dereference out of range during deserialization";
      default:
        return "(unrecognized error)";
      }
    };
  };

  const UnpackerErrCategory theUnpackerErrCategory{};

  inline std::error_code make_error_code(borshcpp::UnpackerError e)
  {
    return {static_cast<int>(e), theUnpackerErrCategory};
  }
}

namespace std
{
  template <>
  struct is_error_code_enum<borshcpp::UnpackerError> : public true_type
  {
  };
}

namespace borshcpp
{

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

  class Packer
  {
  public:
    template <class... Types>
    void operator()(const Types &...args)
    {
      (pack_type(std::forward<const Types &>(args)), ...);
    }

    template <class... Types>
    void process(const Types &...args)
    {
      (pack_type(std::forward<const Types &>(args)), ...);
    }

    const std::vector<uint8_t> &vector() const
    {
      return serialized_object;
    }

    void clear()
    {
      serialized_object.clear();
    }

  private:
    std::vector<uint8_t> serialized_object;

    template <class T>
    void pack_type(const T &value)
    {
      if constexpr (is_map<T>::value)
      {
        pack_map(value);
      }
      else if constexpr (is_container<T>::value || is_stdarray<T>::value)
      {
        pack_array(value);
      }
      else
      {
        auto recursive_packer = Packer{};
        const_cast<T &>(value).pack(recursive_packer);
        pack_type(recursive_packer.vector());
      }
    }

    template <class T>
    void pack_array(const T &array)
    {
      // if (array.size() < 16) {
      //   auto size_mask = uint8_t(0b10010000);
      //   serialized_object.emplace_back(uint8_t(array.size() | size_mask));
      // } else if (array.size() < std::numeric_limits<uint16_t>::max()) {
      //   serialized_object.emplace_back(array16);
      //   for (auto i = sizeof(uint16_t); i > 0; --i) {
      //     serialized_object.emplace_back(uint8_t(array.size() >> (8U * (i - 1)) & 0xff));
      //   }
      // } else if (array.size() < std::numeric_limits<uint32_t>::max()) {
      //   serialized_object.emplace_back(array32);
      //   for (auto i = sizeof(uint32_t); i > 0; --i) {
      //     serialized_object.emplace_back(uint8_t(array.size() >> (8U * (i - 1)) & 0xff));
      //   }
      // } else {
      //   return; // Give up if string is too long
      // }
      // for (const auto &elem : array) {
      //   pack_type(elem);
      // }
    }

    template <class T>
    void pack_map(const T &map)
    {
      // if (map.size() < 16) {
      //   auto size_mask = uint8_t(0b10000000);
      //   serialized_object.emplace_back(uint8_t(map.size() | size_mask));
      // } else if (map.size() < std::numeric_limits<uint16_t>::max()) {
      //   serialized_object.emplace_back(map16);
      //   for (auto i = sizeof(uint16_t); i > 0; --i) {
      //     serialized_object.emplace_back(uint8_t(map.size() >> (8U * (i - 1)) & 0xff));
      //   }
      // } else if (map.size() < std::numeric_limits<uint32_t>::max()) {
      //   serialized_object.emplace_back(map32);
      //   for (auto i = sizeof(uint32_t); i > 0; --i) {
      //     serialized_object.emplace_back(uint8_t(map.size() >> (8U * (i - 1)) & 0xff));
      //   }
      // }
      // for (const auto &elem : map) {
      //   pack_type(std::get<0>(elem));
      //   pack_type(std::get<1>(elem));
      // }
    }
  };

  template <>
  inline void Packer::pack_type(const std::nullptr_t & /*value*/)
  {
    // serialized_object.emplace_back(nil);
  }

  template <>
  inline void Packer::pack_type(const std::string &value)
  {
    // if (value.size() < 32) {
    //   serialized_object.emplace_back(uint8_t(value.size()) | 0b10100000);
    // } else if (value.size() < std::numeric_limits<uint8_t>::max()) {
    //   serialized_object.emplace_back(str8);
    //   serialized_object.emplace_back(uint8_t(value.size()));
    // } else if (value.size() < std::numeric_limits<uint16_t>::max()) {
    //   serialized_object.emplace_back(str16);
    //   for (auto i = sizeof(uint16_t); i > 0; --i) {
    //     serialized_object.emplace_back(uint8_t(value.size() >> (8U * (i - 1)) & 0xff));
    //   }
    // } else if (value.size() < std::numeric_limits<uint32_t>::max()) {
    //   serialized_object.emplace_back(str32);
    //   for (auto i = sizeof(uint32_t); i > 0; --i) {
    //     serialized_object.emplace_back(uint8_t(value.size() >> (8U * (i - 1)) & 0xff));
    //   }
    // } else {
    //   return; // Give up if string is too long
    // }
    for (char i : value)
    {
      serialized_object.emplace_back(static_cast<uint8_t>(i));
    }
  }

  // class Unpacker {
  //  public:
  //   Unpacker() : data_pointer(nullptr), data_end(nullptr) {};

  //   Unpacker(const uint8_t *data_start, std::size_t bytes)
  //       : data_pointer(data_start), data_end(data_start + bytes) {};

  //   template<class ... Types>
  //   void operator()(Types &... args) {
  //     (unpack_type(std::forward<Types &>(args)), ...);
  //   }

  //   template<class ... Types>
  //   void process(Types &... args) {
  //     (unpack_type(std::forward<Types &>(args)), ...);
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
  //   void unpack_type(T &value) {
  //     if constexpr(is_map<T>::value) {
  //       unpack_map(value);
  //     } else if constexpr (is_container<T>::value) {
  //       unpack_array(value);
  //     } else if constexpr (is_stdarray<T>::value) {
  //       unpack_stdarray(value);
  //     } else {
  //       auto recursive_data = std::vector<uint8_t>{};
  //       unpack_type(recursive_data);

  //       auto recursive_unpacker = Unpacker{recursive_data.data(), recursive_data.size()};
  //       value.pack(recursive_unpacker);
  //       ec = recursive_unpacker.ec;
  //     }
  //   }

  //   template<class Clock, class Duration>
  //   void unpack_type(std::chrono::time_point<Clock, Duration> &value) {
  //     using RepType = typename std::chrono::time_point<Clock, Duration>::rep;
  //     using DurationType = Duration;
  //     using TimepointType = typename std::chrono::time_point<Clock, Duration>;
  //     auto placeholder = RepType{};
  //     unpack_type(placeholder);
  //     value = TimepointType(DurationType(placeholder));
  //   }

  //   template<class T>
  //   void unpack_array(T &array) {
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
  //         unpack_type(val);
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
  //         unpack_type(val);
  //         array.emplace_back(val);
  //       }
  //     } else {
  //       std::size_t array_size = safe_data() & 0b00001111;
  //       safe_increment();
  //       for (auto i = 0U; i < array_size; ++i) {
  //         ValueType val{};
  //         unpack_type(val);
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
  //         unpack_type(key);
  //         unpack_type(value);
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
  //         unpack_type(key);
  //         unpack_type(value);
  //         map.insert_or_assign(key, value);
  //       }
  //     } else {
  //       std::size_t map_size = safe_data() & 0b00001111;
  //       safe_increment();
  //       for (auto i = 0U; i < map_size; ++i) {
  //         KeyType key{};
  //         MappedType value{};
  //         unpack_type(key);
  //         unpack_type(value);
  //         map.insert_or_assign(key, value);
  //       }
  //     }
  //   }
  // };

  // template<>
  // inline
  // void Unpacker::unpack_type(int8_t &value) {
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
  // void Unpacker::unpack_type(int16_t &value) {
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
  //     unpack_type(val);
  //     value = val;
  //   } else {
  //     value = safe_data();
  //     safe_increment();
  //   }
  // }

  // template<>
  // inline
  // void Unpacker::unpack_type(int32_t &value) {
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
  //     unpack_type(val);
  //     value = val;
  //   } else if (safe_data() == int8) {
  //     int8_t val;
  //     unpack_type(val);
  //     value = val;
  //   } else {
  //     value = safe_data();
  //     safe_increment();
  //   }
  // }

  // template<>
  // inline
  // void Unpacker::unpack_type(int64_t &value) {
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
  //     unpack_type(val);
  //     value = val;
  //   } else if (safe_data() == int16) {
  //     int16_t val;
  //     unpack_type(val);
  //     value = val;
  //   } else if (safe_data() == int8) {
  //     int8_t val;
  //     unpack_type(val);
  //     value = val;
  //   } else {
  //     value = safe_data();
  //     safe_increment();
  //   }
  // }

  // template<>
  // inline
  // void Unpacker::unpack_type(uint8_t &value) {
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
  // void Unpacker::unpack_type(uint16_t &value) {
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
  // void Unpacker::unpack_type(uint32_t &value) {
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
  // void Unpacker::unpack_type(uint64_t &value) {
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
  // void Unpacker::unpack_type(std::nullptr_t &/*value*/) {
  //   safe_increment();
  // }

  // template<>
  // inline
  // void Unpacker::unpack_type(bool &value) {
  //   value = safe_data() != 0xc2;
  //   safe_increment();
  // }

  // template<>
  // inline
  // void Unpacker::unpack_type(float &value) {
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
  //       unpack_type(val);
  //       value = float(val);
  //     } else {
  //       uint64_t val = 0;
  //       unpack_type(val);
  //       value = float(val);
  //     }
  //   }
  // }

  // template<>
  // inline
  // void Unpacker::unpack_type(double &value) {
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
  //       unpack_type(val);
  //       value = float(val);
  //     } else {
  //       uint64_t val = 0;
  //       unpack_type(val);
  //       value = float(val);
  //     }
  //   }
  // }

  // template<>
  // inline
  // void Unpacker::unpack_type(std::string &value) {
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
  // void Unpacker::unpack_type(std::vector<uint8_t> &value) {
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

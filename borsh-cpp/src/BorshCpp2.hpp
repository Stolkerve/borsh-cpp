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
#include <cmath>
#include <string>
#include <string_view>

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
  struct IsInitList
  {
    static const bool value = false;
  };

  template <class T>
  struct IsInitList<std::initializer_list<T>>
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
      if constexpr (std::is_integral<T>::value)
      {
        PackInteger(value);
      }
      else if constexpr (std::is_floating_point<T>::value)
      {
        PackFloatingPoint(value);
      }
      else if constexpr (IsMap<T>::value)
      {
        PackMap(value);
      }
      else if constexpr (IsContainer<T>::value || IsStdArray<T>::value)
      {
        PackArray(value);
      }
    }

    template <class T>
    void PackArray(const T &array)
    {
      printf("Array\n");
      for (const auto &elem : array)
      {
        PackType(elem);
      }
    }

    template <class T>
    void PackMap(const T &map)
    {
      printf("Map\n");
    }

    template <class T>
    void PackInteger(const T &integer)
    {
      printf("Integer\n");
    }

    template <class T>
    void PackFloatingPoint(const T &integer)
    {
      printf("Float\n");
    }
  };

  template <>
  inline void Packer::PackType(const std::nullptr_t & /*value*/)
  {
    printf("Nullptr\n");
  }

  template <>
  inline void Packer::PackType(const std::string &value)
  {
    printf("std::string\n");
    // for (char i : value)
    // {
    // m_Buffer.emplace_back(static_cast<uint8_t>(i));
    // }
  }

  template <>
  inline void Packer::PackType(const std::string_view &value)
  {
    printf("std::string_view\n");
    // for (char i : value)
    // {
    // m_Buffer.emplace_back(static_cast<uint8_t>(i));
    // }
  }

  // template <typename T>
  // inline void Packer::PackType(const std::initializer_list<T> &value)
  // {
  //   printf("std::string\n");
  //   // for (char i : value)
  //   // {
  //     // m_Buffer.emplace_back(static_cast<uint8_t>(i));
  //   // }
  // }

  template <class PackableObject>
  std::vector<uint8_t> pack(PackableObject &obj)
  {
    auto packer = Packer{};
    obj.pack(packer);
    return packer.GetBuffer();
  }

  template <class PackableObject>
  std::vector<uint8_t> pack(PackableObject &&obj)
  {
    auto packer = Packer{};
    obj.pack(packer);
    return packer.GetBuffer();
  }

}

#endif // BORSH_CPP_HPP

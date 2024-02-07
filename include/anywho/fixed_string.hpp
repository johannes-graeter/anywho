#pragma once

#include <array>
#include <cstring>
#include <string>

namespace anywho {

/**
 * @brief FixedString without dynamic memory allocation
 *
 * @tparam N Maximum length of the string
 */
template<size_t N> class FixedString final
{
private:
  std::array<char, N> data{};

public:
  FixedString() { data[0] = '\0'; }


  FixedString(const FixedString &other) { data = other.data; }

  FixedString(FixedString &&other) noexcept : data(std::move(other.data)) {}

  FixedString(const std::string &str) { assign(str); }
  FixedString(const char *str) { assign(str); }

  ~FixedString() = default;

  FixedString &operator=(FixedString &&other) noexcept
  {
    if (this != &other) { data = std::move(other.data); }

    return *this;
  }

  FixedString &operator=(const FixedString &other)
  {
    if (this != &other) { data = other.data; }

    return *this;
  }

  FixedString &operator=(const std::string &str)
  {
    assign(str);

    return *this;
  }

  FixedString &operator=(const char *str)
  {
    assign(str);

    return *this;
  }

  const char &operator[](size_t index) const { return data[index]; }

  char &operator[](size_t index) { return data[index]; }

  operator std::string() const { return std::string(data.data()); }

private:
  void assign(const std::string &str)
  {
    const size_t max_index = std::min(str.size(), N - 1);
    for (size_t i = 0; i < max_index; ++i) { data[i] = str[i]; }
    data[max_index] = '\0';
  }

  void assign(const char *str)
  {
    const size_t max_index = std::min(strlen(str), N - 1);
    for (size_t i = 0; i < max_index; ++i) { data[i] = str[i]; }
    data[max_index] = '\0';
  }
};

}// namespace anywho
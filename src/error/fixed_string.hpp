#pragma once

#include <array>
#include <string>

namespace anywho {

template <size_t N> class FixedString final {
private:
  std::array<char, N> data{};

public:
  FixedString() { data[0] = '\0'; }

  explicit FixedString(const char *str) {
    if (strlen(str) >= N) {
	throw std::runtime_error("input string size is longer than FixedString size");
    }

    for (size_t i = 0; i < strlen(str); ++i) {
      data[i] = str[i];
    }
    data[strlen(str)] = '\0';
  }

  FixedString(const FixedString &other) { data = other.data; }

  FixedString(FixedString &&other) noexcept : data(std::move(other.data)) {}

  ~FixedString() = default;

  FixedString &operator=(FixedString &&other) noexcept {
    if (this != &other) {
      data = std::move(other.data);
    }

    return *this;
  }

  FixedString &operator=(const FixedString &other) {
    if (this != &other) {
      data = other.data;
    }

    return *this;
  }

  const char &operator[](size_t index) const { return data[index]; }

  char &operator[](size_t index) { return data[index]; }

  operator std::string() const { return std::string(data.data()); }
};

} // namespace anywho
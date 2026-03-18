//
// Created by peter on 16/02/2025.
//

#pragma once

/// calculate the number of bits needed to store n
constexpr int bitCount(const int n) {
  auto nn = static_cast<unsigned int>(n);
  int count = 0;
  while (nn > 0) {
    nn >>= 1;
    ++count;
  }
  return count - 1;  /// Subtract 1 because count includes the bit for 2^0
}

constexpr int numValuesWithBits(int n) {
  return (n >= 0) ? (1 << n) : 0;  // 1 << n is equivalent to 2^n
}

constexpr bool isPowerOf2(int n) {
  return n > 0 && (n & (n - 1)) == 0;
}

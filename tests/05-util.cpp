// Tests for util.h -- bitCount, numValuesWithBits, isPowerOf2

#include "util.h"

#include "gtest/gtest.h"

// ---------------------------------------------------------------------------
// bitCount
// ---------------------------------------------------------------------------

TEST(TEST_05_Util, 00_BitCountZeroReturnsMinusOne) {
  EXPECT_EQ(-1, bitCount(0));
}

TEST(TEST_05_Util, 01_BitCountPowersOfTwo) {
  EXPECT_EQ(0, bitCount(1));
  EXPECT_EQ(1, bitCount(2));
  EXPECT_EQ(2, bitCount(4));
  EXPECT_EQ(3, bitCount(8));
  EXPECT_EQ(4, bitCount(16));
}

TEST(TEST_05_Util, 02_BitCountNonPowerOfTwo) {
  EXPECT_EQ(1, bitCount(3));   // highest bit of 3 is bit 1
  EXPECT_EQ(2, bitCount(7));   // highest bit of 7 is bit 2
  EXPECT_EQ(3, bitCount(15));  // highest bit of 15 is bit 3
  EXPECT_EQ(7, bitCount(255)); // highest bit of 255 is bit 7
}

// ---------------------------------------------------------------------------
// numValuesWithBits
// ---------------------------------------------------------------------------

TEST(TEST_05_Util, 10_NumValuesWithBitsZero) {
  EXPECT_EQ(1, numValuesWithBits(0));  // 2^0 = 1
}

TEST(TEST_05_Util, 11_NumValuesWithBitsPositive) {
  EXPECT_EQ(2, numValuesWithBits(1));
  EXPECT_EQ(4, numValuesWithBits(2));
  EXPECT_EQ(8, numValuesWithBits(3));
  EXPECT_EQ(256, numValuesWithBits(8));
}

TEST(TEST_05_Util, 12_NumValuesWithBitsNegative) {
  EXPECT_EQ(0, numValuesWithBits(-1));
  EXPECT_EQ(0, numValuesWithBits(-5));
}

// ---------------------------------------------------------------------------
// isPowerOf2
// ---------------------------------------------------------------------------

TEST(TEST_05_Util, 20_IsPowerOf2True) {
  EXPECT_TRUE(isPowerOf2(1));
  EXPECT_TRUE(isPowerOf2(2));
  EXPECT_TRUE(isPowerOf2(4));
  EXPECT_TRUE(isPowerOf2(16));
  EXPECT_TRUE(isPowerOf2(256));
}

TEST(TEST_05_Util, 21_IsPowerOf2False) {
  EXPECT_FALSE(isPowerOf2(0));
  EXPECT_FALSE(isPowerOf2(3));
  EXPECT_FALSE(isPowerOf2(6));
  EXPECT_FALSE(isPowerOf2(255));
}

TEST(TEST_05_Util, 22_IsPowerOf2NegativeIsFalse) {
  EXPECT_FALSE(isPowerOf2(-1));
  EXPECT_FALSE(isPowerOf2(-4));
}

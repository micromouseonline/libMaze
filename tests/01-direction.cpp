
// Created by peter on 12/02/25.
//

#include "direction.h"
#include "gtest/gtest.h"

using namespace MazeLib;

class TEST_01_directions : public ::testing::Test {
 protected:
  virtual void SetUp() {
  }

  virtual void TearDown() {
  }
};

TEST_F(TEST_01_directions, 00_Sizes) {
  EXPECT_EQ(1, sizeof(Direction));
}

TEST_F(TEST_01_directions, 01_constructor) {
  Direction d(Absolute::South);
  EXPECT_EQ(4, (int)d);
  Direction dir(3);
  EXPECT_EQ(3, (int)dir);
  EXPECT_EQ(Absolute::SouthEast, dir);
}

TEST_F(TEST_01_directions, 05_CardinalsAreOrtho) {
  for (auto& d : Direction::cardinals) {
    Direction dir(d);
    EXPECT_TRUE(dir.isOrthogonal());
    EXPECT_FALSE(dir.isDiagonal());
  }
}

TEST_F(TEST_01_directions, 10_OrdinalListIsDiagonal) {
  for (auto& d : Direction::ordinals) {
    Direction dir(d);
    EXPECT_FALSE(dir.isOrthogonal());
    EXPECT_TRUE(dir.isDiagonal());
  }
}

TEST_F(TEST_01_directions, 15_DirectionChars) {
  EXPECT_EQ('^', Direction(Absolute::North).toAbsoluteChar());
  EXPECT_EQ('/', Direction(Absolute::NorthEast).toAbsoluteChar());
  EXPECT_EQ('>', Direction(Absolute::East).toAbsoluteChar());
  EXPECT_EQ('\\', Direction(Absolute::SouthEast).toAbsoluteChar());
  EXPECT_EQ('v', Direction(Absolute::South).toAbsoluteChar());
  EXPECT_EQ('/', Direction(Absolute::SouthWest).toAbsoluteChar());
  EXPECT_EQ('<', Direction(Absolute::West).toAbsoluteChar());
  EXPECT_EQ('\\', Direction(Absolute::NorthWest).toAbsoluteChar());
}

TEST_F(TEST_01_directions, 20_DirectionAsNumbers) {
  EXPECT_EQ(0, Direction(Absolute::North));
  EXPECT_EQ(1, Direction(Absolute::NorthEast));
  EXPECT_EQ(2, Direction(Absolute::East));
  EXPECT_EQ(3, Direction(Absolute::SouthEast));
  EXPECT_EQ(4, Direction(Absolute::South));
  EXPECT_EQ(5, Direction(Absolute::SouthWest));
  EXPECT_EQ(6, Direction(Absolute::West));
  EXPECT_EQ(7, Direction(Absolute::NorthWest));

  EXPECT_EQ(Absolute::North, Direction(0));
  EXPECT_EQ(Absolute::NorthEast, Direction(1));
  EXPECT_EQ(Absolute::East, Direction(2));
  EXPECT_EQ(Absolute::SouthEast, Direction(3));
  EXPECT_EQ(Absolute::South, Direction(4));
  EXPECT_EQ(Absolute::SouthWest, Direction(5));
  EXPECT_EQ(Absolute::West, Direction(6));
  EXPECT_EQ(Absolute::NorthWest, Direction(7));
}

TEST_F(TEST_01_directions, 25_Overflows) {
  EXPECT_EQ(Absolute::North, Direction(8 + Absolute::North));
  EXPECT_EQ(1, Direction(8 + Absolute::NorthEast));
  EXPECT_EQ(2, Direction(8 + Absolute::East));
  EXPECT_EQ(3, Direction(8 + Absolute::SouthEast));
  EXPECT_EQ(4, Direction(8 + Absolute::South));
  EXPECT_EQ(5, Direction(8 + Absolute::SouthWest));
  EXPECT_EQ(6, Direction(8 + Absolute::West));
  EXPECT_EQ(7, Direction(8 + Absolute::NorthWest));
}

TEST_F(TEST_01_directions, 26_Arithmetic) {
  EXPECT_EQ(Relative::Left, Direction(Absolute::East - Absolute::South));
  EXPECT_EQ(Absolute::East, Direction(Absolute::South + Relative::Left));
  EXPECT_EQ(Relative::Back, Direction(Relative::Right + Relative::Right));
  EXPECT_EQ(Relative::Right, Direction(-Relative::Left));
}

TEST_F(TEST_01_directions, 30_Turns) {
  EXPECT_EQ(Absolute::North, Direction(Relative::Front + Absolute::North));
  EXPECT_EQ(Absolute::NorthEast, Direction(Relative::Front + Absolute::NorthEast));
  EXPECT_EQ(Absolute::East, Direction(Relative::Front + Absolute::East));
  EXPECT_EQ(Absolute::SouthEast, Direction(Relative::Front + Absolute::SouthEast));
  EXPECT_EQ(Absolute::South, Direction(Relative::Front + Absolute::South));
  EXPECT_EQ(Absolute::SouthWest, Direction(Relative::Front + Absolute::SouthWest));
  EXPECT_EQ(Absolute::West, Direction(Relative::Front + Absolute::West));
  EXPECT_EQ(Absolute::NorthWest, Direction(Relative::Front + Absolute::NorthWest));
}

TEST_F(TEST_01_directions, 35_Turns) {
  EXPECT_EQ(Absolute::North, Direction(Absolute::North + Relative::Front));
  EXPECT_EQ(Absolute::NorthEast, Direction(Absolute::North + Relative::Right45));
  EXPECT_EQ(Absolute::East, Direction(Absolute::North + Relative::Right));
  EXPECT_EQ(Absolute::SouthEast, Direction(Absolute::North + Relative::Right135));
  EXPECT_EQ(Absolute::South, Direction(Absolute::North + Relative::Back));
  EXPECT_EQ(Absolute::SouthWest, Direction(Absolute::North + Relative::Left135));
  EXPECT_EQ(Absolute::West, Direction(Absolute::North + Relative::Left));
  EXPECT_EQ(Absolute::NorthWest, Direction(Absolute::North + Relative::Left45));
}

TEST_F(TEST_01_directions, 40_calcAbsolute) {
  // Test absolute direction with no relative direction
  EXPECT_EQ(Absolute::North, Direction::getNewDirection(Absolute::North, Relative::Front));

  // Test absolute direction with relative direction that doesn't change the absolute direction
  EXPECT_EQ(Absolute::South, Direction::getNewDirection(Absolute::North, Relative::Back));

  // Test absolute direction with relative direction that changes the absolute direction
  EXPECT_EQ(Absolute::West, Direction::getNewDirection(Absolute::North, Relative::Left));
  EXPECT_EQ(Absolute::East, Direction::getNewDirection(Absolute::North, Relative::Right));
  EXPECT_EQ(Absolute::NorthWest, Direction::getNewDirection(Absolute::North, Relative::Left45));
  EXPECT_EQ(Absolute::NorthEast, Direction::getNewDirection(Absolute::North, Relative::Right45));
  EXPECT_EQ(Absolute::SouthWest, Direction::getNewDirection(Absolute::North, Relative::Left135));
  EXPECT_EQ(Absolute::SouthEast, Direction::getNewDirection(Absolute::North, Relative::Right135));
}

TEST_F(TEST_01_directions, 50_differences) {
  EXPECT_EQ(0, Direction(Absolute::North) - Direction(Absolute::North));
  EXPECT_EQ(7, Direction(Absolute::North) - Direction(Absolute::NorthEast));
  EXPECT_EQ(6, Direction(Absolute::North) - Direction(Absolute::East));
  EXPECT_EQ(5, Direction(Absolute::North) - Direction(Absolute::SouthEast));
  EXPECT_EQ(4, Direction(Absolute::North) - Direction(Absolute::South));
  EXPECT_EQ(4, Direction(Absolute::South) - Direction(Absolute::North));
  EXPECT_EQ(3, Direction(Absolute::North) - Direction(Absolute::SouthWest));
  EXPECT_EQ(2, Direction(Absolute::North) - Direction(Absolute::West));
  EXPECT_EQ(1, Direction(Absolute::North) - Direction(Absolute::NorthWest));
}

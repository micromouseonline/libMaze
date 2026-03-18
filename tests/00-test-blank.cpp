
// Created by peter on 12/02/25.
//

#include "direction.h"
#include "maze.h"

#include "gtest/gtest.h"

class TEST_00_BlankMaze : public ::testing::Test {
 protected:
  virtual void SetUp() {}

  virtual void TearDown() {}
};

TEST_F(TEST_00_BlankMaze, HomeIsZero) {
  Maze maze(16);
  EXPECT_EQ(maze.home(), 0);
}

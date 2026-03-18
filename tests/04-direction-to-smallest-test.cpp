// Tests for distance functions (manhattanDistance, chebyshevDistance) and directionToSmallest.
//
// Coordinate system:
//   cell = col * 16 + row
//   row increases going North  (row 0 = South edge, row 15 = North edge)
//   col increases going East   (col 0 = West  edge, col 15 = East  edge)
//   cell   0 = SW corner  (col 0, row 0)
//   cell 0x77 = (col 7, row 7)  -- classic contest goal centre

#include "maze.h"
#include "mazeconstants.h"

#include "gtest/gtest.h"

static constexpr uint16_t WIDTH = 16;
static constexpr uint16_t GOAL = 0x77;
static constexpr uint16_t HOME = 0;

class TEST_04_DirectionToSmallest : public ::testing::Test {
 protected:
  Maze maze{WIDTH};

  void SetUp() override { maze.resetToEmptyMaze(); }
};

// ---------------------------------------------------------------------------
// 00: manhattanDistance tests
// ---------------------------------------------------------------------------

TEST_F(TEST_04_DirectionToSmallest, 00_ManhattanSameCell) {
  EXPECT_EQ(0u, maze.manhattanDistance(0x00, 0x00));
}

TEST_F(TEST_04_DirectionToSmallest, 01_ManhattanAdjacentNorth) {
  // (0,0) -> (0,1): dy=1, dx=0
  EXPECT_EQ(1u, maze.manhattanDistance(0x00, 0x01));
}

TEST_F(TEST_04_DirectionToSmallest, 02_ManhattanAdjacentEast) {
  // (0,0) -> (1,0): dx=1, dy=0
  EXPECT_EQ(1u, maze.manhattanDistance(0x00, 0x10));
}

TEST_F(TEST_04_DirectionToSmallest, 03_ManhattanDiagonal) {
  // (0,0) -> (1,1): dx=1, dy=1
  EXPECT_EQ(2u, maze.manhattanDistance(0x00, 0x11));
}

TEST_F(TEST_04_DirectionToSmallest, 04_ManhattanLargerDistance) {
  // (0,0) -> (5,3): dx=5, dy=3
  EXPECT_EQ(8u, maze.manhattanDistance(0x00, 0x53));
}

TEST_F(TEST_04_DirectionToSmallest, 05_ManhattanSymmetric) {
  uint16_t d1 = maze.manhattanDistance(0x00, 0x53);
  uint16_t d2 = maze.manhattanDistance(0x53, 0x00);
  EXPECT_EQ(d1, d2);
}

TEST_F(TEST_04_DirectionToSmallest, 06_ManhattanHomeToGoal) {
  // (0,0) -> (7,7): dx=7, dy=7 => 14
  EXPECT_EQ(14u, maze.manhattanDistance(HOME, GOAL));
}

// ---------------------------------------------------------------------------
// 10: chebyshevDistance tests
// ---------------------------------------------------------------------------

TEST_F(TEST_04_DirectionToSmallest, 10_ChebyshevSameCell) {
  EXPECT_EQ(0u, maze.chebyshevDistance(0x00, 0x00));
}

TEST_F(TEST_04_DirectionToSmallest, 11_ChebyshevAdjacentNorth) {
  // (0,0) -> (0,1): max(0,1) = 1
  EXPECT_EQ(1u, maze.chebyshevDistance(0x00, 0x01));
}

TEST_F(TEST_04_DirectionToSmallest, 12_ChebyshevAdjacentEast) {
  // (0,0) -> (1,0): max(1,0) = 1
  EXPECT_EQ(1u, maze.chebyshevDistance(0x00, 0x10));
}

TEST_F(TEST_04_DirectionToSmallest, 13_ChebyshevDiagonal) {
  // (0,0) -> (1,1): max(1,1) = 1
  EXPECT_EQ(1u, maze.chebyshevDistance(0x00, 0x11));
}

TEST_F(TEST_04_DirectionToSmallest, 14_ChebyshevLargerDistance) {
  // (0,0) -> (5,3): max(5,3) = 5
  EXPECT_EQ(5u, maze.chebyshevDistance(0x00, 0x53));
}

TEST_F(TEST_04_DirectionToSmallest, 15_ChebyshevSymmetric) {
  uint16_t d1 = maze.chebyshevDistance(0x00, 0x53);
  uint16_t d2 = maze.chebyshevDistance(0x53, 0x00);
  EXPECT_EQ(d1, d2);
}

TEST_F(TEST_04_DirectionToSmallest, 16_ChebyshevHomeToGoal) {
  // (0,0) -> (7,7): max(7,7) = 7
  EXPECT_EQ(7u, maze.chebyshevDistance(HOME, GOAL));
}

// ---------------------------------------------------------------------------
// 20: directionToSmallest(cell) -- no target parameter
// ---------------------------------------------------------------------------

TEST_F(TEST_04_DirectionToSmallest, 20_DirectionAllMaxCost) {
  // Fresh maze: all neighbors have MAX_COST
  uint16_t cell = 0x55;
  EXPECT_EQ(INVALID_DIRECTION, maze.directionToSmallest(cell));
}

TEST_F(TEST_04_DirectionToSmallest, 21_DirectionSingleLowestNorth) {
  uint16_t cell = 0x55;
  uint16_t north = maze.cellNorth(cell);
  maze.setCost(north, 10);
  EXPECT_EQ(NORTH, maze.directionToSmallest(cell));
}

TEST_F(TEST_04_DirectionToSmallest, 22_DirectionSingleLowestEast) {
  uint16_t cell = 0x55;
  uint16_t east = maze.cellEast(cell);
  maze.setCost(east, 10);
  EXPECT_EQ(EAST, maze.directionToSmallest(cell));
}

TEST_F(TEST_04_DirectionToSmallest, 23_DirectionSingleLowestSouth) {
  uint16_t cell = 0x55;
  uint16_t south = maze.cellSouth(cell);
  maze.setCost(south, 10);
  EXPECT_EQ(SOUTH, maze.directionToSmallest(cell));
}

TEST_F(TEST_04_DirectionToSmallest, 24_DirectionSingleLowestWest) {
  uint16_t cell = 0x55;
  uint16_t west = maze.cellWest(cell);
  maze.setCost(west, 10);
  EXPECT_EQ(WEST, maze.directionToSmallest(cell));
}

TEST_F(TEST_04_DirectionToSmallest, 25_DirectionPicksSmallestAmongMultiple) {
  uint16_t cell = 0x55;
  maze.setCost(maze.cellNorth(cell), 5);
  maze.setCost(maze.cellEast(cell), 3);
  maze.setCost(maze.cellSouth(cell), 7);
  maze.setCost(maze.cellWest(cell), 4);
  EXPECT_EQ(EAST, maze.directionToSmallest(cell));
}

// ---------------------------------------------------------------------------
// 30: directionToSmallest(cell, target) -- with target parameter for tiebreaker
// ---------------------------------------------------------------------------

TEST_F(TEST_04_DirectionToSmallest, 30_DirectionWithTargetNoTie) {
  uint16_t cell = 0x55;
  uint16_t target = 0x77;
  maze.setCost(maze.cellNorth(cell), 5);
  maze.setCost(maze.cellEast(cell), 3);
  maze.setCost(maze.cellSouth(cell), 7);
  maze.setCost(maze.cellWest(cell), 4);
  // East has lowest cost (3), should be chosen
  EXPECT_EQ(EAST, maze.directionToSmallest(cell, target));
}

TEST_F(TEST_04_DirectionToSmallest, 31_DirectionWithTargetTieBrokenByProximity) {
  uint16_t cell = 0x55;
  uint16_t target = 0x77;
  // Set North and East to same cost (5)
  maze.setCost(maze.cellEast(cell), 5);
  maze.setCost(maze.cellSouth(cell), 5);
  maze.setCost(maze.cellNorth(cell), 7);
  maze.setCost(maze.cellWest(cell), 9);
  // East is at (6,5), South is at (5,4), target is (7,7)
  // Chebyshev from East: max(|6-7|,|5-7|) = max(1,2) = 2
  // Chebyshev from South: max(|5-7|,|4-7|) = max(2,3) = 3
  // East is closer, should be selected
  EXPECT_EQ(EAST, maze.directionToSmallest(cell, target));
}

TEST_F(TEST_04_DirectionToSmallest, 32_DirectionWithTargetAllMaxCost) {
  uint16_t cell = 0x55;
  uint16_t target = 0x77;
  // When all costs are MAX_COST (all exits blocked), should return INVALID_DIRECTION.
  EXPECT_EQ(INVALID_DIRECTION, maze.directionToSmallest(cell, target));
}

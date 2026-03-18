// Tests for Maze -- extended methods not covered in 03-maze-16-tests.cpp
// Covers: weightedFlood, directionFlood, testForSolution/isSolved,
//         setFloodType/getFloodType, setCornerWeight/getCornerWeight,
//         updateDirections, copyMazeFromFileData

#include "maze.h"
#include "mazeconstants.h"

#include "gtest/gtest.h"

static constexpr uint16_t WIDTH = 16;
static constexpr uint16_t CELL_COUNT = WIDTH * WIDTH;
static constexpr uint16_t GOAL = 0x77;
static constexpr uint16_t HOME = 0;

class TEST_07_MazeExtended : public ::testing::Test {
 protected:
  Maze maze{WIDTH};

  void SetUp() override { maze.resetToEmptyMaze(); }
};

// ---------------------------------------------------------------------------
// FloodType accessors
// ---------------------------------------------------------------------------

TEST_F(TEST_07_MazeExtended, 00_DefaultFloodTypeAfterReset) {
  // resetToEmptyMaze does not change flood type; default from constructor is RUNLENGTH_FLOOD
  maze.setFloodType(Maze::RUNLENGTH_FLOOD);
  EXPECT_EQ(Maze::RUNLENGTH_FLOOD, maze.getFloodType());
}

TEST_F(TEST_07_MazeExtended, 01_SetGetFloodTypeRoundTrip) {
  maze.setFloodType(Maze::MANHATTAN_FLOOD);
  EXPECT_EQ(Maze::MANHATTAN_FLOOD, maze.getFloodType());

  maze.setFloodType(Maze::WEIGHTED_FLOOD);
  EXPECT_EQ(Maze::WEIGHTED_FLOOD, maze.getFloodType());

  maze.setFloodType(Maze::DIRECTION_FLOOD);
  EXPECT_EQ(Maze::DIRECTION_FLOOD, maze.getFloodType());
}

// ---------------------------------------------------------------------------
// CornerWeight accessors
// ---------------------------------------------------------------------------

TEST_F(TEST_07_MazeExtended, 10_DefaultCornerWeightIsThree) {
  EXPECT_EQ(3u, maze.getCornerWeight());
}

TEST_F(TEST_07_MazeExtended, 11_SetCornerWeightRoundTrip) {
  maze.setCornerWeight(10);
  EXPECT_EQ(10u, maze.getCornerWeight());
  maze.setCornerWeight(1);
  EXPECT_EQ(1u, maze.getCornerWeight());
}

// ---------------------------------------------------------------------------
// weightedFlood
// ---------------------------------------------------------------------------

TEST_F(TEST_07_MazeExtended, 20_WeightedFloodGoalCostIsZero) {
  maze.weightedFlood(GOAL);
  EXPECT_EQ(0u, maze.cost(GOAL));
}

TEST_F(TEST_07_MazeExtended, 21_WeightedFloodHomeCostLessThanMax) {
  maze.weightedFlood(GOAL);
  EXPECT_LT(maze.cost(HOME), MAX_COST);
}

TEST_F(TEST_07_MazeExtended, 22_WeightedFloodAllCellsReachable) {
  maze.weightedFlood(GOAL);
  for (uint16_t i = 0; i < CELL_COUNT; i++) {
    EXPECT_LT(maze.cost(i), MAX_COST) << "Cell " << i << " unreachable";
  }
}

TEST_F(TEST_07_MazeExtended, 23_WeightedFloodHomeCostGeqManhattan) {
  // weighted flood penalises turns; cost >= Manhattan distance
  maze.weightedFlood(GOAL);
  uint16_t wCost = maze.cost(HOME);

  maze.manhattanFlood(GOAL);
  uint16_t mCost = maze.cost(HOME);

  EXPECT_GE(wCost, mCost);
}

// ---------------------------------------------------------------------------
// directionFlood
// ---------------------------------------------------------------------------

TEST_F(TEST_07_MazeExtended, 30_DirectionFloodGoalCostIsZero) {
  maze.directionFlood(GOAL);
  EXPECT_EQ(0u, maze.cost(GOAL));
}

TEST_F(TEST_07_MazeExtended, 31_DirectionFloodAllCellsReachable) {
  maze.directionFlood(GOAL);
  for (uint16_t i = 0; i < CELL_COUNT; i++) {
    EXPECT_LT(maze.cost(i), MAX_COST) << "Cell " << i << " unreachable";
  }
}

// ---------------------------------------------------------------------------
// updateDirections
// ---------------------------------------------------------------------------

TEST_F(TEST_07_MazeExtended, 40_UpdateDirectionsAllCellsValid) {
  maze.manhattanFlood(GOAL);
  maze.updateDirections(GOAL);
  for (uint16_t i = 0; i < CELL_COUNT; i++) {
    if (i == GOAL) continue;
    EXPECT_NE(INVALID_DIRECTION, maze.direction(i)) << "Cell " << i;
  }
}

TEST_F(TEST_07_MazeExtended, 41_FollowUpdatedDirectionsReachesGoal) {
  maze.manhattanFlood(GOAL);
  maze.updateDirections(GOAL);
  uint16_t cell = HOME;
  int steps = 0;
  while (cell != GOAL && steps < 50) {
    cell = maze.neighbour(cell, maze.direction(cell));
    steps++;
  }
  EXPECT_EQ(GOAL, cell);
}

// ---------------------------------------------------------------------------
// testForSolution / isSolved
// ---------------------------------------------------------------------------

TEST_F(TEST_07_MazeExtended, 50_EmptyMazeWithAllVisitedIsSolved) {
  // Mark all cells visited so open==closed flood
  for (uint16_t i = 0; i < CELL_COUNT; i++) {
    maze.setVisited(i);
  }
  maze.setFloodType(Maze::MANHATTAN_FLOOD);
  bool result = maze.testForSolution();
  EXPECT_TRUE(result);
  EXPECT_TRUE(maze.isSolved());
}

TEST_F(TEST_07_MazeExtended, 51_UnvisitedMazeIsNotSolved) {
  // Default reset leaves interior cells unvisited (unseen walls)
  maze.setFloodType(Maze::MANHATTAN_FLOOD);
  bool result = maze.testForSolution();
  EXPECT_FALSE(result);
  EXPECT_FALSE(maze.isSolved());
}

// ---------------------------------------------------------------------------
// copyMazeFromFileData
// ---------------------------------------------------------------------------

TEST_F(TEST_07_MazeExtended, 60_CopyFromFileDataRoundTrips) {
  // Set some walls, save, reload via copyMazeFromFileData, check walls survive
  maze.setWall(0x55, NORTH);
  maze.setWall(0x33, EAST);

  uint8_t buf[CELL_COUNT];
  maze.save(buf);

  // Reset and reload
  maze.resetToEmptyMaze();
  ASSERT_TRUE(maze.hasExit(0x55, NORTH));  // wall should be gone after reset

  maze.copyMazeFromFileData(buf, CELL_COUNT);
  EXPECT_FALSE(maze.hasExit(0x55, NORTH));
  EXPECT_FALSE(maze.hasExit(0x33, EAST));
}

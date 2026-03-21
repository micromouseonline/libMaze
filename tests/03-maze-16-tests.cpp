// Tests for the Maze class -- 16x16 maze only.
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
static constexpr uint16_t CELL_COUNT = WIDTH * WIDTH;  // 256
static constexpr uint16_t GOAL = 0x77;                 // 119 -- centre of goal area
static constexpr uint16_t HOME = 0;

class TEST_03_Maze_16 : public ::testing::Test {
 protected:
  Maze maze{WIDTH};

  void SetUp() override { maze.resetToEmptyMaze(); }
};

// ---------------------------------------------------------------------------
// 00: Construction and dimensions
// ---------------------------------------------------------------------------

TEST_F(TEST_03_Maze_16, 00_Width) {
  EXPECT_EQ(WIDTH, maze.width());
}

TEST_F(TEST_03_Maze_16, 01_NumCells) {
  EXPECT_EQ(CELL_COUNT, maze.numCells());
}

TEST_F(TEST_03_Maze_16, 02_Home) {
  EXPECT_EQ(0u, maze.home());
}

// ---------------------------------------------------------------------------
// 10: Static direction utilities
// ---------------------------------------------------------------------------

TEST_F(TEST_03_Maze_16, 10_AheadIsIdentity) {
  EXPECT_EQ(NORTH, Maze::ahead(NORTH));
  EXPECT_EQ(EAST, Maze::ahead(EAST));
  EXPECT_EQ(SOUTH, Maze::ahead(SOUTH));
  EXPECT_EQ(WEST, Maze::ahead(WEST));
}

TEST_F(TEST_03_Maze_16, 11_RightOfCyclesNESWN) {
  EXPECT_EQ(EAST, Maze::rightOf(NORTH));
  EXPECT_EQ(SOUTH, Maze::rightOf(EAST));
  EXPECT_EQ(WEST, Maze::rightOf(SOUTH));
  EXPECT_EQ(NORTH, Maze::rightOf(WEST));
}

TEST_F(TEST_03_Maze_16, 12_LeftOfCyclesNWSEN) {
  EXPECT_EQ(WEST, Maze::leftOf(NORTH));
  EXPECT_EQ(NORTH, Maze::leftOf(EAST));
  EXPECT_EQ(EAST, Maze::leftOf(SOUTH));
  EXPECT_EQ(SOUTH, Maze::leftOf(WEST));
}

TEST_F(TEST_03_Maze_16, 13_BehindReversesDirection) {
  EXPECT_EQ(SOUTH, Maze::behind(NORTH));
  EXPECT_EQ(WEST, Maze::behind(EAST));
  EXPECT_EQ(NORTH, Maze::behind(SOUTH));
  EXPECT_EQ(EAST, Maze::behind(WEST));
}

TEST_F(TEST_03_Maze_16, 14_OppositeEqualsBehind) {
  EXPECT_EQ(Maze::behind(NORTH), Maze::opposite(NORTH));
  EXPECT_EQ(Maze::behind(EAST), Maze::opposite(EAST));
  EXPECT_EQ(Maze::behind(SOUTH), Maze::opposite(SOUTH));
  EXPECT_EQ(Maze::behind(WEST), Maze::opposite(WEST));
}

TEST_F(TEST_03_Maze_16, 15_LeftRightAreInverses) {
  EXPECT_EQ(NORTH, Maze::leftOf(Maze::rightOf(NORTH)));
  EXPECT_EQ(EAST, Maze::leftOf(Maze::rightOf(EAST)));
  EXPECT_EQ(SOUTH, Maze::leftOf(Maze::rightOf(SOUTH)));
  EXPECT_EQ(WEST, Maze::leftOf(Maze::rightOf(WEST)));
}

TEST_F(TEST_03_Maze_16, 16_BehindIsItsOwnInverse) {
  EXPECT_EQ(NORTH, Maze::behind(Maze::behind(NORTH)));
  EXPECT_EQ(EAST, Maze::behind(Maze::behind(EAST)));
}

TEST_F(TEST_03_Maze_16, 17_DifferenceBetween) {
  // Result encodes the turn needed: 0=AHEAD, 1=RIGHT, 2=BEHIND, 3=LEFT
  EXPECT_EQ(0, Maze::differenceBetween(NORTH, NORTH));  // ahead
  EXPECT_EQ(1, Maze::differenceBetween(NORTH, EAST));   // right
  EXPECT_EQ(2, Maze::differenceBetween(NORTH, SOUTH));  // behind
  EXPECT_EQ(3, Maze::differenceBetween(NORTH, WEST));   // left
  // Wrap-around
  EXPECT_EQ(1, Maze::differenceBetween(WEST, NORTH));  // right (3->0)
  EXPECT_EQ(3, Maze::differenceBetween(EAST, NORTH));  // left  (1->0)
}

// ---------------------------------------------------------------------------
// 20: Cell neighbour navigation
// ---------------------------------------------------------------------------

TEST_F(TEST_03_Maze_16, 20_CellNorthIncreasesRow) {
  EXPECT_EQ(1u, maze.cellNorth(0));    // (col 0, row 0) -> (col 0, row 1)
  EXPECT_EQ(17u, maze.cellNorth(16));  // (col 1, row 0) -> (col 1, row 1)
}

TEST_F(TEST_03_Maze_16, 21_CellEastIncreasesCol) {
  EXPECT_EQ(16u, maze.cellEast(0));   // (col 0, row 0) -> (col 1, row 0)
  EXPECT_EQ(32u, maze.cellEast(16));  // (col 1, row 0) -> (col 2, row 0)
}

TEST_F(TEST_03_Maze_16, 22_CellSouthDecreasesRow) {
  EXPECT_EQ(0u, maze.cellSouth(1));
  EXPECT_EQ(1u, maze.cellSouth(2));
}

TEST_F(TEST_03_Maze_16, 23_CellWestDecreasesCol) {
  EXPECT_EQ(0u, maze.cellWest(16));
  EXPECT_EQ(16u, maze.cellWest(32));
}

TEST_F(TEST_03_Maze_16, 24_NeighbourMatchesCellFunctions) {
  uint16_t cell = 0x55;  // (col 5, row 5) -- interior
  EXPECT_EQ(maze.cellNorth(cell), maze.neighbour(cell, NORTH));
  EXPECT_EQ(maze.cellEast(cell), maze.neighbour(cell, EAST));
  EXPECT_EQ(maze.cellSouth(cell), maze.neighbour(cell, SOUTH));
  EXPECT_EQ(maze.cellWest(cell), maze.neighbour(cell, WEST));
}

TEST_F(TEST_03_Maze_16, 25_NorthSouthRoundTrip) {
  uint16_t cell = 0x55;
  EXPECT_EQ(cell, maze.cellSouth(maze.cellNorth(cell)));
  EXPECT_EQ(cell, maze.cellNorth(maze.cellSouth(cell)));
}

TEST_F(TEST_03_Maze_16, 26_EastWestRoundTrip) {
  uint16_t cell = 0x55;
  EXPECT_EQ(cell, maze.cellWest(maze.cellEast(cell)));
  EXPECT_EQ(cell, maze.cellEast(maze.cellWest(cell)));
}

// ---------------------------------------------------------------------------
// 30: Wall storage and hasExit after resetToEmptyMaze
// ---------------------------------------------------------------------------

TEST_F(TEST_03_Maze_16, 30_Cell0HasOnlyNorthExit) {
  // reset gives cell 0: south wall (boundary), west wall (boundary),
  // east wall (start post), north cleared explicitly
  EXPECT_TRUE(maze.hasExit(0, NORTH));
  EXPECT_FALSE(maze.hasExit(0, EAST));
  EXPECT_FALSE(maze.hasExit(0, SOUTH));
  EXPECT_FALSE(maze.hasExit(0, WEST));
}

TEST_F(TEST_03_Maze_16, 31_NorthRowCellsHaveNoNorthExit) {
  // row 15 cells (cells 15, 31, 47, ...) have a north boundary wall
  EXPECT_FALSE(maze.hasExit(15, NORTH));   // col 0, row 15
  EXPECT_FALSE(maze.hasExit(31, NORTH));   // col 1, row 15
  EXPECT_FALSE(maze.hasExit(255, NORTH));  // col 15, row 15
}

TEST_F(TEST_03_Maze_16, 32_WestColCellsHaveNoWestExit) {
  // col 0 cells (0..15) have a west boundary wall
  EXPECT_FALSE(maze.hasExit(0, WEST));
  EXPECT_FALSE(maze.hasExit(7, WEST));
  EXPECT_FALSE(maze.hasExit(15, WEST));
}

TEST_F(TEST_03_Maze_16, 33_EastColCellsHaveNoEastExit) {
  // col 15 cells (240..255) have an east boundary wall
  EXPECT_FALSE(maze.hasExit(240, EAST));
  EXPECT_FALSE(maze.hasExit(247, EAST));
  EXPECT_FALSE(maze.hasExit(255, EAST));
}

TEST_F(TEST_03_Maze_16, 34_SouthRowCellsHaveNoSouthExit) {
  // row 0 cells (0, 16, 32, ..., 240) have a south boundary wall
  EXPECT_FALSE(maze.hasExit(0, SOUTH));
  EXPECT_FALSE(maze.hasExit(16, SOUTH));
  EXPECT_FALSE(maze.hasExit(32, SOUTH));
  EXPECT_FALSE(maze.hasExit(240, SOUTH));
}

TEST_F(TEST_03_Maze_16, 35_InteriorCellHasAllFourExits) {
  uint16_t cell = 0x55;  // col 5, row 5 -- no boundary, no start post
  EXPECT_TRUE(maze.hasExit(cell, NORTH));
  EXPECT_TRUE(maze.hasExit(cell, EAST));
  EXPECT_TRUE(maze.hasExit(cell, SOUTH));
  EXPECT_TRUE(maze.hasExit(cell, WEST));
}

TEST_F(TEST_03_Maze_16, 36_SetWallRemovesExit) {
  uint16_t cell = 0x55;
  ASSERT_TRUE(maze.hasExit(cell, NORTH));
  maze.setWall(cell, NORTH);
  EXPECT_FALSE(maze.hasExit(cell, NORTH));
}

TEST_F(TEST_03_Maze_16, 37_SetWallPropagatestoNeighbour) {
  uint16_t cell = 0x55;
  maze.setWall(cell, NORTH);
  EXPECT_FALSE(maze.hasExit(maze.cellNorth(cell), SOUTH));
}

TEST_F(TEST_03_Maze_16, 38_ClearWallRestoresExit) {
  uint16_t cell = 0x55;
  maze.setWall(cell, NORTH);
  maze.clearWall(cell, NORTH);
  EXPECT_TRUE(maze.hasExit(cell, NORTH));
  EXPECT_TRUE(maze.hasExit(maze.cellNorth(cell), SOUTH));
}

// ---------------------------------------------------------------------------
// 40: Visited state
// ---------------------------------------------------------------------------

TEST_F(TEST_03_Maze_16, 40_InteriorCellNotVisitedAfterReset) {
  // Interior cells still have unseen flags on all four walls
  EXPECT_FALSE(maze.isVisited(0x55));
}

TEST_F(TEST_03_Maze_16, 41_Cell0IsFullyVisitedAfterReset) {
  // Cell 0 has all four walls explicitly set or cleared by resetToEmptyMaze
  EXPECT_TRUE(maze.isVisited(HOME));
}

TEST_F(TEST_03_Maze_16, 42_SetVisitedMarksAllWallsSeen) {
  uint16_t cell = 0x55;
  ASSERT_FALSE(maze.isVisited(cell));
  maze.setVisited(cell);
  EXPECT_TRUE(maze.isVisited(cell));
}

TEST_F(TEST_03_Maze_16, 43_ClearVisitedRestoresUnseenFlags) {
  uint16_t cell = 0x55;
  maze.setVisited(cell);
  maze.clearVisited(cell);
  EXPECT_FALSE(maze.isVisited(cell));
}

TEST_F(TEST_03_Maze_16, 44_IsVisitedPerDirectionAfterSetWall) {
  uint16_t cell = 0x55;
  maze.setWall(cell, NORTH);
  EXPECT_TRUE(maze.isVisited(cell, NORTH));
  // Other directions remain unseen
  EXPECT_FALSE(maze.isVisited(cell, EAST));
  EXPECT_FALSE(maze.isVisited(cell, SOUTH));
  EXPECT_FALSE(maze.isVisited(cell, WEST));
}

// ---------------------------------------------------------------------------
// 50: updateMap -- search-time wall update
// ---------------------------------------------------------------------------

TEST_F(TEST_03_Maze_16, 50_UpdateMapSetsWallOnUnseenCell) {
  uint16_t cell = 0x55;
  maze.updateMap(cell, WALL_NORTH);
  EXPECT_FALSE(maze.hasExit(cell, NORTH));
  EXPECT_TRUE(maze.isVisited(cell, NORTH));
}

TEST_F(TEST_03_Maze_16, 51_UpdateMapClearsWallOnUnseenCell) {
  uint16_t cell = 0x55;
  maze.updateMap(cell, 0);  // no walls detected
  EXPECT_TRUE(maze.hasExit(cell, NORTH));
  EXPECT_TRUE(maze.isVisited(cell, NORTH));
}

TEST_F(TEST_03_Maze_16, 52_UpdateMapDoesNotOverwriteSeenWall) {
  uint16_t cell = 0x55;
  maze.setWall(cell, NORTH);           // marks wall present + seen
  ASSERT_FALSE(maze.hasExit(cell, NORTH));
  maze.updateMap(cell, 0);             // sensor says no wall -- must be ignored
  EXPECT_FALSE(maze.hasExit(cell, NORTH));
}

TEST_F(TEST_03_Maze_16, 53_UpdateMapPropagatesWallToNeighbour) {
  uint16_t cell = 0x55;
  uint16_t north = maze.cellNorth(cell);
  maze.updateMap(cell, WALL_NORTH);
  EXPECT_FALSE(maze.hasExit(north, SOUTH));
  EXPECT_TRUE(maze.isVisited(north, SOUTH));
}

TEST_F(TEST_03_Maze_16, 54_UpdateMapSetsAllFourWalls) {
  uint16_t cell = 0x55;
  uint8_t allWalls = WALL_NORTH | WALL_EAST | WALL_SOUTH | WALL_WEST;
  maze.updateMap(cell, allWalls);
  EXPECT_FALSE(maze.hasExit(cell, NORTH));
  EXPECT_FALSE(maze.hasExit(cell, EAST));
  EXPECT_FALSE(maze.hasExit(cell, SOUTH));
  EXPECT_FALSE(maze.hasExit(cell, WEST));
  EXPECT_TRUE(maze.isVisited(cell));
}

// ---------------------------------------------------------------------------
// 60: Cost management
// ---------------------------------------------------------------------------

TEST_F(TEST_03_Maze_16, 60_AllCostsAreMaxAfterClearData) {
  maze.clearData();
  for (uint16_t i = 0; i < CELL_COUNT; i++) {
    EXPECT_EQ(MAX_COST, maze.cost(i));
  }
}

TEST_F(TEST_03_Maze_16, 61_SetCostAndCostRoundTrip) {
  maze.setCost(0x55, 42);
  EXPECT_EQ(42u, maze.cost(0x55));
}

TEST_F(TEST_03_Maze_16, 62_CostThroughWallIsMaxCost) {
  uint16_t cell = 0x55;
  maze.setWall(cell, NORTH);
  maze.setCost(maze.cellNorth(cell), 100);
  // costNorth checks hasExit first; wall is present so returns MAX_COST
  EXPECT_EQ(MAX_COST, maze.cost(cell, NORTH));
}

TEST_F(TEST_03_Maze_16, 63_CostThroughOpenExitReturnsNeighbourCost) {
  uint16_t cell = 0x55;
  maze.setCost(maze.cellNorth(cell), 99);
  EXPECT_EQ(99u, maze.cost(cell, NORTH));
}

TEST_F(TEST_03_Maze_16, 64_DirectionRoundTrip) {
  maze.setDirection(0x55, EAST);
  EXPECT_EQ(EAST, maze.direction(0x55));
}

// ---------------------------------------------------------------------------
// 70: Manhattan flood
// ---------------------------------------------------------------------------

TEST_F(TEST_03_Maze_16, 70_ManhattanFloodGoalCostIsZero) {
  maze.manhattanFlood(GOAL);
  EXPECT_EQ(0u, maze.cost(GOAL));
}

TEST_F(TEST_03_Maze_16, 71_ManhattanFloodHomeCostIs14) {
  // Manhattan distance from (col 0, row 0) to (col 7, row 7) = 14
  maze.manhattanFlood(GOAL);
  EXPECT_EQ(14u, maze.cost(HOME));
}

TEST_F(TEST_03_Maze_16, 72_ManhattanFloodNeighboursOfGoalCostOne) {
  maze.manhattanFlood(GOAL);
  EXPECT_EQ(1u, maze.cost(maze.cellNorth(GOAL)));
  EXPECT_EQ(1u, maze.cost(maze.cellEast(GOAL)));
  EXPECT_EQ(1u, maze.cost(maze.cellSouth(GOAL)));
  EXPECT_EQ(1u, maze.cost(maze.cellWest(GOAL)));
}

TEST_F(TEST_03_Maze_16, 73_ManhattanFloodAllCellsReachable) {
  maze.manhattanFlood(GOAL);
  for (uint16_t i = 0; i < CELL_COUNT; i++) {
    EXPECT_LT(maze.cost(i), MAX_COST) << "Cell " << i << " unreachable";
  }
}

// ---------------------------------------------------------------------------
// 75: RunLength flood
// ---------------------------------------------------------------------------

TEST_F(TEST_03_Maze_16, 75_RunLengthFloodGoalCostIsZero) {
  maze.runLengthFlood(GOAL);
  EXPECT_EQ(0u, maze.cost(GOAL));
}

TEST_F(TEST_03_Maze_16, 76_RunLengthFloodHomeCostLessThanMaxCost) {
  maze.runLengthFlood(GOAL);
  EXPECT_LT(maze.cost(HOME), MAX_COST);
}

TEST_F(TEST_03_Maze_16, 77_RunLengthFloodAllCellsReachable) {
  maze.runLengthFlood(GOAL);
  for (uint16_t i = 0; i < CELL_COUNT; i++) {
    EXPECT_LT(maze.cost(i), MAX_COST) << "Cell " << i << " unreachable";
  }
}

// ---------------------------------------------------------------------------
// 80: Open vs closed flood mask
// ---------------------------------------------------------------------------

// After resetToEmptyMaze, interior cells still have unseen wall flags.
// OPEN_MASK (0x01): only WALL_PRESENT bit checked -- treats unseen as passable.
// CLOSED_MASK (0x11): also checks WALL_UNSEEN bit  -- treats unseen as blocked.

TEST_F(TEST_03_Maze_16, 80_OpenMaskSeesExitThroughUnseenWall) {
  // Cell 1 north is unseen but no wall present; OPEN_MASK must allow exit
  EXPECT_TRUE(maze.hasExit(1, NORTH));
}

TEST_F(TEST_03_Maze_16, 81_OpenFloodCostNotExceedClosedFloodCost) {
  // On a freshly reset maze with interior unseen walls:
  // open flood (optimistic) cost <= closed flood (pessimistic) cost
  maze.setFloodType(Maze::MANHATTAN_FLOOD);
  maze.flood(GOAL, OPEN_MASK);
  uint16_t openCost = maze.cost(HOME);

  maze.setFloodType(Maze::MANHATTAN_FLOOD);
  maze.flood(GOAL, CLOSED_MASK);
  uint16_t closedCost = maze.cost(HOME);

  EXPECT_LE(openCost, closedCost);
}

TEST_F(TEST_03_Maze_16, 82_FullyVisitedMazeOpenAndClosedCostsMatch) {
  // Once all cells are visited (no unseen flags), open and closed flood
  // must produce identical costs
  for (uint16_t i = 0; i < CELL_COUNT; i++) {
    maze.setVisited(i);
  }
  maze.setFloodType(Maze::MANHATTAN_FLOOD);
  maze.flood(GOAL, OPEN_MASK);
  uint16_t openCost = maze.cost(HOME);

  maze.flood(GOAL, CLOSED_MASK);
  uint16_t closedCost = maze.cost(HOME);

  EXPECT_EQ(openCost, closedCost);
}

// ---------------------------------------------------------------------------
// 85: Goal area management
// ---------------------------------------------------------------------------

TEST_F(TEST_03_Maze_16, 85_ResetMazeHasFourCellGoalArea) {
  EXPECT_EQ(4, maze.goalAreaSize());
}

TEST_F(TEST_03_Maze_16, 86_GoalReturnsFrontOfGoalArea) {
  EXPECT_EQ(GOAL, maze.goal());  // 0x77 is added first
}

TEST_F(TEST_03_Maze_16, 87_GoalContainsAllFourCentreGoals) {
  EXPECT_TRUE(maze.goalContains(0x77));
  EXPECT_TRUE(maze.goalContains(0x78));
  EXPECT_TRUE(maze.goalContains(0x87));
  EXPECT_TRUE(maze.goalContains(0x88));
  EXPECT_FALSE(maze.goalContains(HOME));
}

TEST_F(TEST_03_Maze_16, 88_SetGoalReplacesGoalArea) {
  maze.setGoal(42);
  EXPECT_EQ(42u, maze.goal());
  EXPECT_EQ(1, maze.goalAreaSize());
  EXPECT_TRUE(maze.goalContains(42));
  EXPECT_FALSE(maze.goalContains(GOAL));
}

TEST_F(TEST_03_Maze_16, 89_ClearAndAddGoalArea) {
  maze.clearGoalArea();
  EXPECT_EQ(0, maze.goalAreaSize());
  maze.addToGoalArea(10);
  maze.addToGoalArea(20);
  EXPECT_EQ(2, maze.goalAreaSize());
  EXPECT_TRUE(maze.goalContains(10));
  EXPECT_TRUE(maze.goalContains(20));
}

TEST_F(TEST_03_Maze_16, 89a_RemoveFromGoalAreaReducesSize) {
  maze.clearGoalArea();
  maze.addToGoalArea(10);
  maze.addToGoalArea(20);
  maze.addToGoalArea(30);
  ASSERT_EQ(3, maze.goalAreaSize());

  maze.removeFromGoalArea(20);
  EXPECT_EQ(2, maze.goalAreaSize());
  EXPECT_TRUE(maze.goalContains(10));
  EXPECT_FALSE(maze.goalContains(20));
  EXPECT_TRUE(maze.goalContains(30));
}

TEST_F(TEST_03_Maze_16, 89b_RemoveAllFromGoalAreaMakesEmpty) {
  maze.clearGoalArea();
  maze.addToGoalArea(10);
  maze.addToGoalArea(20);
  ASSERT_EQ(2, maze.goalAreaSize());

  maze.removeFromGoalArea(10);
  EXPECT_EQ(1, maze.goalAreaSize());
  maze.removeFromGoalArea(20);
  EXPECT_EQ(0, maze.goalAreaSize());
}

TEST_F(TEST_03_Maze_16, 89c_RemoveNonexistentCellIsNoOp) {
  maze.clearGoalArea();
  maze.addToGoalArea(10);
  maze.addToGoalArea(20);
  ASSERT_EQ(2, maze.goalAreaSize());

  // Remove a cell that doesn't exist
  maze.removeFromGoalArea(100);
  EXPECT_EQ(2, maze.goalAreaSize());
  EXPECT_TRUE(maze.goalContains(10));
  EXPECT_TRUE(maze.goalContains(20));
}

TEST_F(TEST_03_Maze_16, 89d_RemoveFromEmptyGoalAreaIsNoOp) {
  maze.clearGoalArea();
  ASSERT_EQ(0, maze.goalAreaSize());

  maze.removeFromGoalArea(10);
  EXPECT_EQ(0, maze.goalAreaSize());
}

// ---------------------------------------------------------------------------
// 90: Direction update and path following
// ---------------------------------------------------------------------------

TEST_F(TEST_03_Maze_16, 90_AllCellsHaveValidDirectionAfterManhattanFlood) {
  maze.manhattanFlood(GOAL);
  for (uint16_t i = 0; i < CELL_COUNT; i++) {
    if (i == GOAL) {
      continue;
    }
    EXPECT_NE(INVALID_DIRECTION, maze.direction(i)) << "Cell " << i;
  }
}

TEST_F(TEST_03_Maze_16, 91_FollowDirectionsFromHomeReachesGoalIn14Steps) {
  maze.manhattanFlood(GOAL);
  uint16_t cell = HOME;
  int steps = 0;
  while (cell != GOAL && steps < 30) {
    cell = maze.neighbour(cell, maze.direction(cell));
    steps++;
  }
  EXPECT_EQ(GOAL, cell);
  EXPECT_LE(steps, 14);
}

TEST_F(TEST_03_Maze_16, 92_FollowDirectionsAfterRunLengthFloodReachesGoal) {
  maze.runLengthFlood(GOAL);
  uint16_t cell = HOME;
  int steps = 0;
  while (cell != GOAL && steps < 50) {
    cell = maze.neighbour(cell, maze.direction(cell));
    steps++;
  }
  EXPECT_EQ(GOAL, cell);
}

// ---------------------------------------------------------------------------
// 95: Save and load
// ---------------------------------------------------------------------------

TEST_F(TEST_03_Maze_16, 95_SaveLoadRoundTripsWallData) {
  maze.setWall(0x55, NORTH);
  maze.setWall(0x33, EAST);
  maze.setWall(0x12, WEST);

  uint8_t buf[CELL_COUNT];
  maze.save(buf);

  maze.clearData();
  // Verify clearData wiped the walls
  ASSERT_TRUE(maze.hasExit(0x55, NORTH));

  maze.load(buf);
  EXPECT_FALSE(maze.hasExit(0x55, NORTH));
  EXPECT_FALSE(maze.hasExit(0x33, EAST));
  EXPECT_FALSE(maze.hasExit(0x12, WEST));
  // Propagated walls also restored
  EXPECT_FALSE(maze.hasExit(maze.cellNorth(0x55), SOUTH));
  EXPECT_FALSE(maze.hasExit(maze.cellEast(0x33), WEST));
}

TEST_F(TEST_03_Maze_16, 96_SaveLoadPreservesVisitedState) {
  maze.setVisited(0x55);
  ASSERT_TRUE(maze.isVisited(0x55));

  uint8_t buf[CELL_COUNT];
  maze.save(buf);
  maze.clearData();
  maze.load(buf);

  EXPECT_TRUE(maze.isVisited(0x55));
}

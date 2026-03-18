// Tests for MazeSearcher using apec1996 as the real maze.

#include "maze.h"
#include "mazedata.h"
#include "mazeconstants.h"
#include "mazesearcher.h"

#include "gtest/gtest.h"

static constexpr uint16_t WIDTH = 16;
static constexpr uint16_t CELL_COUNT = WIDTH * WIDTH;
static constexpr uint16_t GOAL = 0x77;
static constexpr uint16_t HOME = 0;

/*** Fixture: real maze is apec1996 loaded into a Maze object. */
class TEST_11_MazeSearcher : public ::testing::Test {
 protected:
  Maze realMaze{WIDTH};
  MazeSearcher searcher;

  void SetUp() override {
    realMaze.copyMazeFromFileData(apec1996, CELL_COUNT);
    searcher.setRealMaze(&realMaze);
    searcher.setLocation(HOME);
    searcher.setHeading(NORTH);
  }
};

// ---------------------------------------------------------------------------
// Accessor round-trips
// ---------------------------------------------------------------------------

TEST_F(TEST_11_MazeSearcher, 00_LocationRoundTrip) {
  searcher.setLocation(0x33);
  EXPECT_EQ(0x33u, searcher.location());
}

TEST_F(TEST_11_MazeSearcher, 01_HeadingRoundTrip) {
  searcher.setHeading(EAST);
  EXPECT_EQ(EAST, searcher.heading());
}

TEST_F(TEST_11_MazeSearcher, 02_DefaultLocationIsHome) {
  EXPECT_EQ(HOME, searcher.location());
}

TEST_F(TEST_11_MazeSearcher, 03_MapIsNotNull) {
  EXPECT_NE(nullptr, searcher.map());
}

TEST_F(TEST_11_MazeSearcher, 04_RealMazeMatchesSet) {
  EXPECT_EQ(&realMaze, searcher.realMaze());
}

// ---------------------------------------------------------------------------
// move / turn
// ---------------------------------------------------------------------------

TEST_F(TEST_11_MazeSearcher, 10_MoveNorthAdvancesLocation) {
  searcher.setHeading(NORTH);
  uint16_t before = searcher.location();
  searcher.move();
  // cell = col*16 + row; NORTH increases row by 1
  EXPECT_EQ(before + 1, searcher.location());
}

TEST_F(TEST_11_MazeSearcher, 11_TurnRightFromNorthFacesEast) {
  searcher.setHeading(NORTH);
  searcher.turnRight();
  EXPECT_EQ(EAST, searcher.heading());
}

TEST_F(TEST_11_MazeSearcher, 12_TurnLeftFromNorthFacesWest) {
  searcher.setHeading(NORTH);
  searcher.turnLeft();
  EXPECT_EQ(WEST, searcher.heading());
}

TEST_F(TEST_11_MazeSearcher, 13_TurnAroundFromNorthFacesSouth) {
  searcher.setHeading(NORTH);
  searcher.turnAround();
  EXPECT_EQ(SOUTH, searcher.heading());
}

// ---------------------------------------------------------------------------
// searchTo -- normal flood-based search
// ---------------------------------------------------------------------------

TEST_F(TEST_11_MazeSearcher, 20_NormalSearchReachesGoal) {
  searcher.setSearchMethod(MazeSearcher::SEARCH_NORMAL);
  int steps = searcher.searchTo(GOAL);
  EXPECT_GT(steps, 0);
  EXPECT_EQ(GOAL, searcher.location());
}

TEST_F(TEST_11_MazeSearcher, 21_NormalSearchStepCountReasonable) {
  searcher.setSearchMethod(MazeSearcher::SEARCH_NORMAL);
  int steps = searcher.searchTo(GOAL);
  // Manhattan lower bound is 14; upper bound well within 256
  EXPECT_GE(steps, 14);
  EXPECT_LE(steps, static_cast<int>(CELL_COUNT));
}

// ---------------------------------------------------------------------------
// runTo -- uses pre-loaded map data
// ---------------------------------------------------------------------------

TEST_F(TEST_11_MazeSearcher, 30_RunToReachesGoalWithFullMap) {
  // Load the complete real maze into the internal map so runTo can use it
  searcher.map()->copyMazeFromFileData(apec1996, CELL_COUNT);
  for (uint16_t i = 0; i < CELL_COUNT; i++) {
    searcher.map()->setVisited(i);
  }
  searcher.setLocation(HOME);

  int steps = searcher.runTo(GOAL);
  EXPECT_GT(steps, 0);
  EXPECT_EQ(GOAL, searcher.location());
}

// ---------------------------------------------------------------------------
// Wall-follow modes (smoke: no crash, terminates, visits cells)
// ---------------------------------------------------------------------------

TEST_F(TEST_11_MazeSearcher, 40_LeftWallFollowDoesNotCrash) {
  searcher.setSearchMethod(MazeSearcher::SEARCH_LEFT_WALL);
  // Left-wall follow on an empty map will wander; just check it terminates
  // within the cell budget and does not crash.
  int steps = searcher.searchTo(GOAL);
  EXPECT_NE(MazeSearcher::E_NO_ROUTE, steps);
}

TEST_F(TEST_11_MazeSearcher, 41_RightWallFollowDoesNotCrash) {
  searcher.setSearchMethod(MazeSearcher::SEARCH_RIGHT_WALL);
  int steps = searcher.searchTo(GOAL);
  EXPECT_NE(MazeSearcher::E_NO_ROUTE, steps);
}

// ---------------------------------------------------------------------------
// setMapFromFileData
// ---------------------------------------------------------------------------

TEST_F(TEST_11_MazeSearcher, 50_SetMapFromFileDataLoadsWalls) {
  searcher.setMapFromFileData(apec1996, CELL_COUNT);
  // After loading, the start cell should have south and west walls
  EXPECT_FALSE(searcher.map()->hasExit(HOME, SOUTH));
  EXPECT_FALSE(searcher.map()->hasExit(HOME, WEST));
}

// Tests for PathFinder using known maze data.

#include <cstring>

#include "maze.h"
#include "mazedata.h"
#include "mazeconstants.h"
#include "mazepathfinder.h"

#include "gtest/gtest.h"

static constexpr uint16_t WIDTH = 16;
static constexpr uint16_t CELL_COUNT = WIDTH * WIDTH;
static constexpr uint16_t GOAL = 0x77;
static constexpr uint16_t HOME = 0;

/*** Fixture: loads apec1996, fully visited (all walls seen), floods manhattan. */
class TEST_10_PathFinder : public ::testing::Test {
 protected:
  Maze maze{WIDTH};
  PathFinder pf;

  void SetUp() override {
    maze.copyMazeFromFileData(apec1996, CELL_COUNT);
    // Mark all cells visited so safe/unsafe path behaves identically
    for (uint16_t i = 0; i < CELL_COUNT; i++) {
      maze.setVisited(i);
    }
    maze.setFloodType(Maze::MANHATTAN_FLOOD);
    maze.flood(GOAL, OPEN_MASK);
  }
};

// ---------------------------------------------------------------------------
// generateSafePath
// ---------------------------------------------------------------------------

TEST_F(TEST_10_PathFinder, 00_SafePathStartsWithB) {
  pf.generateSafePath(HOME, GOAL, &maze);
  EXPECT_EQ('B', pf.path()[0]);
}

TEST_F(TEST_10_PathFinder, 01_SafePathReachesTarget) {
  pf.generateSafePath(HOME, GOAL, &maze);
  EXPECT_TRUE(pf.reachesTarget());
}

TEST_F(TEST_10_PathFinder, 02_SafePathCellCountPositive) {
  pf.generateSafePath(HOME, GOAL, &maze);
  EXPECT_GT(pf.cellCount(), 0u);
}

TEST_F(TEST_10_PathFinder, 03_SafePathDistancePositive) {
  pf.generateSafePath(HOME, GOAL, &maze);
  EXPECT_GT(pf.distance(), 0u);
}

TEST_F(TEST_10_PathFinder, 04_SafePathStartCellIsHome) {
  pf.generateSafePath(HOME, GOAL, &maze);
  EXPECT_EQ(HOME, pf.startCell());
}

TEST_F(TEST_10_PathFinder, 05_SafePathEndCellIsGoal) {
  pf.generateSafePath(HOME, GOAL, &maze);
  EXPECT_EQ(GOAL, pf.endCell());
}

TEST_F(TEST_10_PathFinder, 06_SafePathStartHeadingIsValid) {
  pf.generateSafePath(HOME, GOAL, &maze);
  EXPECT_NE(INVALID_DIRECTION, pf.startHeading());
}

TEST_F(TEST_10_PathFinder, 07_SafePathEndHeadingIsValid) {
  pf.generateSafePath(HOME, GOAL, &maze);
  EXPECT_NE(INVALID_DIRECTION, pf.endHeading());
}

TEST_F(TEST_10_PathFinder, 08_SafePathEndsWithSorX) {
  pf.generateSafePath(HOME, GOAL, &maze);
  const char *p = pf.path();
  int len = static_cast<int>(strlen(p));
  ASSERT_GT(len, 0);
  char last = p[len - 1];
  EXPECT_TRUE(last == 'S' || last == 'X') << "Last char: " << last;
}

// ---------------------------------------------------------------------------
// generateUnsafePath
// ---------------------------------------------------------------------------

TEST_F(TEST_10_PathFinder, 10_UnsafePathReachesTarget) {
  pf.generateUnsafePath(HOME, GOAL, &maze);
  EXPECT_TRUE(pf.reachesTarget());
}

TEST_F(TEST_10_PathFinder, 11_UnsafePathCellCountPositive) {
  pf.generateUnsafePath(HOME, GOAL, &maze);
  EXPECT_GT(pf.cellCount(), 0u);
}

// ---------------------------------------------------------------------------
// reversePath
// ---------------------------------------------------------------------------

TEST_F(TEST_10_PathFinder, 20_ReversePathPreservesLength) {
  pf.generateSafePath(HOME, GOAL, &maze);
  int lenBefore = static_cast<int>(strlen(pf.path()));
  pf.reversePath();
  int lenAfter = static_cast<int>(strlen(pf.path()));
  EXPECT_EQ(lenBefore, lenAfter);
}

TEST_F(TEST_10_PathFinder, 21_ReversePathTwiceRestoresOriginal) {
  pf.generateSafePath(HOME, GOAL, &maze);
  char original[MAX_PATH_LENGTH + 2];
  strncpy(original, pf.path(), sizeof(original));

  pf.reversePath();
  pf.reversePath();

  EXPECT_STREQ(original, pf.path());
}

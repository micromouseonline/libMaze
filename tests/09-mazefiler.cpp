// Round-trip tests for MazeFiler -- binary and text formats.

#include <cstdio>
#include <cstring>

#include "maze.h"
#include "mazefiler.h"

#include "gtest/gtest.h"

static constexpr uint16_t WIDTH = 16;
static constexpr uint16_t CELL_COUNT = WIDTH * WIDTH;

class TEST_09_MazeFiler : public ::testing::Test {
 protected:
  MazeFiler filer;
  Maze src{WIDTH};
  Maze dst{WIDTH};

  // Temp file paths written once per suite and cleaned up in TearDown.
  char binaryPath[64] = "/tmp/maze_test_09_rt.maz";
  char textPath[64] = "/tmp/maze_test_09_rt.txt";

  void SetUp() override {
    src.resetToEmptyMaze();
    dst.resetToEmptyMaze();
    // Set some interior walls to give non-trivial data
    src.setWall(0x55, NORTH);
    src.setWall(0x33, EAST);
    src.setWall(0x12, WEST);
  }

  void TearDown() override {
    std::remove(binaryPath);
    std::remove(textPath);
  }
};

// ---------------------------------------------------------------------------
// Binary round-trip
// ---------------------------------------------------------------------------

TEST_F(TEST_09_MazeFiler, 00_WriteBinarySucceeds) {
  int result = filer.writeBinaryMaze(&src, binaryPath);
  EXPECT_EQ(MazeFiler::MAZE_SUCCESS, result);
}

TEST_F(TEST_09_MazeFiler, 01_BinaryRoundTripPreservesWalls) {
  ASSERT_EQ(MazeFiler::MAZE_SUCCESS, filer.writeBinaryMaze(&src, binaryPath));

  // readMaze dispatches on content; binary starts with 0x0e
  int result = filer.readMaze(&dst, binaryPath);
  EXPECT_EQ(MazeFiler::MAZE_SUCCESS, result);

  // Wall at 0x55 north
  EXPECT_FALSE(dst.hasExit(0x55, NORTH));
  EXPECT_FALSE(dst.hasExit(0x33, EAST));
  EXPECT_FALSE(dst.hasExit(0x12, WEST));
  // Propagated neighbour walls
  EXPECT_FALSE(dst.hasExit(dst.cellNorth(0x55), SOUTH));
  EXPECT_FALSE(dst.hasExit(dst.cellEast(0x33), WEST));
}

TEST_F(TEST_09_MazeFiler, 02_BinaryRoundTripPreservesOpenWalls) {
  ASSERT_EQ(MazeFiler::MAZE_SUCCESS, filer.writeBinaryMaze(&src, binaryPath));
  ASSERT_EQ(MazeFiler::MAZE_SUCCESS, filer.readMaze(&dst, binaryPath));

  // Interior cell with no explicit wall set should remain open
  EXPECT_TRUE(dst.hasExit(0x55, EAST));
}

// ---------------------------------------------------------------------------
// Text round-trip
// ---------------------------------------------------------------------------

TEST_F(TEST_09_MazeFiler, 10_WriteTextSucceeds) {
  int result = filer.writeTextMaze(&src, textPath);
  EXPECT_EQ(MazeFiler::MAZE_SUCCESS, result);
}

TEST_F(TEST_09_MazeFiler, 11_TextRoundTripPreservesBoundaryWalls) {
  ASSERT_EQ(MazeFiler::MAZE_SUCCESS, filer.writeTextMaze(&src, textPath));

  int result = filer.readMaze(&dst, textPath);
  EXPECT_EQ(MazeFiler::MAZE_SUCCESS, result);

  // Boundary walls must be present after reload
  EXPECT_FALSE(dst.hasExit(0, SOUTH));
  EXPECT_FALSE(dst.hasExit(0, WEST));
}

// ---------------------------------------------------------------------------
// ReadMaze on non-existent file
// ---------------------------------------------------------------------------

TEST_F(TEST_09_MazeFiler, 20_ReadNonExistentFileReturnsError) {
  char bad[] = "/tmp/no_such_maze_09.maz";
  std::remove(bad);
  int result = filer.readMaze(&dst, bad);
  EXPECT_EQ(MazeFiler::MAZE_READ_ERROR, result);
}

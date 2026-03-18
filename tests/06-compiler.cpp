// Tests for compiler.h -- makeInPlaceCommands, makeDiagonalCommands, makeSmoothCommands

#include "commandnames.h"
#include "compiler.h"
#include "path-test-data.h"

#include "gtest/gtest.h"

static constexpr uint16_t BUF = 64;

// ---------------------------------------------------------------------------
// makeInPlaceCommands  (no tabular test data; hand-coded cases)
// ---------------------------------------------------------------------------

TEST(TEST_06_Compiler, 00_InPlace_StraightStop) {
  // "BFS" -> FWD1, CMD_STOP
  uint8_t cmd[BUF] = {};
  makeInPlaceCommands("BFS", BUF, cmd);
  EXPECT_EQ(FWD1, cmd[0]);
  EXPECT_EQ(CMD_STOP, cmd[1]);
}

TEST(TEST_06_Compiler, 01_InPlace_TwoForwardStop) {
  // "BFFS" -> FWD2, CMD_STOP
  uint8_t cmd[BUF] = {};
  makeInPlaceCommands("BFFS", BUF, cmd);
  EXPECT_EQ(FWD2, cmd[0]);
  EXPECT_EQ(CMD_STOP, cmd[1]);
}

TEST(TEST_06_Compiler, 02_InPlace_RightTurn) {
  // "BFRFS": cmd resets to FWD1 after turn, next 'F' increments to FWD2
  // -> FWD1, IP90R, FWD2, CMD_STOP
  uint8_t cmd[BUF] = {};
  makeInPlaceCommands("BFRFS", BUF, cmd);
  EXPECT_EQ(FWD1, cmd[0]);
  EXPECT_EQ(IP90R, cmd[1]);
  EXPECT_EQ(FWD2, cmd[2]);
  EXPECT_EQ(CMD_STOP, cmd[3]);
}

TEST(TEST_06_Compiler, 03_InPlace_LeftTurn) {
  // "BFLFS": cmd resets to FWD1 after turn, next 'F' increments to FWD2
  // -> FWD1, IP90L, FWD2, CMD_STOP
  uint8_t cmd[BUF] = {};
  makeInPlaceCommands("BFLFS", BUF, cmd);
  EXPECT_EQ(FWD1, cmd[0]);
  EXPECT_EQ(IP90L, cmd[1]);
  EXPECT_EQ(FWD2, cmd[2]);
  EXPECT_EQ(CMD_STOP, cmd[3]);
}

TEST(TEST_06_Compiler, 04_InPlace_ExploreTerminator) {
  // "BFX" -> FWD1, CMD_EXPLORE, CMD_STOP
  uint8_t cmd[BUF] = {};
  makeInPlaceCommands("BFX", BUF, cmd);
  EXPECT_EQ(FWD1, cmd[0]);
  EXPECT_EQ(CMD_EXPLORE, cmd[1]);
  EXPECT_EQ(CMD_STOP, cmd[2]);
}

TEST(TEST_06_Compiler, 05_InPlace_NoErrorOnValidPath) {
  uint8_t cmd[BUF] = {};
  makeInPlaceCommands("BFFRFLFFS", BUF, cmd);
  // Walk through commands -- none should be errors
  for (int i = 0; i < BUF; i++) {
    if (cmd[i] == CMD_STOP) {
      break;
    }
    EXPECT_FALSE(isErr(cmd[i])) << "Error command at position " << i;
  }
}

// ---------------------------------------------------------------------------
// Comparison helper
//
// The test data was written for a compiler that emits CMD_BEGIN (0x80) at the
// start and CMD_END (0x82) as the stop marker. The current compiler does
// neither: CMD_BEGIN emission is commented out, and PathStop uses CMD_STOP
// (0x00). The comparison normalises expected to match actual:
//
//   1. If expected[0] == CMD_BEGIN, skip it (offset = 1).
//   2. Treat CMD_END in expected as CMD_STOP.
//   3. Stop at the first (normalised) CMD_STOP in expected.
// ---------------------------------------------------------------------------

static void compareOutput(int pair_index, const char *input, const uint8_t *expected, const uint8_t *actual) {
  int offset = (expected[0] == CMD_BEGIN) ? 1 : 0;
  for (int j = offset; j < MAX_CMD_COUNT; j++) {
    uint8_t exp = expected[j];
    if (exp == CMD_END) {
      exp = CMD_STOP;
    }
    EXPECT_EQ(static_cast<int>(exp), static_cast<int>(actual[j - offset]))
        << "pair " << pair_index << " (\"" << input << "\") byte " << (j - offset);
    if (exp == CMD_STOP) {
      break;
    }
  }
}

// ---------------------------------------------------------------------------
// makeDiagonalCommands  (169 pairs from testPairs[])
// ---------------------------------------------------------------------------

TEST(TEST_06_Compiler, 10_Diagonal_AllPairs) {
  for (int i = 0; i < diagonalPairCount; i++) {
    uint8_t actual[MAX_CMD_COUNT] = {};
    makeDiagonalCommands(testPairs[i].input, MAX_CMD_COUNT, actual);
    compareOutput(i, testPairs[i].input, testPairs[i].expected, actual);
  }
}

// ---------------------------------------------------------------------------
// makeSmoothCommands  (19 pairs from smoothTestPairs[])
// ---------------------------------------------------------------------------

TEST(TEST_06_Compiler, 20_Smooth_AllPairs) {
  for (int i = 0; i < smoothPairCount; i++) {
    uint8_t actual[MAX_CMD_COUNT] = {};
    makeSmoothCommands(smoothTestPairs[i].input, MAX_CMD_COUNT, actual);
    compareOutput(i, smoothTestPairs[i].input, smoothTestPairs[i].expected, actual);
  }
}

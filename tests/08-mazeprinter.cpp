// Smoke tests for MazePrinter -- verify no crash; content not compared.
//
// Set suppress_output = false in the fixture to see labelled output.

#include <sstream>
#include <string>

#include "maze.h"
#include "mazeprinter.h"

#include "gtest/gtest.h"

static constexpr uint16_t WIDTH = 16;
static constexpr uint16_t GOAL = 0x77;

class TEST_08_MazePrinter : public ::testing::Test {
 protected:
  Maze maze{WIDTH};

  /// Set to false to print each test's output labelled with the test name.
  bool suppress_output = true;

  void SetUp() override {
    maze.resetToEmptyMaze();
    maze.manhattanFlood(GOAL);
  }

  /*** Capture stdout from func, then emit each line prefixed with the
   *   current test name, unless suppress_output is true. */
  template <typename Func>
  void runAndLabel(Func func) {
    ::testing::internal::CaptureStdout();
    func();
    std::string captured = ::testing::internal::GetCapturedStdout();
    if (suppress_output) {
      return;
    }
    const char *test_name = ::testing::UnitTest::GetInstance()->current_test_info()->name();
    std::istringstream ss(captured);
    std::string line;
    while (std::getline(ss, line)) {
      printf("[%s] %s\n", test_name, line.c_str());
    }
  }
};

TEST_F(TEST_08_MazePrinter, 00_PrintPlainNoCrash) {
  ASSERT_NO_FATAL_FAILURE(runAndLabel([this] { MazePrinter::printPlain(&maze); }));
}

TEST_F(TEST_08_MazePrinter, 01_PrintCostsNoCrash) {
  ASSERT_NO_FATAL_FAILURE(runAndLabel([this] { MazePrinter::printCosts(&maze); }));
}

TEST_F(TEST_08_MazePrinter, 02_PrintDirsNoCrash) {
  ASSERT_NO_FATAL_FAILURE(runAndLabel([this] { MazePrinter::printDirs(&maze); }));
}

TEST_F(TEST_08_MazePrinter, 03_PrintVisitedDirsNoCrash) {
  ASSERT_NO_FATAL_FAILURE(runAndLabel([this] { MazePrinter::printVisitedDirs(&maze); }));
}

TEST_F(TEST_08_MazePrinter, 04_PrintCDecl_NoCrash) {
  ASSERT_NO_FATAL_FAILURE(runAndLabel([this] { MazePrinter::printCDecl(&maze, "test_maze"); }));
}

TEST_F(TEST_08_MazePrinter, 05_PrintRawDecl_NoCrash) {
  ASSERT_NO_FATAL_FAILURE(runAndLabel([this] { MazePrinter::printRawDecl(&maze, "test_maze"); }));
}

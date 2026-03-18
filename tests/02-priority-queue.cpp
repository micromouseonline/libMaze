
// Tests for PriorityQueue<T>
//
// These tests define expected behaviour.
//
// Known issue not tested here:
//   The copy constructor allocates a single item instead of an array:
//     mData = new item_t(MAX_ITEMS + 1)   should be new item_t[MAX_ITEMS + 1]
//   Invoking the copy constructor causes heap corruption; it is not exercised here.
//
// Tests use small explicit capacities to keep runtime predictable and avoid
// hitting the default capacity limit (128) during flood-style usage patterns.

#include "floodinfo.h"
#include "priorityqueue.h"

#include "gtest/gtest.h"

class TEST_02_PriorityQueue : public ::testing::Test {
 protected:
  void SetUp() override {}
  void TearDown() override {}
};

// ---------------------------------------------------------------------------
// Construction and empty-state
// ---------------------------------------------------------------------------

TEST_F(TEST_02_PriorityQueue, 00_DefaultConstructionIsEmpty) {
  PriorityQueue<int> q;
  EXPECT_EQ(0, q.size());
}

TEST_F(TEST_02_PriorityQueue, 01_CustomCapacityConstructionIsEmpty) {
  PriorityQueue<int> q(16);
  EXPECT_EQ(0, q.size());
}

// ---------------------------------------------------------------------------
// add() / size()
// ---------------------------------------------------------------------------

TEST_F(TEST_02_PriorityQueue, 10_AddSingleItemIncreasesSize) {
  PriorityQueue<int> q(8);
  q.add(42);
  EXPECT_EQ(1, q.size());
}

TEST_F(TEST_02_PriorityQueue, 11_AddMultipleItemsAccumulatesSize) {
  PriorityQueue<int> q(8);
  q.add(1);
  q.add(2);
  q.add(3);
  EXPECT_EQ(3, q.size());
}

TEST_F(TEST_02_PriorityQueue, 12_AddToCapacityDoesNotAssert) {
  const int cap = 8;
  PriorityQueue<int> q(cap);
  for (int i = 0; i < cap; ++i) {
    q.add(i);
  }
  EXPECT_EQ(cap, q.size());
}

// ---------------------------------------------------------------------------
// clear()
// ---------------------------------------------------------------------------

TEST_F(TEST_02_PriorityQueue, 20_ClearEmptiesQueue) {
  PriorityQueue<int> q(8);
  q.add(1);
  q.add(2);
  q.clear();
  EXPECT_EQ(0, q.size());
}

TEST_F(TEST_02_PriorityQueue, 21_AddAfterClearWorks) {
  PriorityQueue<int> q(8);
  q.add(10);
  q.add(20);
  q.clear();
  q.add(99);
  EXPECT_EQ(1, q.size());
  EXPECT_EQ(99, q.head());
}

// ---------------------------------------------------------------------------
// head() -- FIFO behaviour (no priority ordering)
// ---------------------------------------------------------------------------

TEST_F(TEST_02_PriorityQueue, 30_HeadReturnsFIFOOrder) {
  PriorityQueue<int> q(8);
  q.add(10);
  q.add(20);
  q.add(30);
  EXPECT_EQ(10, q.head());
  EXPECT_EQ(20, q.head());
  EXPECT_EQ(30, q.head());
}

TEST_F(TEST_02_PriorityQueue, 31_HeadDecreasesSize) {
  PriorityQueue<int> q(8);
  q.add(1);
  q.add(2);
  EXPECT_EQ(2, q.size());
  q.head();
  EXPECT_EQ(1, q.size());
  q.head();
  EXPECT_EQ(0, q.size());
}

TEST_F(TEST_02_PriorityQueue, 32_HeadDoesNotPreserveOrderAfterFetchSmallest) {
  // After fetchSmallest swaps items, head() no longer reflects original
  // insertion order. This is documented behaviour: fetchSmallest corrupts FIFO.
  PriorityQueue<int> q(8);
  q.add(30);
  q.add(10);
  q.add(20);
  int smallest = q.fetchSmallest();  // removes 10, corrupts order
  EXPECT_EQ(10, smallest);
  EXPECT_EQ(2, q.size());
  // remaining two items are accessible but order is implementation-defined
  int a = q.head();
  int b = q.head();
  EXPECT_EQ(0, q.size());
  // both values must be from the original set (not 10)
  bool a_valid = (a == 20 || a == 30);
  bool b_valid = (b == 20 || b == 30);
  EXPECT_TRUE(a_valid);
  EXPECT_TRUE(b_valid);
  EXPECT_NE(a, b);
}

// ---------------------------------------------------------------------------
// fetchSmallest() -- priority behaviour
// ---------------------------------------------------------------------------

TEST_F(TEST_02_PriorityQueue, 40_FetchSmallestSingleItem) {
  PriorityQueue<int> q(8);
  q.add(42);
  EXPECT_EQ(42, q.fetchSmallest());
  EXPECT_EQ(0, q.size());
}

TEST_F(TEST_02_PriorityQueue, 41_FetchSmallestReturnsMinimum) {
  PriorityQueue<int> q(8);
  q.add(30);
  q.add(10);
  q.add(20);
  EXPECT_EQ(10, q.fetchSmallest());
}

TEST_F(TEST_02_PriorityQueue, 42_FetchSmallestDecreasesSize) {
  PriorityQueue<int> q(8);
  q.add(5);
  q.add(3);
  EXPECT_EQ(2, q.size());
  q.fetchSmallest();
  EXPECT_EQ(1, q.size());
}

TEST_F(TEST_02_PriorityQueue, 43_FetchSmallestRepeatedlyGivesAscendingOrder) {
  PriorityQueue<int> q(16);
  q.add(50);
  q.add(10);
  q.add(40);
  q.add(20);
  q.add(30);
  EXPECT_EQ(10, q.fetchSmallest());
  EXPECT_EQ(20, q.fetchSmallest());
  EXPECT_EQ(30, q.fetchSmallest());
  EXPECT_EQ(40, q.fetchSmallest());
  EXPECT_EQ(50, q.fetchSmallest());
  EXPECT_EQ(0, q.size());
}

TEST_F(TEST_02_PriorityQueue, 44_FetchSmallestNegativeValues) {
  PriorityQueue<int> q(8);
  q.add(-5);
  q.add(0);
  q.add(-10);
  q.add(3);
  EXPECT_EQ(-10, q.fetchSmallest());
  EXPECT_EQ(-5, q.fetchSmallest());
  EXPECT_EQ(0, q.fetchSmallest());
  EXPECT_EQ(3, q.fetchSmallest());
}

TEST_F(TEST_02_PriorityQueue, 45_FetchSmallestAllEqualReturnsFirst) {
  // When all items are equal, the first-added should be returned.
  PriorityQueue<int> q(8);
  q.add(7);
  q.add(7);
  q.add(7);
  EXPECT_EQ(7, q.fetchSmallest());
  EXPECT_EQ(2, q.size());
}

TEST_F(TEST_02_PriorityQueue, 46_FetchSmallestWithTiesPreservesRelativeOrder) {
  // Two items with the same minimum value: the one added first should be returned first.
  PriorityQueue<int> q(8);
  q.add(5);   // index 0 - first minimum
  q.add(5);   // index 1 - second minimum
  q.add(10);
  int first = q.fetchSmallest();
  EXPECT_EQ(5, first);
  int second = q.fetchSmallest();
  EXPECT_EQ(5, second);
  int third = q.fetchSmallest();
  EXPECT_EQ(10, third);
}

// ---------------------------------------------------------------------------
// uint16_t -- as used in manhattanFlood
// ---------------------------------------------------------------------------

TEST_F(TEST_02_PriorityQueue, 50_Uint16AddAndHead) {
  PriorityQueue<uint16_t> q(8);
  q.add(uint16_t(100));
  q.add(uint16_t(200));
  EXPECT_EQ(uint16_t(100), q.head());
  EXPECT_EQ(uint16_t(200), q.head());
}

TEST_F(TEST_02_PriorityQueue, 51_Uint16FetchSmallest) {
  PriorityQueue<uint16_t> q(8);
  q.add(uint16_t(300));
  q.add(uint16_t(100));
  q.add(uint16_t(200));
  EXPECT_EQ(uint16_t(100), q.fetchSmallest());
  EXPECT_EQ(uint16_t(200), q.fetchSmallest());
  EXPECT_EQ(uint16_t(300), q.fetchSmallest());
}

// ---------------------------------------------------------------------------
// FloodInfo -- as used in runLengthFlood
// ---------------------------------------------------------------------------

TEST_F(TEST_02_PriorityQueue, 60_FloodInfoAddAndSize) {
  PriorityQueue<FloodInfo> q(8);
  q.add(FloodInfo(10, 1, 1, 0, 0));
  q.add(FloodInfo(20, 2, 1, 0, 0));
  EXPECT_EQ(2, q.size());
}

TEST_F(TEST_02_PriorityQueue, 61_FloodInfoFetchSmallestByCost) {
  PriorityQueue<FloodInfo> q(8);
  q.add(FloodInfo(30, 3, 1, 0, 0));
  q.add(FloodInfo(10, 1, 1, 0, 0));
  q.add(FloodInfo(20, 2, 1, 0, 0));
  FloodInfo f = q.fetchSmallest();
  EXPECT_EQ(10, f.cost);
  EXPECT_EQ(1, f.cell);
}

TEST_F(TEST_02_PriorityQueue, 62_FloodInfoSortedByCostAscending) {
  PriorityQueue<FloodInfo> q(16);
  q.add(FloodInfo(50, 5, 1, 0, 0));
  q.add(FloodInfo(10, 1, 1, 0, 0));
  q.add(FloodInfo(40, 4, 1, 0, 0));
  q.add(FloodInfo(20, 2, 1, 0, 0));
  q.add(FloodInfo(30, 3, 1, 0, 0));
  EXPECT_EQ(10, q.fetchSmallest().cost);
  EXPECT_EQ(20, q.fetchSmallest().cost);
  EXPECT_EQ(30, q.fetchSmallest().cost);
  EXPECT_EQ(40, q.fetchSmallest().cost);
  EXPECT_EQ(50, q.fetchSmallest().cost);
}

TEST_F(TEST_02_PriorityQueue, 63_FloodInfoTiedCostReturnsFirstAdded) {
  PriorityQueue<FloodInfo> q(8);
  q.add(FloodInfo(10, 1, 1, 0, 0));  // cell 1 - first with cost 10
  q.add(FloodInfo(10, 2, 1, 0, 0));  // cell 2 - second with cost 10
  q.add(FloodInfo(20, 3, 1, 0, 0));
  FloodInfo first = q.fetchSmallest();
  EXPECT_EQ(10, first.cost);
  EXPECT_EQ(1, first.cell);  // cell 1 was added first
  FloodInfo second = q.fetchSmallest();
  EXPECT_EQ(10, second.cost);
  EXPECT_EQ(2, second.cell);
}

TEST_F(TEST_02_PriorityQueue, 64_FloodInfoClearAndReuse) {
  PriorityQueue<FloodInfo> q(8);
  q.add(FloodInfo(5, 0, 1, 0, 0));
  q.add(FloodInfo(3, 1, 1, 0, 0));
  q.clear();
  EXPECT_EQ(0, q.size());
  q.add(FloodInfo(99, 7, 2, 1, 2));
  EXPECT_EQ(1, q.size());
  FloodInfo f = q.fetchSmallest();
  EXPECT_EQ(99, f.cost);
  EXPECT_EQ(7, f.cell);
}

// ---------------------------------------------------------------------------
// Wrap-around: fill, drain, refill to exercise circular buffer
// ---------------------------------------------------------------------------

TEST_F(TEST_02_PriorityQueue, 70_CircularBufferWrapAround) {
  const int cap = 8;
  PriorityQueue<int> q(cap);
  // fill half
  for (int i = 0; i < 4; ++i) {
    q.add(i * 10);
  }
  // drain half via head() to advance mHead
  q.head();
  q.head();
  EXPECT_EQ(2, q.size());
  // refill past the original tail position to exercise wrap
  for (int i = 0; i < 4; ++i) {
    q.add(i * 5);
  }
  EXPECT_EQ(6, q.size());
  // fetchSmallest should still return the global minimum
  EXPECT_EQ(0, q.fetchSmallest());
}

TEST_F(TEST_02_PriorityQueue, 71_CircularBufferFetchSmallestAfterWrap) {
  // Exercises the full-buffer case where mHead == mTail after wrap-around.
  // fetchSmallest() must scan all items, not stop immediately at mTail.
  const int cap = 8;
  PriorityQueue<int> q(cap);
  for (int i = 7; i >= 0; --i) {
    q.add(i);  // adds 7,6,5,4,3,2,1,0
  }
  // drain three to advance mHead, then refill to make the buffer exactly full
  // with mHead == mTail, triggering the scan-loop bug
  q.head();
  q.head();
  q.head();
  EXPECT_EQ(5, q.size());
  q.add(100);
  q.add(50);
  q.add(75);
  EXPECT_EQ(8, q.size());
  // remaining items: {4,3,2,1,0,100,50,75} -- minimum is 0
  EXPECT_EQ(0, q.fetchSmallest());
}

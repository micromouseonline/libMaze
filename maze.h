/************************************************************************
 *
 * Copyright (C) 2017 by Peter Harrison. www.micromouseonline.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without l> imitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************/

#ifndef _maze_h
#define _maze_h

#include <algorithm>
#include <cstdint>
#include <list>
#include <vector>
#include "floodinfo.h"
#include "mazeconstants.h"
#include "priorityqueue.h"

typedef std::list<int> GoalArea_t;

/// TODO: is the closed maze needed? is it enough to see if the path has unvisited cells?

using namespace std;
class Maze {
 public:
  explicit Maze(uint16_t width);
  enum FloodType { MANHATTAN_FLOOD, WEIGHTED_FLOOD, RUNLENGTH_FLOOD, DIRECTION_FLOOD };

  /// the maze is assumed to be square
  uint16_t width() const;  ///
  uint16_t numCells();     ///

  ///  reset the wall, cost and direction data to defaults
  void clearData();
  /// clear the data and then set all the walls that exist in an empty maze
  void resetToEmptyMaze();  ///

  /// Clear the costs and directions and then copy the walls from an array
  void copyMazeFromFileData(const uint8_t *wallData, uint16_t cellCount);

  /// return the column number of  given cell
  inline uint16_t col(uint16_t cell) { return cell / mWidth; }
  /// return the roww number of a given cell
  inline uint16_t row(uint16_t cell) { return cell % mWidth; }

  inline uint16_t cellID(int x, int y) const { return mWidth * x + y; }

  /// return the address of the cell ahead from this cardinal direction
  static uint8_t ahead(uint8_t direction);
  /// return the address of the cell on the right of this cardinal direction
  static uint8_t rightOf(uint8_t direction);
  /// return the address of the cell on the left of this cardinal direction
  static uint8_t leftOf(uint8_t direction);
  /// return the address of the cell behind this cardinal direction
  static uint8_t behind(uint8_t direction);
  /// return the address of the cell opposite the given wall
  static uint8_t opposite(uint8_t direction);
  static uint8_t differenceBetween(uint8_t oldDirection, uint8_t newDirection);

  // static functions about neighbours
  /// return the address of the cell in the indicated direction
  uint16_t cellNorth(uint16_t cell);
  /// return the address of the cell in the indicated direction
  uint16_t cellEast(uint16_t cell);
  /// return the address of the cell in the indicated direction
  uint16_t cellSouth(uint16_t cell);
  /// return the address of the cell in the indicated direction
  uint16_t cellWest(uint16_t cell);

  /// return the address of the cell in the given direction
  uint16_t neighbour(uint16_t cell, uint16_t direction);

  /// return the address of the home cell. Nearly always cell zero
  uint16_t home();
  /// return the cell address of the current goal
  uint16_t goal() const;
  ///  set the current goal to a new value
  void setGoal(uint16_t goal);
  GoalArea_t getGoalArea() const;
  void setGoalArea(GoalArea_t &goalArea);

  /// return the state of the four walls surrounding a given cell
  uint8_t walls(uint16_t cell) const;
  uint8_t openWalls(uint16_t cell);
  uint8_t closedWalls(uint16_t cell);

  bool hasOpenExit(uint16_t cell, uint8_t direction) const;
  bool hasClosedExit(uint16_t cell, uint8_t direction) const;
  bool hasExit(uint16_t cell, uint8_t direction, uint8_t mazeType) const;

  /// return the stored direction for the given cell
  uint8_t direction(uint16_t cell);
  /// set the direction for the given cell
  void setDirection(uint16_t cell, uint8_t direction);

  /// test to see if  all the walls of a given cell have been seen
  bool isVisited(uint16_t cell);

  /// NOT TO BE USED IN SEARCH. Unconditionally set a  wall in a cell and mark as seen.
  void setWallPresent(uint16_t cell, uint8_t direction);
  /// NOT TO BE USED IN SEARCH. Unconditionally clear a  wall in a cell and mark as seen.
  void setWallAbsent(uint16_t cell, uint8_t direction);

  /// USE THIS FOR SEARCH. Update a single cell with wall data (normalised for direction)
  void updateMap(uint16_t cell, uint8_t wallData);

  /// return the cost value for a given cell. Used in flooding and searching
  uint16_t cost(uint16_t cell);
  /// return the cost in the neighbouring cell in the given direction
  uint16_t cost(uint16_t cell, uint16_t direction);
  /// return the cost in the neighbouring cell to the North
  uint16_t costNorth(uint16_t cell);
  /// return the cost in the neighbouring cell to the East
  uint16_t costEast(uint16_t cell);
  /// return the cost in the neighbouring cell to the South
  uint16_t costSouth(uint16_t cell);
  /// return the cost in the neighbouring cell to the West
  uint16_t costWest(uint16_t cell);

  /// set the cost in the given cell.
  void setCost(uint16_t cell, uint16_t cost);  ///

  /// examine the goal area and move the goal if needed for a better entry speed
  void recalculateGoal();

  /// return the cost of the current best path assuming unknowns are absent
  uint16_t openMazeCost() const;
  /// return the cost of the current best path assuming unknowns are present
  uint16_t closedMazeCost() const;
  /// return the difference between the open and closed cost. Zero when the best route is found.
  int32_t costDifference();
  /// flood the maze for the give goal
  uint16_t flood(uint16_t target, uint8_t maze_type);
  /// RunLengthFlood is a specific kind of flood used in this mouse
  uint16_t runLengthFlood(uint16_t target, uint8_t maze_type);
  /// manhattanFlood is a the simplest kind of flood used in this mouse
  uint16_t manhattanFlood(uint16_t target, uint8_t maze_type);
  /// weightedFlood assigns a penalty to turns vs straights
  uint16_t weightedFlood(uint16_t target, uint8_t maze_type);
  /// directionFlood does not care about costs, only using direction pointers
  uint16_t directionFlood(uint16_t target, uint8_t maze_type);

  /// Flood the maze both open and closed and then test the cost difference
  /// leaves the maze with unknowns clear
  bool testForSolution();
  /// returns the result of the most recent test for a solution
  bool isSolved();

  ///  return the direction from the given cell to the least costly neighbour
  uint8_t directionToSmallest(uint16_t cell);
  /// for every cell in the maze, calculate and store the least costly direction
  void updateDirections();

  /// save the wall data, including visited flags in the target array. Not checked for overflow.
  void save(uint8_t *data);

  /// load the wall data, including visited flags from the target array. Not checked for overflow.
  void load(const uint8_t *data);

  /// set the Flood Type to use
  void setFloodType(FloodType mFloodType);
  FloodType getFloodType() const;
  /// used only for the weighted Flood
  uint16_t getCornerWeight() const;
  void setCornerWeight(uint16_t cornerWeight);

  void setWidth(uint16_t mWidth);
  void clearGoalArea();
  void addToGoalArea(int cell);
  void addToGoalArea(int x, int y);
  void removeFromGoalArea(int cell);
  void removeFromGoalArea(int x, int y);

  bool goalContains(int cell) const;
  bool goalContains(int x, int y) const;
  int goalAreaSize() const;

 protected:
  /// stores the wall and visited flags. Allows for 32x32 maze but wastes space
  uint8_t m_walls[1024] = {0xf0};
  /// the width of the maze in cells. Assume mazes are always square
  uint16_t mWidth = 32;
  /// stores the least costly direction. Allows for 32x32 maze but wastes space
  uint8_t mDirection[1024] = {NORTH};
  /// stores the cost information from a flood. Allows for 32x32 maze but wastes space
  uint16_t mCost[1024] = {MAX_COST};
  /// The goal is an area so a list of locations is needed. Must have one or more entries
  GoalArea_t goalArea;
  /// The cost of the best path assuming unseen walls are absent
  uint16_t mPathCostOpen = MAX_COST;
  /// The cost of the best path assuming unseen walls are present
  uint16_t mPathCostClosed = MAX_COST;
  /// flag set when maze has been solved
  bool mIsSolved = false;
  /// Remember which type of flood is to be used
  FloodType mFloodType = RUNLENGTH_FLOOD;
  /// the weighted flood needs a cost for corners
  uint16_t mCornerWeight = 3;
  Maze() = default;
  /// used to set up the queue before running the more complex floods
  void seedQueue(PriorityQueue<FloodInfo> &queue, uint16_t goal, uint16_t cost);
  /// set all the cell costs to their maximum value, except the target
  void initialiseFloodCosts(uint16_t target);
  /// NOT TO BE USED IN SEARCH. Update a single cell from stored map data.
  void copyCellFromFileData(uint16_t cell, uint8_t wallData);
};

#endif

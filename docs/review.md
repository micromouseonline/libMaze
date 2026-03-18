# libMaze Code Review

*Date: 2026-03-09*

## Overview

`libMaze` handles maze representation, flood-fill pathfinding, path generation, search strategy, file I/O, and ASCII
visualisation. It spans ~13 source files and compiles cleanly as a standalone host-side library. It is the most
self-contained and unit-testable part of the codebase.

---

## Strengths

**1. Memory-efficient wall encoding.**
Each cell stores walls and visited flags in a single `uint8_t`: bits 0-3 are wall-present flags (one per direction),
bits 4-7 are the corresponding wall-unseen flags. This fits a 16x16 maze into 256 bytes. The `OPEN_MASK`/`CLOSED_MASK`
trick cleanly switches between "treat unknowns as absent" and "treat unknowns as present" without duplicating data.

**2. Bidirectional wall consistency.**
`setWall()` and `clearWall()` always update both sides of a wall atomically (the cell and its neighbour), making it
impossible to have a wall that is present from one side and absent from the other. `updateMap()` correctly skips
already-seen walls during exploration, preventing overwriting valid data.

**3. Well-motivated cost tables in `runLengthFlood`.**
`orthoCostTable[]` and `diagCostTable[]` are derived from kinematics, not guessed. Run-length encoding of straights
accurately models the physics of acceleration: longer straights cost less per cell. This is the right design for a
speed-optimised mouse.

**4. Multiple flood strategies behind a common interface.**
`flood()` dispatches to Manhattan, weighted, run-length, or direction flood via a single enum, with consistent
cost-reset and direction-update behaviour. Callers do not need to know which algorithm is active.

**5. Clean separation of responsibilities.**
`Maze`, `MazeSearcher`, `PathFinder`, `MazeFiler`, and `MazePrinter` each have a clear, single purpose. `MazeSearcher`
maintains its own internal map separate from the reference maze, correctly modelling the real-world distinction between
what the mouse knows and what is true.

**6. `direction.h` is modern, well-designed C++.**
`Direction` is a 1-byte type in its own namespace with `constexpr` lookup tables, implicit `int` cast, proper
orthogonal/diagonal discrimination via a single bit test, and a clean relative/absolute direction model. The
`newDirectionLookup[8][8]` table is readable and trivially verifiable by inspection.

**7. Rich test maze corpus.**
`mazedata.cpp` provides 50+ historical competition mazes. This is invaluable for regression testing flood algorithms
against known results.

---

## Problems and Risks

### Critical

**1. `PriorityQueue` copy constructor allocates a single object, not an array.**
`priorityqueue.h` line 47:
```cpp
mData = new item_t(MAX_ITEMS + 1);   // BUG: calls constructor with argument MAX_ITEMS+1
```
Should be `new item_t[MAX_ITEMS + 1]`. If the copy constructor is ever invoked, this allocates one item and subsequent
indexed accesses corrupt memory silently. The destructor calls `delete[]` on this pointer, which is undefined behaviour
regardless.

**2. Default `PriorityQueue` capacity too small for larger mazes.**
The default `maxSize` is 128. `runLengthFlood` can add up to 4 items per cell (one per open exit). A 32x32 maze has
1024 cells; worst-case queue depth greatly exceeds 128. The `assert` fires in debug builds (or is compiled out in
release), causing a silent overwrite. There is no graceful overflow path.

**3. `recalculateGoal()` is broken and contains a debug `printf`.**
`maze.cpp` line 552 calls `printf()` in a path executed during runs. Lines 556-594 are dead code after an
unconditional `return` on line 554. The live code path traces cells from home using `directionToSmallest()` without
guaranteeing the flood has been run beforehand. The function is unreliable and should be considered non-functional.

**4. Bounds check in `copyMazeFromFileData()` is commented out.**
`maze.cpp` lines 137-139:
```cpp
//  if (cellCount > numCells()) {
//    return; // ERROR here
//  }
```
Oversized input silently writes past `xWalls[]` and corrupts `mDirection[]` and `mCost[]`.

### Significant

**5. Three incompatible direction systems coexist.**
- `mazeconstants.h`: `#define NORTH 0 / EAST 1 / SOUTH 2 / WEST 3` (4-direction macros, used throughout `Maze`)
- `mazeconstants.h`: `enum { DIR_N=0, DIR_NE=1, DIR_E=2, ... DIR_NW=7 }` (8-direction enum, used in flood seeds)
- `direction.h`: `MazeLib::Absolute { North=0, NorthEast=1, ... }` (8-direction class, used only in `direction.h`)

`NORTH` (0) and `DIR_N` (0) accidentally agree; `EAST` (1) and `DIR_E` (2) do not. Code mixing these systems will
produce silent incorrect results. The risk grows as `direction.h` is integrated more widely.

**6. `using namespace std;` in `maze.h`.**
Line 39 of `maze.h` places `using namespace std;` at file scope in a header. Every translation unit that includes
`maze.h` silently imports the entire `std` namespace with no recourse for the includer.

**7. `neighbour()` returns `MAX_COST` (65535) as a cell index for invalid directions.**
If the result is used to index `xWalls[65535]`, that is 640 bytes past the end of the 1024-element array. Silent memory
corruption.

**8. `openWalls()`/`closedWalls()` mutate and restore shared state.**
Both methods save, modify, and restore `mOpenCloseMask` to compute a const-natured result. The pattern is not
thread-safe, cannot be called from a `const` context, and leaves the object in a bad state if anything interrupts
execution between the save and restore.

**9. `fetchSmallest()` is O(n) per call, making flood O(n^2).**
The class is named `PriorityQueue` but is a circular buffer with a full linear scan for the minimum. For 16x16 mazes
this is acceptable; for 32x32 or with run-length flooding adding many items per cell, the quadratic behaviour is
significant. A binary heap gives O(n log n) overall.

### Minor

**10. Dead code in `Direction::operator-`.**
`direction.h` lines 122-129 are unreachable after the `return` on line 117.

**11. `FloodInfo` comparison operators take arguments by value.**
`operator==`, `operator>`, `operator<` etc. should take `const FloodInfo&` to avoid copies inside `fetchSmallest()`.

**12. `std::list<int>` for `goalArea`.**
A doubly-linked list with per-element heap allocation for a collection that typically holds 1-4 elements, iterated in
flood loops. `std::vector` or a small fixed-size array is more appropriate for embedded use.

**13. Copy prevention uses C++03 private-declaration idiom.**
`MazeSearcher` declares the copy constructor and `operator=` as private. Use `= delete` instead for clearer intent and
better compiler diagnostics.

**14. Include guard style is inconsistent.**
`direction.h` and `floodinfo.h` use `#pragma once`. `maze.h`, `mazeconstants.h`, and others use `#ifndef` guards.

**15. `costNorth()` has a different structure from `costEast`/`costSouth`/`costWest`.**
`costNorth` uses an early return; the other three use if-else. Trivially inconsistent.

---

## Test Coverage Assessment

| Component | Coverage | Notes |
|---|---|---|
| `Direction` class | Good | Construction, arithmetic, conversion, wraparound, lookup table all tested |
| `Maze` walls | None | `setWall`, `clearWall`, `updateMap`, `hasExit` untested |
| `Maze` flood algorithms | None | Manhattan, weighted, run-length, direction floods untested |
| `Maze` solution testing | None | `testForSolution`, open/closed cost comparison untested |
| `MazeSearcher` | None | Search strategies and wall-following untested |
| `PathFinder` | None | Safe/unsafe path generation untested |
| `MazeFiler` | None | File I/O untested |
| `PriorityQueue` | None | Untested despite containing a known memory bug |

The test suite covers the newest, simplest component (`Direction`) and nothing else. The flood algorithms are the
most complex and most critical code in the library and have zero test coverage.

---

## Priority Areas for Improvement

| Priority | Issue | Risk if ignored |
|---|---|---|
| 1 | Fix `PriorityQueue` copy constructor (`new item_t` vs `new item_t[]`) | Memory corruption |
| 2 | Increase `PriorityQueue` capacity or add overflow detection | Assert-crash or silent overwrite during flood |
| 3 | Fix `recalculateGoal()` and remove `printf` | Wrong goal selection; debug output in production |
| 4 | Restore bounds check in `copyMazeFromFileData()` | Silent memory corruption on bad input |
| 5 | Unify the three direction systems | Latent correctness bugs as `direction.h` is adopted more widely |
| 6 | Add tests for `Maze` flood algorithms using the `mazedata` corpus | No regression safety for the most critical code |
| 7 | Remove `using namespace std;` from `maze.h` | Namespace pollution in all including translation units |
| 8 | Fix `neighbour()` invalid-direction return value | Out-of-bounds array access |
| 9 | Replace O(n) `fetchSmallest()` with a binary heap | Quadratic flood on larger mazes |
| 10 | Replace `std::list` for `goalArea` with `std::vector` or fixed array | Unnecessary heap allocation in embedded context |

/**
 *      Peter Harrison 2025
 */

#pragma once

#include "util.h"

#include <cstdint>

#include <array>
#include <vector>

namespace MazeLib {

enum Absolute {
  North = 0,      //
  NorthEast = 1,  //
  East = 2,       //
  SouthEast = 3,  //
  South = 4,      //
  SouthWest = 5,  //
  West = 6,       //
  NorthWest = 7,  //
  DIR_COUNT = 8,  //
};

enum Relative {
  Front,     //
  Right45,   //
  Right,     //
  Right135,  //
  Back,      //
  Left135,   //
  Left,      //
  Left45,    //
};

class Direction {
 public:
  static constexpr int cardinals[] = {
      North,  //
      East,   //
      South,  //
      West    //
  };

  static constexpr int ordinals[] = {
      NorthEast,
      SouthEast,
      SouthWest,
      NorthWest,
  };

  static constexpr int AllDirections[] = {
      North,      //
      NorthEast,  //
      East,       //
      SouthEast,  //
      South,      //
      SouthWest,  //
      West,       //
      NorthWest,  //
  };

  static constexpr int LookDirections[] = {
      Front,     //
      Right45,   //
      Left45,    //
      Right,     //
      Left,      //
      Right135,  //
      Left135,   //
      Back,      //
  };

  // clang-format off
    static constexpr Absolute newDirectionLookup[DIR_COUNT][DIR_COUNT] = {
         /* Front    */ {North,     NorthEast, East,      SouthEast, South,     SouthWest, West,      NorthWest},
         /* Right45  */ {NorthEast, East,      SouthEast, South,     SouthWest, West,      NorthWest, North},
         /* Right    */ {East,      SouthEast, South,     SouthWest, West,      NorthWest, North,     NorthEast},
         /* Right135 */ {SouthEast, South,     SouthWest, West,      NorthWest, North,     NorthEast, East},
         /* Back     */ {South,     SouthWest, West,      NorthWest, North,     NorthEast, East,      SouthEast},
         /* Left135  */ {SouthWest, West,      NorthWest, North,     NorthEast, East,      SouthEast, South},
         /* Left     */ {West,      NorthWest, North,     NorthEast, East,      SouthEast, South,     SouthWest},
         /* Left45   */ {NorthWest, North,     NorthEast, East,      SouthEast, South,     SouthWest, West}
    };
  // clang-format on

 public:
  constexpr explicit Direction(const Absolute d = North) : d(d) {}

  constexpr Direction(const int d) : d(int8_t(d) & 7) {}

  /**
   * @brief Calculate the absolute direction from the absolute direction and the relative direction.
   * @param absolute The absolute direction.
   * @param relative The relative direction.
   * @return The absolute direction calculated from the absolute direction and the relative direction.
   */
  static constexpr Absolute getNewDirection(const Absolute absolute, const Relative relative) {
    int abs = static_cast<int>(absolute);
    int rel = static_cast<int>(relative);
    return newDirectionLookup[abs][rel];
  }

  /**
   * @brief Cast to integers.It can be used for calculations such as relative direction.
   * It is also how we get to use a direction as an index into an array
   */
  constexpr operator int() const { return d; }

  constexpr uint8_t operator-(const Direction& other) const {
    // Subtract the two absolute directions and normalize within 0-7 range

    uint8_t result = (d - other.d + DIR_COUNT) % DIR_COUNT;
    return result;
    //      return static_cast<Relative>(result);

    int diff = d - other.d;

    // Normalize the result to handle wraparound between 0 and 7
    if (diff > DIR_COUNT / 2) {
      diff -= DIR_COUNT;  // Adjust for large positive differences
    } else if (diff < -DIR_COUNT / 2) {
      diff += DIR_COUNT;  // Adjust for large negative differences
    }

    return diff;
  }

  bool isOrthogonal() const { return !(d & 1); }

  bool isDiagonal() const { return (d & 1); }

  char toAbsoluteChar() const { return "^/>\\v/<\\X"[d]; }

  char toRelativeChar() const { return "FrRrBlLl"[d]; }

  char toHeadingChar() const { return "NnEeSsWw"[d]; }

 private:
  int8_t d;
};

using Directions = std::vector<Direction>;

}  // namespace MazeLib

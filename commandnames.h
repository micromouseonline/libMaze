#ifndef _commandnames_h
#define _commandnames_h

#include <stdint.h>
/*
 * See docs/command-names.md for a description of NEW values
 */
/*
 * mouse command bit values:
 *
 * Straights:
 *  00LLLLLL
 *  ||||||||
 *  ||++++++-- Run Length in Cells (0-63)
 *  ||
 *  ++-------- 00 => straight (orthogonal)
 *
 * Since there should never be a zero length orthogonal straight
 * it is convenient to use the command value 0x00 as an end marker
 *
 * Diagonals:
 *  01LLLLLL
 *  ||||||||
 *  ||++++++-- Run Length in Cells (0-63)
 *  ||
 *  ++-------- 01 => diagonal
 *
 *
 * Turns - In Place
 *  10000TTD
 *  ||||||||
 *  |||||||+--  0 => Turn Right
 *  |||||||     1 => Turn Left
 *  |||||||
 *  |||||++--- 00 => IP45  Turn
 *  |||||      01 => IP90  Turn
 *  |||||      10 => IP135 Turn
 *  |||||      11 => IP180 Turn
 *  |||||
 *  |||||
 *  +++++----- 1000 => Turn In Place
 *
 *
 * Turns - Smooth
 *  101TTTTD
 *  ||||||||
 *  |||||||+--  0 => Turn Right
 *  |||||||     1 => Turn Left
 *  |||||||
 *  |||++++--- 0000 => SS90  Turn
 *  |||        0001 => SS180 Turn
 *  |||        0010 => SD45  Turn
 *  |||        0011 => SD135 Turn
 *  |||        0100 => DS45 Turn
 *  |||        0101 => DS135 Turn
 *  |||        0110 => DD90 Turn
 *  |||        0111 => EX90 Turn
 *  |||        1xxx => unused (available for complex turns)
 *  |||
 *  +++------- 101 => Turn Smooth
 *
 *
 * Messages/Flags:
 *  11MMMMMM
 *  ||||||||
 *  ||++++++-- Message Code
 *  ||         000000 => Start
 *  ||         000001 => Stop
 *  ||         000010 => Explore
 *  ||         01xxxx => unused
 *  ||         10xxxx => unused
 *  ||         11xxxx => Error Codes
 *  ||
 *  ++-------- 11 => Message
 *
 *
 * Binary constants are a C++14 feature. If you have a cross compiler for
 * a microcontroller it probably supports the binary constant syntax.
 * If not, sorry but you will have to convert the values by hand.
 */

typedef uint8_t MoveCommand;

enum {
  CMD_TYPE_ORTHO,
  CMD_TYPE_DIAG,
  CMD_TYPE_INPLACE,
  CMD_TYPE_SMOOTH,
  CMD_TYPE_MESSAGE,
  CMD_TYPE_COMMAND,
  CMD_TYPE_ERROR,
  CMD_TYPE_UNKNOWN = -1
};

#define INPLACE (0b01000000) _Pragma("GCC warning \"'INPLACE' macro is deprecated\"")
#define SMOOTH (0b01100000) _Pragma("GCC warning \"'SMOOTH' macro is deprecated\"")

#define MOVE_TYPE_MASK 0b11100000
#define MOVE_LENGTH_MASK 0b00011111
#define TURN_INDEX_MASK 0b00011111
#define TURN_DIR_MASK 0b00000001

#define MOVE_TYPE_ORTHO 0b00000000
#define MOVE_TYPE_DIAG 0b00100000
#define MOVE_TYPE_IP 0b01000000
#define MOVE_TYPE_SMOOTH 0b01100000
#define MOVE_TYPE_CMD 0b10000000
#define MOVE_TYPE_MSG 0b10100000
#define MOVE_TYPE_MSG_X 0b11000000  // unused messages
#define MOVE_TYPE_ERR 0b11100000

#define TURN_LEFT 1
#define TURN_RIGHT 0

inline int getMoveLength(MoveCommand move) {
  return move & MOVE_LENGTH_MASK;
}

inline bool isErr(MoveCommand move) {
  return (move & MOVE_TYPE_MASK) == MOVE_TYPE_ERR;
}

inline bool isCmd(MoveCommand move) {
  return (move & MOVE_TYPE_MASK) == MOVE_TYPE_CMD;
}

inline bool isMsg(MoveCommand move) {
  return (move & MOVE_TYPE_MASK) == MOVE_TYPE_MSG;
}

inline bool isOrtho(MoveCommand move) {
  return (move & MOVE_TYPE_MASK) == MOVE_TYPE_ORTHO;
}

inline bool isDiagonal(MoveCommand move) {
  return (move & MOVE_TYPE_MASK) == MOVE_TYPE_DIAG;
}

inline bool isSmoothTurn(MoveCommand move) {
  return (move & MOVE_TYPE_MASK) == MOVE_TYPE_SMOOTH;
}

inline bool isInPlaceTurn(MoveCommand move) {
  return (move & MOVE_TYPE_MASK) == MOVE_TYPE_IP;
}

inline bool isTurn(MoveCommand move) {
  return isSmoothTurn(move) || isInPlaceTurn(move);
}

inline int getTurnIndex(MoveCommand move) {
  return move & TURN_INDEX_MASK;
}

inline int getTurnDirection(MoveCommand move) {
  return move & TURN_DIR_MASK;
}

inline bool isLeftTurn(MoveCommand move) {
  return getTurnDirection(move) == TURN_LEFT;
}

inline int getMoveType(MoveCommand move) {
  if (isOrtho(move)) {
    return CMD_TYPE_ORTHO;
  }
  if (isDiagonal(move)) {
    return CMD_TYPE_DIAG;
  }
  if (isSmoothTurn(move)) {
    return CMD_TYPE_SMOOTH;
  }
  if (isInPlaceTurn(move)) {
    return CMD_TYPE_INPLACE;
  }
  if (isErr(move)) {
    return CMD_TYPE_ERROR;
  }
  if (isCmd(move)) {
    return CMD_TYPE_COMMAND;
  }
  if (isMsg(move)) {
    return CMD_TYPE_MESSAGE;
  }
  return CMD_TYPE_UNKNOWN;
}

#define CMD_STOP (0x00)
#define STOP CMD_STOP _Pragma("GCC warning \"'STOP' macro is deprecated\"")

#define CMD_BEGIN (0x80)
#define CMD_EXPLORE (0x81)
#define CMD_END (0x82)

// error codes  0xf0-0xff
#define CMD_ERROR_NOF (0xF0)
#define CMD_ERROR_RRR (0xF1)
#define CMD_ERROR_LLL (0xF2)
#define CMD_ERR_BEGIN (0xF3)
#define CMD_ERROR_END (0xF4)

#define CMD_ERROR (0xFF)

enum {
  FWD0 = MOVE_TYPE_ORTHO,
  FWD1,
  FWD2,
  FWD3,
  FWD4,
  FWD5,
  FWD6,
  FWD7,
  FWD8,
  FWD9,
  FWD10,
  FWD11,
  FWD12,
  FWD13,
  FWD14,
  FWD15,
  FWD16,
  FWD17,
  FWD18,
  FWD19,
  FWD20,
  FWD21,
  FWD22,
  FWD23,
  FWD24,
  FWD25,
  FWD26,
  FWD27,
  FWD28,
  FWD29,
  FWD30,
  FWD31,
  DIA0 = MOVE_TYPE_DIAG,
  DIA1,
  DIA2,
  DIA3,
  DIA4,
  DIA5,
  DIA6,
  DIA7,
  DIA8,
  DIA9,
  DIA10,
  DIA11,
  DIA12,
  DIA13,
  DIA14,
  DIA15,
  DIA16,
  DIA17,
  DIA18,
  DIA19,
  DIA20,
  DIA21,
  DIA22,
  DIA23,
  DIA24,
  DIA25,
  DIA26,
  DIA27,
  DIA28,
  DIA29,
  DIA30,
  DIA31,
  //
  //  DIA32, DIA33, DIA34, DIA35, DIA36, DIA37, DIA38, DIA39,
  //  DIA40, DIA41, DIA42, DIA43, DIA44, DIA45, DIA46, DIA47,
  //  DIA48, DIA49, DIA50, DIA51, DIA52, DIA53, DIA54, DIA55,
  //  DIA56, DIA57, DIA58, DIA59, DIA60, DIA61, DIA62, DIA63,
};

#define IP45R (MOVE_TYPE_IP + 0)
#define IP45L (MOVE_TYPE_IP + 1)
#define IP90R (MOVE_TYPE_IP + 2)
#define IP90L (MOVE_TYPE_IP + 3)
#define IP135R (MOVE_TYPE_IP + 4)
#define IP135L (MOVE_TYPE_IP + 5)
#define IP180R (MOVE_TYPE_IP + 6)
#define IP180L (MOVE_TYPE_IP + 7)

#define SS90SR (MOVE_TYPE_SMOOTH + 0)
#define SS90SL (MOVE_TYPE_SMOOTH + 1)
#define SS90FR (MOVE_TYPE_SMOOTH + 2)
#define SS90FL (MOVE_TYPE_SMOOTH + 3)
#define SS180R (MOVE_TYPE_SMOOTH + 4)
#define SS180L (MOVE_TYPE_SMOOTH + 5)
#define SD45R (MOVE_TYPE_SMOOTH + 6)
#define SD45L (MOVE_TYPE_SMOOTH + 7)
#define SD135R (MOVE_TYPE_SMOOTH + 8)
#define SD135L (MOVE_TYPE_SMOOTH + 9)
#define DS45R (MOVE_TYPE_SMOOTH + 10)
#define DS45L (MOVE_TYPE_SMOOTH + 11)
#define DS135R (MOVE_TYPE_SMOOTH + 12)
#define DS135L (MOVE_TYPE_SMOOTH + 13)
#define DD90R (MOVE_TYPE_SMOOTH + 14)
#define DD90L (MOVE_TYPE_SMOOTH + 15)
#define SS90ER (MOVE_TYPE_SMOOTH + 16)
#define SS90EL (MOVE_TYPE_SMOOTH + 17)

#endif

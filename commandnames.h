#ifndef _commandnames_h
#define _commandnames_h

/*
 * See docs/command-names.md for a description of these values
 */

typedef uint8_t MoveCommand;

enum { CMD_TYPE_ORTHO, CMD_TYPE_DIAG, CMD_TYPE_INPLACE, CMD_TYPE_SMOOTH, CMD_TYPE_MESSAGE, CMD_TYPE_UNKNOWN = -1 };

#define MOVE_TYPE_ORTHO 0b00000000
#define MOVE_TYPE_DIAG 0b01000000
#define MOVE_TYPE_TURN 0b10000000
#define MOVE_TYPE_MSSG 0b11000000

#define TURN_TYPE_INPLACE 0b00000000
#define TURN_TYPE_SMOOTH 0b00100000

#define MOVE_TYPE_MASK 0b11000000
#define MOVE_LENGTH_MASK 0b00111111
#define TURN_TYPE_MASK 0b00100000
#define TURN_INDEX_MASK 0b00011111
#define TURN_DIR_MASK 0b00000001

inline int getMoveLength(MoveCommand move) {
  return move & MOVE_LENGTH_MASK;
}

inline bool isErrorMssg(MoveCommand move) {
  return (move & MOVE_TYPE_MASK) == MOVE_TYPE_MSSG;
}

inline bool isOrtho(MoveCommand move) {
  return (move & MOVE_TYPE_MASK) == MOVE_TYPE_ORTHO;
}

inline bool isDiagonal(MoveCommand move) {
  return (move & MOVE_TYPE_MASK) == MOVE_TYPE_DIAG;
}

inline bool isTurn(MoveCommand move) {
  return (move & MOVE_TYPE_MASK) == MOVE_TYPE_TURN;
}

inline bool isSmoothTurn(MoveCommand move) {
  return (move & (MOVE_TYPE_MASK | TURN_TYPE_MASK)) == (MOVE_TYPE_TURN + TURN_TYPE_SMOOTH);
}

inline bool isInPlaceTurn(MoveCommand cmd) {
  return (cmd & (MOVE_TYPE_MASK | TURN_TYPE_MASK)) == (MOVE_TYPE_TURN + TURN_TYPE_INPLACE);
}

inline int getTurnIndex(MoveCommand move) {
  return move & TURN_INDEX_MASK;
}
inline int getTurnDirection(MoveCommand cmd) {
  return cmd & TURN_DIR_MASK;
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
  if (isErrorMssg(move)) {
    return CMD_TYPE_MESSAGE;
  }
  return CMD_TYPE_UNKNOWN;
}

#define CMD_END (0x00)
#define CMD_STOP (0xC0)
#define CMD_BEGIN (0xC1)
#define CMD_EXPLORE (0xC2)

// error codes  0xf0-0xff
#define CMD_ERROR_NOF (0xF0)
#define CMD_ERROR_RRR (0xF1)
#define CMD_ERROR_LLL (0xF2)
#define CMD_ERR_BEGIN (0xF3)
#define CMD_ERROR_END (0xF4)
#define CMD_ERROR (0xFF)

// It would be nice if there were an easier way to generate these
enum {
  FWD0 = 0b00000000,
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

  DIA0 = 0b01000000,
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

  DIA32,
  DIA33,
  DIA34,
  DIA35,
  DIA36,
  DIA37,
  DIA38,
  DIA39,
  DIA40,
  DIA41,
  DIA42,
  DIA43,
  DIA44,
  DIA45,
  DIA46,
  DIA47,
  DIA48,
  DIA49,
  DIA50,
  DIA51,
  DIA52,
  DIA53,
  DIA54,
  DIA55,
  DIA56,
  DIA57,
  DIA58,
  DIA59,
  DIA60,
  DIA61,
  DIA62,
  DIA63,

  INPLACE = 0b10000000,
  IP45R = INPLACE + 0,
  IP45L = INPLACE + 1,
  IP90R = INPLACE + 2,
  IP90L = INPLACE + 3,
  IP135R = INPLACE + 4,
  IP135L = INPLACE + 5,
  IP180R = INPLACE + 6,
  IP180L = INPLACE + 7,

  SMOOTH = 0b10100000,
  SS90FR = SMOOTH + 0,
  SS90FL = SMOOTH + 1,
  SS180R = SMOOTH + 2,
  SS180L = SMOOTH + 3,
  SD45R = SMOOTH + 4,
  SD45L = SMOOTH + 5,
  SD135R = SMOOTH + 6,
  SD135L = SMOOTH + 7,
  DS45R = SMOOTH + 8,
  DS45L = SMOOTH + 9,
  DS135R = SMOOTH + 10,
  DS135L = SMOOTH + 11,
  DD90R = SMOOTH + 12,
  DD90L = SMOOTH + 13,
  SS90ER = SMOOTH + 14,
  SS90EL = SMOOTH + 15,
};

#endif

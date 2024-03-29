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

#include "mazeprinter.h"
#include <cstdio>

static char dirChars[] = "^>v<#  ";

void printNorthWalls(Maze *maze, uint16_t row) {
  for (uint16_t col = 0; col < maze->width(); col++) {
    uint16_t cell = row + maze->width() * col;
    printf("o");
    if (maze->hasOpenExit(cell, NORTH)) {
      printf("   ");
    } else {
      printf("---");
    }
  }
  printf("o\n");
}

void printSouthWalls(Maze *maze, uint16_t row) {
  for (uint16_t col = 0; col < maze->width(); col++) {
    uint16_t cell = row + maze->width() * col;
    printf("o");
    if (maze->hasOpenExit(cell, SOUTH)) {
      printf("   ");
    } else {
      printf("---");
    }
  }
  printf("o\n");
}

void MazePrinter::printDirs(Maze *maze) {
  printf("\n");
  for (int row = static_cast<uint16_t>(maze->width() - 1); row >= 0; row--) {
    printNorthWalls(maze, row);
    for (uint16_t col = 0; col < maze->width(); col++) {
      uint16_t cell = row + maze->width() * col;
      if (maze->hasOpenExit(cell, WEST)) {
        printf(" ");
      } else {
        printf("|");
      }
      uint8_t direction = maze->directionToSmallest(cell);
      char c = dirChars[direction];
      if (cell == maze->goal()) {
        c = '*';
      }
      printf(" %c ", c);
    }
    uint16_t cell = row + maze->width() * (maze->width() - 1);
    if (maze->hasOpenExit(cell, EAST)) {
      printf(" ");
    } else {
      printf("|");
    }
    printf("\n");
  }
  printSouthWalls(maze, 0);
}

void MazePrinter::printVisitedDirs(Maze *maze) {
  printf("\n");
  for (int row = static_cast<uint16_t>(maze->width() - 1); row >= 0; row--) {
    printNorthWalls(maze, row);
    for (uint16_t col = 0; col < maze->width(); col++) {
      uint16_t cell = row + maze->width() * col;
      if (maze->hasOpenExit(cell, WEST)) {
        printf(" ");
      } else {
        printf("|");
      }
      uint8_t direction = maze->directionToSmallest(cell);
      char c = ' ';
      if (maze->isVisited(cell)) {
        c = dirChars[direction];
      }
      if (cell == maze->goal()) {
        c = '*';
      }
      printf(" %c ", c);
    }
    uint16_t cell = row + maze->width() * (maze->width() - 1);
    if (maze->hasOpenExit(cell, EAST)) {
      printf(" ");
    } else {
      printf("|");
    }
    printf("\n");
  }
  printSouthWalls(maze, 0);
}

void MazePrinter::printPlain(Maze *maze) {
  printf("\n");
  for (int row = static_cast<uint16_t>(maze->width() - 1); row >= 0; row--) {
    printNorthWalls(maze, row);
    /* TODO:  this is all rather messy */
    for (uint16_t col = 0; col < maze->width(); col++) {
      uint16_t cell = row + maze->width() * col;
      if (maze->hasOpenExit(cell, WEST)) {
        printf(" ");
      } else {
        printf("|");
      }
      char c = ' ';
      if (maze->goalContains(cell)) {
        c = 'G';
      }
      if (cell == 0) {
        c = 'S';
      }
      printf(" %c ", c);
    }
    uint16_t cell = row + maze->width() * (maze->width() - 1);
    if (maze->hasOpenExit(cell, EAST)) {
      printf(" ");
    } else {
      printf("|");
    }
    printf("\n");
  }
  printSouthWalls(maze, 0);
  printf("\n");
}

void MazePrinter::printCDecl(Maze *maze, const char *name) {
  printf("\n\nconst uint8_t %s[] = {\n", name);
  for (uint16_t x = 0; x < maze->width(); x++) {
    printf("   ");
    for (uint16_t y = 0; y < maze->width(); y++) {
      uint16_t cell = x * maze->width() + y;
      uint8_t walls = maze->walls(cell);
      if (maze->goalContains(cell)) {
        walls |= 0x80;
      }
      printf("0x%02X, ", walls);
    }
    printf("\n");
  }
  printf("   };\n\n");
}

void MazePrinter::printRawDecl(Maze *maze, const char *name) {
  printf("\n\nconst uint8_t %s[] = {\n", name);
  for (uint16_t x = 0; x < maze->width(); x++) {
    printf("   ");
    for (uint16_t y = 0; y < maze->width(); y++) {
      uint16_t cell = x * maze->width() + y;
      printf("0x%02X, ", maze->walls(cell));
    }
    printf("\n");
  }
  printf("   };\n\n");
}

void MazePrinter::printCosts(Maze *maze) {
  printf("\n");
  for (int row = static_cast<uint16_t>(maze->width() - 1); row >= 0; row--) {
    printNorthWalls(maze, row);
    /* TODO:  this is all rather messy */
    for (uint16_t col = 0; col < maze->width(); col++) {
      uint16_t cell = row + maze->width() * col;
      if (maze->hasOpenExit(cell, WEST)) {
        printf(" ");
      } else {
        printf("|");
      }
      uint16_t cost = maze->cost(cell);
      if (cost < MAX_COST) {
        printf("%3d", cost);
      } else {
        printf(" - ");
      }
    }
    uint16_t cell = row + maze->width() * (maze->width() - 1);
    if (maze->hasOpenExit(cell, EAST)) {
      printf(" ");
    } else {
      printf("|");
    }
    printf("\n");
  }
  printSouthWalls(maze, 0);
}

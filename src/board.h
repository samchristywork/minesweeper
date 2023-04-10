#ifndef BOARD_H
#define BOARD_H

#include <square.h>

class Board {
public:
  int width;
  int height;
  int num_mines;
  Square **squares;

  Board(int width, int height, int num_mines);

  Square *GetCollision(int mx, int my);
  Square *GetSquare(int x, int y);
  int GetNumNeighbors(int x, int y);
  int GetNumNeighborsCovered(int x, int y);
  int GetNumNeighborsFlagged(int x, int y);
  void Reset();
  void AutoFlag();
  void AutoComplete();
  void MarkProbability();
  void SeedRandomMine();
};

#endif

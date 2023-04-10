#ifndef SQUARE_H
#define SQUARE_H

enum { COVERED, UNCOVERED };

class Square {
public:
  int state;
  bool is_hover;
  bool is_mine;
  bool is_flag;
  int success_probability;

  Square();
  void Initialize();
};

#endif

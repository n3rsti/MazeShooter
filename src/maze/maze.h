#ifndef MAZE_H
#define MAZE_H

#include <utility>
#include <vector>

class Maze {
public:
  int width;
  int height;
  std::vector<std::vector<int>> maze;

  Maze(int width, int height);
  void generate_maze(int x, int y, int count = 0);
  void print_maze();
  int get_cell(int x, int z);

private:
  const std::vector<std::pair<int, int>> DIRECTIONS = {
      {-2, 0}, {2, 0}, {0, -2}, {0, 2}};

  bool is_valid(int x, int y);
};

#endif

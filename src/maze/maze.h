#ifndef MAZE_H
#define MAZE_H

#include <vector>
#include <utility>

class Maze {
public:
    int width;
    int height;
    std::vector<std::vector<int>> maze;

    Maze(int width, int height);
    void generate_maze(int x, int y, int count = 0);
    void print_maze();

private:
    const std::vector<std::pair<int, int>> DIRECTIONS = {
        {-4, 0}, {4, 0}, {0, -4}, {0, 4}
    };

    bool is_valid(int x, int y);
};

#endif

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

    std::pair<int, int> bfs_next_move(std::vector<std::vector<int>> maze,
                                      std::pair<int, int> entity,
                                      std::pair<int, int> player);

    // Maze grid to world
    static float grid_to_world(int grid) {
        return (grid - 35) * 2.0f; // Inverse of your mapping (removes +1)/2
    }

    // World to maze grid
    static int world_to_grid(float world) { return int((world + 1) / 2 + 35); }

  private:
    const std::vector<std::pair<int, int>> DIRECTIONS = {
        {-2, 0}, {2, 0}, {0, -2}, {0, 2}};

    bool is_valid(int x, int y);
};

#endif

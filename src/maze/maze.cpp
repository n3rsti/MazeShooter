#include "maze.h"
#include <algorithm>
#include <iostream>
#include <queue>
#include <random>
#include <utility>
#include <vector>

Maze::Maze(int width, int height) : width(width), height(height) {
    maze.resize(
        height,
        std::vector<int>(width, 1)); // Wypełnij labirynt ścianami (1 = ściana)
}

bool Maze::is_valid(int x, int y) {
    return (0 < x) && (x < height) && (0 < y) && (y < width);
}

void Maze::generate_maze(int x, int y, int count) {
    maze[y][x] = 0; // Odwiedzona komórka

    std::random_device rd;
    std::mt19937 gen(rd());
    auto dirs = DIRECTIONS;
    std::shuffle(dirs.begin(), dirs.end(), gen);

    for (const auto &dir : dirs) {
        int nx = x + dir.first;
        int ny = y + dir.second;

        if (is_valid(nx, ny) && maze[ny][nx] == 1) {
            maze[x + dir.first / 2][y + dir.second / 2] = 0;
            generate_maze(nx, ny, count + 1);
        }

        // std::cout << "Count: " << count << std::endl;
    }
}

void Maze::print_maze() {
    for (const auto &row : maze) {
        for (int cell : row) {
            std::cout << (cell == 0 ? " " : "#");
        }
        std::cout << '\n';
    }
}

int Maze::get_cell(int x, int z) {
    if (!is_valid(x, z)) {
        return -1;
    }

    return maze[x][z];
}

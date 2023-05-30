#pragma once

#include <memory>
#include <filesystem>

class GameOfLife
{
private:
    enum CellState
    {
        // NOTE: Inverted to match PBM format (white = alive, black = dead)
        ALIVE = 0,
        DEAD = 1,
    };
    CellState *m_cells;
    CellState *m_cellsNext;
    uint16_t m_width;
    uint16_t m_height;

    uint8_t countAliveNeighbors(const uint16_t x, const uint16_t y);

public:
    GameOfLife(const uint16_t width, const uint16_t height);
    ~GameOfLife();

    void update();

    operator std::string() const;

    static std::unique_ptr<GameOfLife> random(const uint16_t width, const uint16_t height);
    static std::unique_ptr<GameOfLife> load(const std::filesystem::path &path);
    void store(const std::filesystem::path &path);
};

#include <iostream>
#include <fstream>
#include <cstring>

#include "GameOfLife.hpp"

GameOfLife::GameOfLife(const uint16_t width, const uint16_t height) : m_width(width), m_height(height)
{
    m_cells = new CellState[width * height];
    for (int i = 0; i < width * height; i++)
    {
        m_cells[i] = CellState::DEAD;
    }
    m_cellsNext = new CellState[width * height];
}

GameOfLife::~GameOfLife()
{
    delete[] m_cells;
    delete[] m_cellsNext;
}

uint8_t GameOfLife::countAliveNeighbors(const uint16_t x, const uint16_t y)
{
    uint8_t aliveNeighbors = 0;
    // Check all directions
    for (int offsetY = -1; offsetY <= 1; offsetY++)
    {
        for (int offsetX = -1; offsetX <= 1; offsetX++)
        {
            if (0 == offsetY && 0 == offsetX)
                continue; // Do not count self
            // Wrap around boundary condition
            int neighborY = (y + m_height + offsetY) % m_height;
            int neighborX = (x + m_width + offsetX) % m_width;
            int neighborIsAlive = CellState::ALIVE == m_cells[neighborY * m_width + neighborX];
            aliveNeighbors += neighborIsAlive;
        }
    }
    return aliveNeighbors;
}

void GameOfLife::update()
{
    for (int y = 0; y < m_height; y++)
    {
        for (int x = 0; x < m_width; x++)
        {
            CellState lastCellState = m_cells[y * m_width + x];
            uint8_t aliveNeighbors = countAliveNeighbors(x, y);
            CellState nextCellState = (
                                          // 2 or 3 neighbors alive --> stay alive
                                          (CellState::ALIVE == lastCellState && (2 == aliveNeighbors || 3 == aliveNeighbors)) ||
                                          // 3 neighbors alive --> become alive
                                          (CellState::DEAD == lastCellState && 3 == aliveNeighbors)
                                          )
                                          ? CellState::ALIVE
                                          // else die or stay dead
                                          : CellState::DEAD;
            m_cellsNext[y * m_width + x] = nextCellState;
        }
    }
    // Swap buffers
    auto tmp = m_cells;
    m_cells = m_cellsNext;
    m_cellsNext = tmp;
}

std::unique_ptr<GameOfLife> GameOfLife::random(const uint16_t width, const uint16_t height)
{
    auto golInitialState = std::make_unique<GameOfLife>(width, height);
    for (int i = 0; i < width * height; i++)
    {
        golInitialState->m_cells[i] = rand() % 2 ? CellState::ALIVE : CellState::DEAD;
    }
    return golInitialState;
}

std::unique_ptr<GameOfLife> GameOfLife::load(const std::filesystem::path &path)
{
    // TODO: Ignore all lines starting with #
    std::string line;
    std::ifstream file;
    file.open(path);
    getline(file, line);
    if (line != "P1")
    {
        std::cerr << "Invalid file format (magic number should be P1)" << std::endl;
        exit(1);
    }
    getline(file, line);
    uint16_t width, height;
    sscanf(line.c_str(), "%hu %hu", &width, &height);
    auto golState = std::make_unique<GameOfLife>(width, height);
    for (int i = 0; i < width * height;)
    {
        int c = file.get();
        if ('0' == c)
        {
            golState->m_cells[i] = CellState::ALIVE;
            i++;
        }
        else if ('1' == c)
        {
            golState->m_cells[i] = CellState::DEAD;
            i++;
        }
    }
    return golState;
}

GameOfLife::operator std::string() const
{
    std::stringstream fmtd;
    for (int y = 0; y < m_height; y++)
    {
        for (int x = 0; x < m_width; x++)
        {
            int cellState = m_cells[y * m_width + x];
            fmtd << cellState << " ";
        }
        fmtd << std::endl;
    }
    return fmtd.str();
}

void GameOfLife::store(const std::filesystem::path &path)
{
    std::ofstream file;
    file.open(path);
    file << "P1"
         << std::endl
         << m_width
         << " "
         << m_height
         << std::endl
         << std::string(*this) << std::flush;
    file.close();
}

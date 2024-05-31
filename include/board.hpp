#pragma once

#include <vector>
#include <array>

#include "utils.hpp"

constexpr int boardSize = 11;

// 玩家信息
enum class Player
{
    Empty = 0,
    Self = 1,
    Opponent = -1
};

inline Player nextPlayer(Player player)
{
    if (player == Player::Self)
        return Player::Opponent;
    return Player::Self;
}

class Board
{
public:
    Board() = default;

	Player operator()(int x, int y) const
	{
		return info[x][y];
	}

	Player& operator()(int x, int y)
	{
		return info[x][y];
	}

    Player operator()(const Coord& coord) const
    {
        return info[coord.x][coord.y];
    }

    Player& operator()(const Coord& coord)
    {
        return info[coord.x][coord.y];
    }

    std::vector<Coord> getMoveList() const noexcept
    {
        return moveList;
    }

    std::vector<Coord> getUnmoveList() const noexcept
    {
        return unmoveList;
    }

    Coord getRecent() const noexcept
    {
        return recent;
    }

    // 初始化棋盘信息
    void init(const std::array<std::array<Player, boardSize>, boardSize>& info, const Coord& recent)
    {
        this->info = info;
        this->recent = recent;

        // 初始化合法的落子点
        for (int x = 0; x < boardSize; ++x)
        {
            for (int y = 0; y < boardSize; ++y)
            {
                Coord coord(x, y);
                if (operator()(coord) == Player::Empty)
                {
                    unmoveList.emplace_back(coord);
                }
                else
                {
                    moveList.emplace_back(coord);
                }
            }
        }
    }

    // 设置落子
    void setMove(const Coord& coord, Player player)
    {
        operator()(coord) = player;
        recent = coord;

        // 增加已落子的格子
        moveList.emplace_back(coord);

        // 移除未落子的格子
        for (std::size_t i = 0; i < unmoveList.size(); ++i)
        {
            if (unmoveList[i] == coord)
            {
                unmoveList.erase(unmoveList.begin() + i);
                break;
            }
        }
    }

	// 坐标是否合法
	bool has(const Coord& coord) const
	{
		if (coord.x < 0 || coord.x >= boardSize || coord.y < 0 || coord.y >= boardSize)
			return false;
		return true;
	}

	// 得到临近的坐标
	std::vector<Coord> neighbors(const Coord& coord) const
	{
		std::vector<Coord> neighbors;

		if (coord.x > 0)
		{
            neighbors.emplace_back(coord.x - 1, coord.y);
			if (coord.y < boardSize - 1)
                neighbors.emplace_back(coord.x - 1, coord.y + 1);
		}

		if (coord.x < boardSize - 1)
		{
            neighbors.emplace_back(coord.x + 1, coord.y);
			if (coord.y > 0)
                neighbors.emplace_back(coord.x + 1, coord.y - 1);
		}

		if (coord.y > 0)
            neighbors.emplace_back(coord.x, coord.y - 1);

		if (coord.y < boardSize - 1)
            neighbors.emplace_back(coord.x, coord.y + 1);

		return neighbors;
	}

    std::vector<Coord> surrounding(const Coord& coord) const
    {
        std::vector<Coord> surroundings;

        // 相邻位置
        Coord surrounding(coord.x - 1, coord.y);
        if (has(surrounding)) surroundings.emplace_back(surrounding);

        surrounding = Coord(coord.x - 1, coord.y + 1);
        if (has(surrounding)) surroundings.emplace_back(surrounding);

        surrounding = Coord(coord.x + 1, coord.y);
        if (has(surrounding)) surroundings.emplace_back(surrounding);

        surrounding = Coord(coord.x + 1, coord.y - 1);
        if (has(surrounding)) surroundings.emplace_back(surrounding);

        surrounding = Coord(coord.x, coord.y - 1);
        if (has(surrounding)) surroundings.emplace_back(surrounding);

        surrounding = Coord(coord.x, coord.y + 1);
        if (has(surrounding)) surroundings.emplace_back(surrounding);

        // 桥的位置
        surrounding = Coord(coord.x + 1, coord.y + 1);
        if (has(surrounding)) surroundings.emplace_back(surrounding);

        surrounding = Coord(coord.x - 1, coord.y + 2);
        if (has(surrounding)) surroundings.emplace_back(surrounding);

        surrounding = Coord(coord.x - 2, coord.y + 1);
        if (has(surrounding)) surroundings.emplace_back(surrounding);

        surrounding = Coord(coord.x - 1, coord.y - 1);
        if (has(surrounding)) surroundings.emplace_back(surrounding);

        surrounding = Coord(coord.x + 1, coord.y - 2);
        if (has(surrounding)) surroundings.emplace_back(surrounding);

        surrounding = Coord(coord.x + 2, coord.y - 1);
        if (has(surrounding)) surroundings.emplace_back(surrounding);

        // 跳的位置
        surrounding = Coord(coord.x + 2, coord.y);
        if (has(surrounding)) surroundings.emplace_back(surrounding);

        surrounding = Coord(coord.x, coord.y + 2);
        if (has(surrounding)) surroundings.emplace_back(surrounding);

        surrounding = Coord(coord.x - 2, coord.y + 2);
        if (has(surrounding)) surroundings.emplace_back(surrounding);

        surrounding = Coord(coord.x - 2, coord.y);
        if (has(surrounding)) surroundings.emplace_back(surrounding);

        surrounding = Coord(coord.x, coord.y - 2);
        if (has(surrounding)) surroundings.emplace_back(surrounding);

        surrounding = Coord(coord.x + 2, coord.y - 2);
        if (has(surrounding)) surroundings.emplace_back(surrounding);

        return surroundings;
    }

private:
    std::array<std::array<Player, boardSize>, boardSize> info;  // 棋盘信息

    std::vector<Coord> moveList;            // 已落子的格子
    std::vector<Coord> unmoveList;          // 未落子的格子
    Coord recent;                           // 对方最近一步下的棋
};
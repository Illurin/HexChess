#pragma once

#include <array>

#include "board.hpp"

class Simulation
{
public:
	Simulation(const Board& board, Player red) : board(board), red(red)
	{}

	Player checkWinner() const
	{
		std::array<int, boardSize * boardSize> visited;

		// 检查红方是否获胜
        visited.fill(false);
		for (int y = 0; y < boardSize; ++y)
		{
			if (board(0, y) == red)
			{
				if (isWinHelper(visited, 0, y, red))
					return red;
			}
		}

		// 检查蓝方是否获胜
        auto blue = nextPlayer(red);

        visited.fill(false);
		for (int x = 0; x < boardSize; ++x)
		{
			if (board(x, 0) == blue)
			{
				if (isWinHelper(visited, x, 0, blue))
					return blue;
			}
		}

		return Player::Empty;
	}

	Player start(const Coord& move, Player player)
	{
        // prepare();

        // 从可行下法中随机选择
		auto unmoveList = board.getUnmoveList();
		std::shuffle(unmoveList.begin(), unmoveList.end(), randomGenerator());

		for (const auto& move : unmoveList)
		{
            if (board(move) != Player::Empty)
                continue;

			board.setMove(solve(move, player), player);

            // 切换棋手
            player = nextPlayer(player);
		}

        // 判断胜者
		return checkWinner();
	}

private:
    bool isWinHelper(std::array<int, boardSize * boardSize>& visited, int startX, int startY, Player player) const
    {
        Coord coord(startX, startY);

        if (!board.has(coord)) return false;
        if (board(coord) != player) return false;

        auto loc = startX * boardSize + startY;
        if (visited[loc])
            return false;
        visited[loc] = true;

        // 边缘判断
        if (player == red && startX == boardSize - 1) return true;
        if (player == nextPlayer(red) && startY == boardSize - 1) return true;

        auto neighbors = board.neighbors(coord);
        for (const auto& neighbor : neighbors)
        {
            if (isWinHelper(visited, neighbor.x, neighbor.y, player))
                return true;
        }

        return false;
    }

    // 在模拟前对棋局进行处理
    void prepare()
    {
        auto unmoveList = board.getUnmoveList();

        while (!unmoveList.empty())
        {
            auto move = unmoveList.back();
            unmoveList.pop_back();

            // 该格子已被占用
            if (board(move) != Player::Empty)
                continue;

			bool found = false;

            // 随机填充双桥
			auto plotA = move + Coord(1, -1); // 位于左边的坐标
			auto mid = move + Coord(1, 0);    // 位于左下角的坐标
			auto plotB = move + Coord(0, 1);  // 位于右下角的坐标

            for (int i = 0; i < 6; ++i)
            {
                if (isBridge(plotA, mid, plotB, Player::Self) ||
					isBridge(plotA, mid, plotB, Player::Opponent))
                {
					board.setMove(move, Player::Self);
					board.setMove(mid, Player::Opponent);
					found = true;
					break;
                }

				plotA += rotations[i];
				mid += rotations[(i + 1) % rotations.size()];
				plotB += rotations[(i + 2) % rotations.size()];
            }

			if (found)
				continue;

            // 随机填充无用位置
			if (isUseless(move))
				continue;
        }
    }

    // 根据上一步推演下一步的落子位置
    Coord solve(const Coord& move, Player player) const
    {
		//    / \ / \
		//   |   |   |
		//  / \ / \ / \
		// | + | * |   |
		//  \ / \ / \ /
		//   | - | + |
		//    \ / \ /

		// 获取上一步落子周围的格子
		auto recent = board.getRecent();
		auto plotA = recent + Coord(1, -1); // 位于左边的坐标
		auto mid = recent + Coord(1, 0);    // 位于左下角的坐标
		auto plotB = recent + Coord(0, 1);  // 位于右下角的坐标

        // 对上一步落子的周围检测是否存在桥
        for (int i = 0; i < 6; ++i)
        {
            if (isBridge(plotA, mid, plotB, player))
                return mid;

			plotA += rotations[i];
			mid += rotations[(i + 1) % rotations.size()];
			plotB += rotations[(i + 2) % rotations.size()];
        }

        return move;
    }

    // 判断桥式棋型
    bool isBridge(const Coord& plotA, const Coord& mid, const Coord& plotB, Player player) const
    {
        if (!board.has(mid) || board(mid) != Player::Empty)
            return false;

        if (board.has(plotA) && board.has(plotB))
        {
            if (board(plotA) != player)
                return false;

            if (board(plotB) != player)
                return false;

            return true;
        }

        if (player == red)
            return (mid.x == 0) || (mid.x == boardSize - 1);

        return (mid.y == 0) || (mid.y == boardSize - 1);
    }

	bool isUseless(const Coord& move)
	{
		auto plotA = move + Coord(1, -1);    // 位于左边的坐标
		auto midA = move + Coord(1, 0);      // 位于左下角的坐标
		auto midB = move + Coord(0, 1);	     // 位于右下角的坐标
		auto plotB = move + Coord(-1, 1);	 // 位于右边的坐标

		for (int i = 0; i < 6; ++i)
		{
			if (!board.has(plotA) || !board.has(plotB) || !board.has(midA) || !board.has(midB))
				continue;

			if (board(plotA) == Player::Self &&
				board(midA) == Player::Self &&
				board(midB) == Player::Self &&
				board(plotB) == Player::Self)
			{
				board.setMove(move, Player::Self);
				return true;
			}
			if (board(plotA) == Player::Opponent &&
				board(midA) == Player::Opponent &&
				board(midB) == Player::Opponent &&
				board(plotB) == Player::Opponent)
			{
				board.setMove(move, Player::Opponent);
				return true;
			}

			plotA += rotations[i];
			midA += rotations[(i + 1) % rotations.size()];
			midB += rotations[(i + 2) % rotations.size()];
			plotB += rotations[(i + 3) % rotations.size()];
		}

		plotA = move + Coord(1, -1);	// 位于左边的坐标
		midA = move + Coord(1, 0);      // 位于左下角的坐标
		midB = move + Coord(-1, 1);	    // 位于右边的坐标
		plotB = move + Coord(-1, 0);    // 位于右上角的坐标

		for (int i = 0; i < 6; ++i)
		{
			if (!board.has(plotA) || !board.has(plotB) || !board.has(midA) || !board.has(midB))
				continue;

			if (board(plotA) == Player::Self &&
				board(midA) == Player::Self &&
				board(midB) == Player::Opponent &&
				board(plotB) == Player::Opponent)
			{
				board.setMove(move, Player::Self);
				return true;
			}
			if (board(plotA) == Player::Opponent &&
				board(midA) == Player::Opponent &&
				board(midB) == Player::Self &&
				board(plotB) == Player::Self)
			{
				board.setMove(move, Player::Opponent);
				return true;
			}

			plotA += rotations[i];
			midA += rotations[(i + 1) % rotations.size()];
			midB += rotations[(i + 3) % rotations.size()];
			plotB += rotations[(i + 4) % rotations.size()];
		}

		midA = move + Coord(1, -1);     // 位于左边的坐标
		plotA = move + Coord(0, 1);		// 位于右下角的坐标
		midB = move + Coord(-1, 1);		// 位于右边的坐标
		plotB = move + Coord(-1, 0);    // 位于右上角的坐标

		for (int i = 0; i < 6; ++i)
		{
			if (!board.has(plotA) || !board.has(plotB) || !board.has(midA) || !board.has(midB))
				continue;

			if (board(midA) == Player::Self &&
				board(plotA) == Player::Opponent &&
				board(midB) == Player::Opponent &&
				board(plotB) == Player::Opponent)
			{
				board.setMove(move, Player::Self);
				return true;
			}
			if (board(midA) == Player::Opponent &&
				board(plotA) == Player::Self &&
				board(midB) == Player::Self &&
				board(plotB) == Player::Self)
			{
				board.setMove(move, Player::Self);
				return true;
			}

			plotA += rotations[i];
			midA += rotations[(i + 2) % rotations.size()];
			midB += rotations[(i + 3) % rotations.size()];
			plotB += rotations[(i + 4) % rotations.size()];
		}

		return false;
	}

private:
	Board board;
	Player red;

	const std::array<Coord, 6> rotations{ {
		Coord(0, 1),   // 向右下移动
		Coord(-1, 1),  // 向右移动
		Coord(-1, 0),  // 向右上移动
		Coord(0, -1),  // 向左上移动
		Coord(1, -1),  // 向左移动
		Coord(1, 0)    // 向左下移动
	} };
};
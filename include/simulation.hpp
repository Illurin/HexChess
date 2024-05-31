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

		// ���췽�Ƿ��ʤ
        visited.fill(false);
		for (int y = 0; y < boardSize; ++y)
		{
			if (board(0, y) == red)
			{
				if (isWinHelper(visited, 0, y, red))
					return red;
			}
		}

		// ��������Ƿ��ʤ
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

        // �ӿ����·������ѡ��
		auto unmoveList = board.getUnmoveList();
		std::shuffle(unmoveList.begin(), unmoveList.end(), randomGenerator());

		for (const auto& move : unmoveList)
		{
            if (board(move) != Player::Empty)
                continue;

			board.setMove(solve(move, player), player);

            // �л�����
            player = nextPlayer(player);
		}

        // �ж�ʤ��
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

        // ��Ե�ж�
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

    // ��ģ��ǰ����ֽ��д���
    void prepare()
    {
        auto unmoveList = board.getUnmoveList();

        while (!unmoveList.empty())
        {
            auto move = unmoveList.back();
            unmoveList.pop_back();

            // �ø����ѱ�ռ��
            if (board(move) != Player::Empty)
                continue;

			bool found = false;

            // ������˫��
			auto plotA = move + Coord(1, -1); // λ����ߵ�����
			auto mid = move + Coord(1, 0);    // λ�����½ǵ�����
			auto plotB = move + Coord(0, 1);  // λ�����½ǵ�����

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

            // ����������λ��
			if (isUseless(move))
				continue;
        }
    }

    // ������һ��������һ��������λ��
    Coord solve(const Coord& move, Player player) const
    {
		//    / \ / \
		//   |   |   |
		//  / \ / \ / \
		// | + | * |   |
		//  \ / \ / \ /
		//   | - | + |
		//    \ / \ /

		// ��ȡ��һ��������Χ�ĸ���
		auto recent = board.getRecent();
		auto plotA = recent + Coord(1, -1); // λ����ߵ�����
		auto mid = recent + Coord(1, 0);    // λ�����½ǵ�����
		auto plotB = recent + Coord(0, 1);  // λ�����½ǵ�����

        // ����һ�����ӵ���Χ����Ƿ������
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

    // �ж���ʽ����
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
		auto plotA = move + Coord(1, -1);    // λ����ߵ�����
		auto midA = move + Coord(1, 0);      // λ�����½ǵ�����
		auto midB = move + Coord(0, 1);	     // λ�����½ǵ�����
		auto plotB = move + Coord(-1, 1);	 // λ���ұߵ�����

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

		plotA = move + Coord(1, -1);	// λ����ߵ�����
		midA = move + Coord(1, 0);      // λ�����½ǵ�����
		midB = move + Coord(-1, 1);	    // λ���ұߵ�����
		plotB = move + Coord(-1, 0);    // λ�����Ͻǵ�����

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

		midA = move + Coord(1, -1);     // λ����ߵ�����
		plotA = move + Coord(0, 1);		// λ�����½ǵ�����
		midB = move + Coord(-1, 1);		// λ���ұߵ�����
		plotB = move + Coord(-1, 0);    // λ�����Ͻǵ�����

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
		Coord(0, 1),   // �������ƶ�
		Coord(-1, 1),  // �����ƶ�
		Coord(-1, 0),  // �������ƶ�
		Coord(0, -1),  // �������ƶ�
		Coord(1, -1),  // �����ƶ�
		Coord(1, 0)    // �������ƶ�
	} };
};
#pragma once

#include <cmath>
#include <algorithm>
#include <random>
#include <unordered_map>

#include "board.hpp"

class TreeNode
{
public:
	class Compare {
	public:
		Compare(double c) : c(c)
		{}

		bool operator()(TreeNode* pLhs, TreeNode* pRhs)
		{
			return pLhs->ucb(c) < pRhs->ucb(c);
		}

	private:
		double c;
	};

    TreeNode(TreeNode* pParent, const Coord& move, Player player, const Board& board)
        : pParent(pParent), move(move), player(player)
    {
        for (const auto& move : board.getMoveList())
        {
            auto surroundings = board.surrounding(move);
            for (const auto& surrounding : surroundings)
            {
                if (board(surrounding) != Player::Empty)
                    continue;

                bool found = false;
                for (const auto& coord : unmoveList)
                {
                    if (coord == surrounding)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                    unmoveList.emplace_back(surrounding);
            }
        }

        // �ԺϷ������ӵ�����������
        std::shuffle(unmoveList.begin(), unmoveList.end(), randomGenerator());
    }

    ~TreeNode()
    {}

	Coord getMove() const noexcept
	{
		return move;
	}

	Player getPlayer() const noexcept
	{
		return player;
	}

    double ucb(double c) const noexcept
    {
        return static_cast<double>(winCount) / static_cast<double>(simCount) +
            c * sqrt(log(static_cast<double>(pParent->simCount)) / static_cast<double>(simCount));
    }

	// ���� UCB ѡ������ӽڵ�
	TreeNode* getBest(double c) noexcept
	{
		return *(std::max_element(children.begin(), children.end(), Compare(c)));
	}

	// ѡ���ӽڵ�
	TreeNode* select(Board& board, Player player) noexcept
    {
        if (unmoveList.empty() && children.empty())
            return this;
		
        if (!unmoveList.empty())
        {
			// ���ѡ��Ϸ������ӵ�
            auto selectedMove = unmoveList.back();
            unmoveList.pop_back();

            board.setMove(selectedMove, player);

            auto pSelected = new TreeNode(this, selectedMove, player, board);
			children.emplace_back(pSelected);

			return pSelected;
        }

		// ���޺Ϸ����ӵ㣬������Žڵ������չ
        auto pBest = *(std::max_element(children.begin(), children.end(), Compare(0.5)));
        board.setMove(pBest->move, player);
		return pBest->select(board, nextPlayer(player));
    }

	// ���򴫲�
    void reachBack(Player winner) noexcept
    {
        TreeNode* pNode = this;
        while (pNode != nullptr)
        {
            ++pNode->simCount;
            if (winner == pNode->player)
                ++pNode->winCount;
            pNode = pNode->pParent;
        }
    }

private:
    TreeNode* pParent{ nullptr };
    std::vector<TreeNode*> children;

    Coord move;                    // ��ǰ�ڵ��Ӧ������
    std::vector<Coord> unmoveList;

    Player player;

    // ģ�������Ϣ
    int winCount{ 0 };
    int simCount{ 0 };
};
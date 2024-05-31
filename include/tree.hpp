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

        // 对合法的落子点进行随机打乱
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

	// 根据 UCB 选出最佳子节点
	TreeNode* getBest(double c) noexcept
	{
		return *(std::max_element(children.begin(), children.end(), Compare(c)));
	}

	// 选举子节点
	TreeNode* select(Board& board, Player player) noexcept
    {
        if (unmoveList.empty() && children.empty())
            return this;
		
        if (!unmoveList.empty())
        {
			// 随机选择合法的落子点
            auto selectedMove = unmoveList.back();
            unmoveList.pop_back();

            board.setMove(selectedMove, player);

            auto pSelected = new TreeNode(this, selectedMove, player, board);
			children.emplace_back(pSelected);

			return pSelected;
        }

		// 若无合法落子点，则对最优节点继续扩展
        auto pBest = *(std::max_element(children.begin(), children.end(), Compare(0.5)));
        board.setMove(pBest->move, player);
		return pBest->select(board, nextPlayer(player));
    }

	// 反向传播
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

    Coord move;                    // 当前节点对应的落子
    std::vector<Coord> unmoveList;

    Player player;

    // 模拟相关信息
    int winCount{ 0 };
    int simCount{ 0 };
};
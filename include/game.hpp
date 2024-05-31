#pragma once

#include <iostream>

#include "tree.hpp"
#include "simulation.hpp"

class Game
{
public:
    Game() = default;

    // 读取目前的棋盘信息
    void read()
    {
        // 初始化：本方 1，对方 -1，空白 0
        std::array<std::array<Player, boardSize>, boardSize> boardInfo;
        for (auto& row : boardInfo)
            row.fill(Player::Empty);

        int x, y, n;

        std::cin >> n;

        for (int i = 0; i < n - 1; ++i) {
            std::cin >> x >> y; if (x != -1) boardInfo[x][y] = Player::Opponent;    // 对方

            // 判断先手
            if (i == 0)
            {
                if (x == -1) red = Player::Self;
                else         red = Player::Opponent;
            }

            std::cin >> x >> y; if (x != -1) boardInfo[x][y] = Player::Self;	    // 我方
        }
        std::cin >> x >> y;
        if (x != -1) boardInfo[x][y] = Player::Opponent;    // 对方

        // 初始化棋盘信息
        board.init(boardInfo, Coord(x, y));
    }

    // 决策部分代码
    Coord judge()
    {
        if (pTreeRoot == nullptr)
            pTreeRoot = new TreeNode(nullptr, board.getRecent(), Player::Opponent, board);

        // 固定时间内模拟
        using namespace std::chrono;
        auto beginTime = high_resolution_clock::now();
        while (true)
        {
            // 选举或扩展子节点
            auto simBoard = board;
            auto pSelected = pTreeRoot->select(simBoard, Player::Self);

            for (int i = 0; i < simCount; ++i)
            {
                // 蒙特卡洛模拟
                auto winner = Simulation(simBoard, red).start(pSelected->getMove(), nextPlayer(pSelected->getPlayer()));

                // 反向传播模拟结果
                pSelected->reachBack(winner);
            }

            // 判断是否超时
            auto endTime = high_resolution_clock::now();
            auto elapsed = duration_cast<duration<double, std::ratio<1>>>(endTime - beginTime);
            if (elapsed.count() > 0.95)
                break;
        }

        // 选出最佳落子
        return pTreeRoot->getBest(0)->getMove();
    }

    // 在下面填充你的代码，决策结果（本方将落子的位置）作为返回值
    Coord execute()
    {
        // 先手 B3
        if (board.getRecent().x == -1)
            return Coord(1, 2);

		// 若非先手，则第一步下中间
		if (board.getMoveList().size() == 1)
			return Coord(5, 5);

		// 第二步
		//if (board.getMoveList().size() == 2)
		//{
		//	// 对方不下中间
		//	if (board(5, 5) == Player::Empty)
		//	{
		//		return Coord(5, 5);
		//	}
		//	else
		//	{
		//		return Coord(6, 7);
		//	}
		//}

		return judge();
    }

private:
    const int simCount{ 3 };

    Board board;
	Player red;

    TreeNode* pTreeRoot{ nullptr }; // 蒙特卡洛树的根
};
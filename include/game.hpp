#pragma once

#include <iostream>

#include "tree.hpp"
#include "simulation.hpp"

class Game
{
public:
    Game() = default;

    // ��ȡĿǰ��������Ϣ
    void read()
    {
        // ��ʼ�������� 1���Է� -1���հ� 0
        std::array<std::array<Player, boardSize>, boardSize> boardInfo;
        for (auto& row : boardInfo)
            row.fill(Player::Empty);

        int x, y, n;

        std::cin >> n;

        for (int i = 0; i < n - 1; ++i) {
            std::cin >> x >> y; if (x != -1) boardInfo[x][y] = Player::Opponent;    // �Է�

            // �ж�����
            if (i == 0)
            {
                if (x == -1) red = Player::Self;
                else         red = Player::Opponent;
            }

            std::cin >> x >> y; if (x != -1) boardInfo[x][y] = Player::Self;	    // �ҷ�
        }
        std::cin >> x >> y;
        if (x != -1) boardInfo[x][y] = Player::Opponent;    // �Է�

        // ��ʼ��������Ϣ
        board.init(boardInfo, Coord(x, y));
    }

    // ���߲��ִ���
    Coord judge()
    {
        if (pTreeRoot == nullptr)
            pTreeRoot = new TreeNode(nullptr, board.getRecent(), Player::Opponent, board);

        // �̶�ʱ����ģ��
        using namespace std::chrono;
        auto beginTime = high_resolution_clock::now();
        while (true)
        {
            // ѡ�ٻ���չ�ӽڵ�
            auto simBoard = board;
            auto pSelected = pTreeRoot->select(simBoard, Player::Self);

            for (int i = 0; i < simCount; ++i)
            {
                // ���ؿ���ģ��
                auto winner = Simulation(simBoard, red).start(pSelected->getMove(), nextPlayer(pSelected->getPlayer()));

                // ���򴫲�ģ����
                pSelected->reachBack(winner);
            }

            // �ж��Ƿ�ʱ
            auto endTime = high_resolution_clock::now();
            auto elapsed = duration_cast<duration<double, std::ratio<1>>>(endTime - beginTime);
            if (elapsed.count() > 0.95)
                break;
        }

        // ѡ���������
        return pTreeRoot->getBest(0)->getMove();
    }

    // �����������Ĵ��룬���߽�������������ӵ�λ�ã���Ϊ����ֵ
    Coord execute()
    {
        // ���� B3
        if (board.getRecent().x == -1)
            return Coord(1, 2);

		// �������֣����һ�����м�
		if (board.getMoveList().size() == 1)
			return Coord(5, 5);

		// �ڶ���
		//if (board.getMoveList().size() == 2)
		//{
		//	// �Է������м�
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

    TreeNode* pTreeRoot{ nullptr }; // ���ؿ������ĸ�
};
#include "../include/game.hpp"

int main()
{
    Game game;

    game.read();

    auto result = game.execute();

    // ��ƽ̨������߽��
    std::cout << result.x << ' ' << result.y << std::endl;

    return 0;
}
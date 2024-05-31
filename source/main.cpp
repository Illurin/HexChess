#include "../include/game.hpp"

int main()
{
    Game game;

    game.read();

    auto result = game.execute();

    // 向平台输出决策结果
    std::cout << result.x << ' ' << result.y << std::endl;

    return 0;
}
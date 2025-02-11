#ifndef SNAKESTATE_HPP
#define SNAKESTATE_HPP

#include <utility>
#include <vector>
#include <variant>

const int SQUARE_SIZE_PIXELS = 15;

enum class Direction { Up, Down, Left, Right };

struct GameState {
    int rows;
    int cols;
    std::vector<std::pair<int, int>> snake;
    std::pair<int, int> reward;
    Direction currentDirection;
    bool moveMade;
};

struct UserDirectionAction {
    Direction direction;
};

struct RequestNextFrameAction {};

using Action = std::variant<UserDirectionAction, RequestNextFrameAction>;

GameState reduce(const GameState &state, const Action &action);
GameState initializeGameState(int rows, int cols);
void placeReward(GameState &state);

#endif // SNAKESTATE_HPP

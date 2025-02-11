#include <QApplication>
#include <QDebug>
#include <QKeyEvent>
#include <QMainWindow>
#include <QMessageBox>
#include <QOpenGLWidget>
#include <QPainter>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <set>
#include <utility>
#include <variant>
#include <vector>

const int SQUARE_SIZE_PIXELS = 15;

enum class Direction { Up, Down, Left, Right };

struct GameState {
  int rows;
  int cols;
  std::vector<std::pair<int, int>> snake;
  std::pair<int, int> reward;
  Direction currentDirection;
  bool moveMade; // Track if a move has been made in the current time step
};

struct UserDirectionAction {
  Direction direction;
};

struct RequestNextFrameAction {};

using Action = std::variant<UserDirectionAction, RequestNextFrameAction>;

void placeReward(GameState &state) {
  std::srand(std::time(nullptr));

  // Create a set of all possible grid positions
  std::set<std::pair<int, int>> gridPositions;
  for (int i = 0; i < state.rows; ++i) {
    for (int j = 0; j < state.cols; ++j) {
      gridPositions.insert({i, j});
    }
  }

  // Create a set of snake positions
  const std::set<std::pair<int, int>> snakePositions(state.snake.begin(),
                                                     state.snake.end());

  // Calculate the set difference to find available positions
  std::vector<std::pair<int, int>> availablePositions;
  std::set_difference(gridPositions.begin(), gridPositions.end(),
                      snakePositions.begin(), snakePositions.end(),
                      std::back_inserter(availablePositions));

  // Randomly select a position from the available positions
  if (!availablePositions.empty()) {
    const int index = std::rand() % availablePositions.size();
    state.reward = availablePositions[index];
  }
}

class ActionVisitor {
public:
  GameState operator()(const UserDirectionAction &action,
                       const GameState &state) const {
    GameState newState = state;
    if (newState.moveMade)
      return newState;
    if ((action.direction == Direction::Up &&
         newState.currentDirection != Direction::Down) ||
        (action.direction == Direction::Down &&
         newState.currentDirection != Direction::Up) ||
        (action.direction == Direction::Left &&
         newState.currentDirection != Direction::Right) ||
        (action.direction == Direction::Right &&
         newState.currentDirection != Direction::Left)) {
      newState.currentDirection = action.direction;
      newState.moveMade = true;
    }
    return newState;
  }

  GameState operator()(const RequestNextFrameAction &,
                       const GameState &state) const {
    GameState newState = state;
    auto head = newState.snake.front();
    switch (newState.currentDirection) {
    case Direction::Up:
      head.first = (head.first - 1 + newState.rows) % newState.rows;
      break;
    case Direction::Down:
      head.first = (head.first + 1) % newState.rows;
      break;
    case Direction::Left:
      head.second = (head.second - 1 + newState.cols) % newState.cols;
      break;
    case Direction::Right:
      head.second = (head.second + 1) % newState.cols;
      break;
    }

    // Check for self-intersection
    if (std::find(newState.snake.begin(), newState.snake.end(), head) !=
        newState.snake.end()) {
      const int score = newState.snake.size();
      QMessageBox::information(nullptr, "Game Over",
                               QString("Game Over! Your score: %1").arg(score));
      QApplication::quit();
      return newState;
    }

    newState.snake.insert(newState.snake.begin(), head);

    // Check if the snake has eaten the reward
    if (head == newState.reward) {
      placeReward(newState);
    } else {
      newState.snake.pop_back(); // Remove the tail if no reward is eaten
    }

    newState.moveMade = false; // Reset moveMade for the next time step
    return newState;
  }
};

GameState reduce(const GameState &state, const Action &action) {
  return std::visit(
      [&state](auto &&arg) -> GameState { return ActionVisitor{}(arg, state); },
      action);
}

GameState initializeGameState(int rows, int cols) {
  GameState state = {rows, cols, {}, {0, 0}, Direction::Right, false};
  const int centerRow = rows / 2;
  const int centerCol = cols / 2;
  state.snake.push_back({centerRow, centerCol});
  state.snake.push_back({centerRow, centerCol - 1});
  state.snake.push_back({centerRow, centerCol - 2});
  placeReward(state); // Initial reward placement
  return state;
}

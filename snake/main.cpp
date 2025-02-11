#include <QApplication>
#include <QOpenGLWidget>
#include <QPainter>
#include <QMainWindow>
#include <vector>
#include <utility>
#include <QKeyEvent>
#include <QDebug>
#include <cstdlib>
#include <ctime>
#include <QMessageBox>

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

const int SQUARE_SIZE = 15; // Each square is 10x10 pixels

GameState reduce(const GameState& state, const UserDirectionAction& action) {
    GameState newState = state;
    if (newState.moveMade) return newState;
    if ((action.direction == Direction::Up && newState.currentDirection != Direction::Down) ||
        (action.direction == Direction::Down && newState.currentDirection != Direction::Up) ||
        (action.direction == Direction::Left && newState.currentDirection != Direction::Right) ||
        (action.direction == Direction::Right && newState.currentDirection != Direction::Left)) {
        newState.currentDirection = action.direction;
        newState.moveMade = true;
    }
    return newState;
}

GameState reduce(const GameState& state, const RequestNextFrameAction&) {
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
    if (std::find(newState.snake.begin(), newState.snake.end(), head) != newState.snake.end()) {
        int score = newState.snake.size();
        QMessageBox::information(nullptr, "Game Over", QString("Game Over! Your score: %1").arg(score));
        QApplication::quit();
        return newState;
    }

    newState.snake.insert(newState.snake.begin(), head);

    // Check if the snake has eaten the reward
    if (head == newState.reward) {
        std::srand(std::time(nullptr));
        do {
            int rewardRow = std::rand() % newState.rows;
            int rewardCol = std::rand() % newState.cols;
            newState.reward = {rewardRow, rewardCol};
        } while (std::find(newState.snake.begin(), newState.snake.end(), newState.reward) != newState.snake.end());
    } else {
        newState.snake.pop_back(); // Remove the tail if no reward is eaten
    }

    newState.moveMade = false; // Reset moveMade for the next time step
    return newState;
}

class SnakeWidget : public QOpenGLWidget {
public:
    SnakeWidget(int rows, int cols, QWidget *parent = nullptr) 
        : QOpenGLWidget(parent), state_({rows, cols, {}, {0, 0}, Direction::Right, false}) {
        setFocusPolicy(Qt::StrongFocus); // Ensure the widget can catch input
        initializeSnake_();
        placeReward_();
    }

protected:
    void initializeGL() override {}

    void paintGL() override {
        QPainter painter(this);

        // Draw the grid
        for (int i = 0; i < state_.rows; ++i) {
            for (int j = 0; j < state_.cols; ++j) {
                const QRect rect(j * SQUARE_SIZE, i * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE);
                painter.setBrush(QColor(200, 200, 255)); // Light blue background
                painter.drawRect(rect);
            }
        }

        // Draw the snake
        for (size_t i = 0; i < state_.snake.size(); ++i) {
            const auto& segment = state_.snake[i];
            const QRect rect(segment.second * SQUARE_SIZE, segment.first * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE);
            if (i == 0) {
                // Draw the head with a different color and direction indicator
                painter.setBrush(QColor(255, 255, 0)); // Yellow color for the head
                painter.drawRect(rect);

                // Draw half of the head in the direction of movement
                QRect halfRect;
                painter.setBrush(QColor(0, 0, 0)); // Black color for the direction indicator
                switch (state_.currentDirection) {
                    case Direction::Up:
                        halfRect = QRect(rect.left(), rect.top(), rect.width(), rect.height() / 2);
                        break;
                    case Direction::Down:
                        halfRect = QRect(rect.left(), rect.center().y(), rect.width(), rect.height() / 2);
                        break;
                    case Direction::Left:
                        halfRect = QRect(rect.left(), rect.top(), rect.width() / 2, rect.height());
                        break;
                    case Direction::Right:
                        halfRect = QRect(rect.center().x(), rect.top(), rect.width() / 2, rect.height());
                        break;
                }
                painter.drawRect(halfRect);
            } else {
                painter.setBrush(QColor(0, 255, 0)); // Green color for the snake body
                painter.drawRect(rect);
            }
        }

        // Draw the reward
        painter.setBrush(QColor(255, 0, 0)); // Red color for the reward
        const QRect rewardRect(state_.reward.second * SQUARE_SIZE, state_.reward.first * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE);
        painter.drawRect(rewardRect);
    }

    void keyPressEvent(QKeyEvent *event) override {
        if (state_.moveMade) return; // Only allow one move per time step
        UserDirectionAction action;
        switch (event->key()) {
            case Qt::Key_Up:
                action.direction = Direction::Up;
                break;
            case Qt::Key_Down:
                action.direction = Direction::Down;
                break;
            case Qt::Key_Left:
                action.direction = Direction::Left;
                break;
            case Qt::Key_Right:
                action.direction = Direction::Right;
                break;
            default:
                return;
        }
        state_ = reduce(state_, action);
    }

    void timerEvent(QTimerEvent *event) override {
        RequestNextFrameAction request;
        state_ = reduce(state_, request);
        update();
    }

private:
    GameState state_;

    void initializeSnake_() {
        int centerRow = state_.rows / 2;
        int centerCol = state_.cols / 2;
        state_.snake.push_back({centerRow, centerCol});
        state_.snake.push_back({centerRow, centerCol - 1});
        state_.snake.push_back({centerRow, centerCol - 2});
        startTimer(100); // Start a timer to move the snake every 100ms
    }

    void placeReward_() {
        std::srand(std::time(nullptr));
        do {
            int rewardRow = std::rand() % state_.rows;
            int rewardCol = std::rand() % state_.cols;
            state_.reward = {rewardRow, rewardCol};
        } while (std::find(state_.snake.begin(), state_.snake.end(), state_.reward) != state_.snake.end());
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    constexpr int rows = 25; // Example number of rows
    constexpr int cols = 25; // Example number of columns

    SnakeWidget *widget = new SnakeWidget(rows, cols);
    widget->resize(cols * SQUARE_SIZE, rows * SQUARE_SIZE); // Set the widget size based on grid size

    QMainWindow mainWindow;
    mainWindow.setCentralWidget(widget);
    mainWindow.setFixedSize(widget->size());
    mainWindow.setWindowTitle("Snake Game");
    mainWindow.show();

    return app.exec();
}

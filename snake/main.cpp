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

struct Action {
    Direction direction;
};

struct RequestNextStep {};

const int SQUARE_SIZE = 15; // Each square is 10x10 pixels

class SnakeWidget : public QOpenGLWidget {
public:
    SnakeWidget(int rows, int cols, QWidget *parent = nullptr) 
        : QOpenGLWidget(parent), state({rows, cols, {}, {0, 0}, Direction::Right, false}) {
        setFocusPolicy(Qt::StrongFocus); // Ensure the widget can catch input
        initializeSnake();
        placeReward();
    }

protected:
    void initializeGL() override {}

    void paintGL() override {
        QPainter painter(this);

        // Draw the grid
        for (int i = 0; i < state.rows; ++i) {
            for (int j = 0; j < state.cols; ++j) {
                const QRect rect(j * SQUARE_SIZE, i * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE);
                painter.setBrush(QColor(200, 200, 255)); // Light blue background
                painter.drawRect(rect);
            }
        }

        // Draw the snake
        for (size_t i = 0; i < state.snake.size(); ++i) {
            const auto& segment = state.snake[i];
            const QRect rect(segment.second * SQUARE_SIZE, segment.first * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE);
            if (i == 0) {
                // Draw the head with a different color and direction indicator
                painter.setBrush(QColor(255, 255, 0)); // Yellow color for the head
                painter.drawRect(rect);
                painter.setBrush(QColor(0, 0, 0)); // Black color for the direction indicator
                switch (state.currentDirection) {
                    case Direction::Up:
                        painter.drawLine(rect.center(), QPoint(rect.center().x(), rect.top()));
                        break;
                    case Direction::Down:
                        painter.drawLine(rect.center(), QPoint(rect.center().x(), rect.bottom()));
                        break;
                    case Direction::Left:
                        painter.drawLine(rect.center(), QPoint(rect.left(), rect.center().y()));
                        break;
                    case Direction::Right:
                        painter.drawLine(rect.center(), QPoint(rect.right(), rect.center().y()));
                        break;
                }
            } else {
                painter.setBrush(QColor(0, 255, 0)); // Green color for the snake body
                painter.drawRect(rect);
            }
        }

        // Draw the reward
        painter.setBrush(QColor(255, 0, 0)); // Red color for the reward
        const QRect rewardRect(state.reward.second * SQUARE_SIZE, state.reward.first * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE);
        painter.drawRect(rewardRect);
    }

    void keyPressEvent(QKeyEvent *event) override {
        if (state.moveMade) return; // Only allow one move per time step
        Action action;
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
        processAction(action);
    }

    void timerEvent(QTimerEvent *event) override {
        RequestNextStep request;
        processRequest(request);
        update();
    }

private:
    GameState state;

    void initializeSnake() {
        int centerRow = state.rows / 2;
        int centerCol = state.cols / 2;
        state.snake.push_back({centerRow, centerCol});
        state.snake.push_back({centerRow, centerCol - 1});
        state.snake.push_back({centerRow, centerCol - 2});
        startTimer(100); // Start a timer to move the snake every 100ms
    }

    void placeReward() {
        std::srand(std::time(nullptr));
        do {
            int rewardRow = std::rand() % state.rows;
            int rewardCol = std::rand() % state.cols;
            state.reward = {rewardRow, rewardCol};
        } while (std::find(state.snake.begin(), state.snake.end(), state.reward) != state.snake.end());
    }

    void processAction(const Action& action) {
        if (state.moveMade) return;
        if ((action.direction == Direction::Up && state.currentDirection != Direction::Down) ||
            (action.direction == Direction::Down && state.currentDirection != Direction::Up) ||
            (action.direction == Direction::Left && state.currentDirection != Direction::Right) ||
            (action.direction == Direction::Right && state.currentDirection != Direction::Left)) {
            state.currentDirection = action.direction;
            state.moveMade = true;
        }
    }

    void processRequest(const RequestNextStep&) {
        moveSnake();
        state.moveMade = false; // Reset moveMade for the next time step
    }

    void moveSnake() {
        auto head = state.snake.front();
        switch (state.currentDirection) {
            case Direction::Up:
                head.first = (head.first - 1 + state.rows) % state.rows;
                break;
            case Direction::Down:
                head.first = (head.first + 1) % state.rows;
                break;
            case Direction::Left:
                head.second = (head.second - 1 + state.cols) % state.cols;
                break;
            case Direction::Right:
                head.second = (head.second + 1) % state.cols;
                break;
        }

        // Check for self-intersection
        if (std::find(state.snake.begin(), state.snake.end(), head) != state.snake.end()) {
            endGame();
            return;
        }

        state.snake.insert(state.snake.begin(), head);

        // Check if the snake has eaten the reward
        if (head == state.reward) {
            placeReward(); // Place a new reward
        } else {
            state.snake.pop_back(); // Remove the tail if no reward is eaten
        }
    }

    void endGame() {
        int score = state.snake.size();
        QMessageBox::information(this, "Game Over", QString("Game Over! Your score: %1").arg(score));
        QApplication::quit();
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

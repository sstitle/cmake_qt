#include <QApplication>
#include <QOpenGLWidget>
#include <QPainter>
#include <QMainWindow>
#include <vector>
#include <utility>
#include <QKeyEvent>
#include <QDebug>

enum class Direction { Up, Down, Left, Right };

struct GameState {
    int rows;
    int cols;
    std::vector<std::pair<int, int>> snake;
    Direction currentDirection;
};

class SnakeWidget : public QOpenGLWidget {
public:
    SnakeWidget(int rows, int cols, QWidget *parent = nullptr) 
        : QOpenGLWidget(parent), state({rows, cols, {}, Direction::Right}) {
        setFocusPolicy(Qt::StrongFocus); // Ensure the widget can catch input
        initializeSnake();
    }

protected:
    void initializeGL() override {}

    void paintGL() override {
        QPainter painter(this);
        const int squareSize = 10; // Each square is 10x10 pixels

        // Draw the grid
        for (int i = 0; i < state.rows; ++i) {
            for (int j = 0; j < state.cols; ++j) {
                const QRect rect(j * squareSize, i * squareSize, squareSize, squareSize);
                painter.setBrush(QColor(200, 200, 255)); // Light blue background
                painter.drawRect(rect);
            }
        }

        // Draw the snake
        painter.setBrush(QColor(0, 255, 0)); // Green color for the snake
        for (const auto& segment : state.snake) {
            const QRect rect(segment.second * squareSize, segment.first * squareSize, squareSize, squareSize);
            painter.drawRect(rect);
        }
    }

    void keyPressEvent(QKeyEvent *event) override {
        qDebug() << "Key Pressed:" << event->key(); // Log the key press for debugging
        switch (event->key()) {
            case Qt::Key_Up:
                if (state.currentDirection != Direction::Down) state.currentDirection = Direction::Up;
                break;
            case Qt::Key_Down:
                if (state.currentDirection != Direction::Up) state.currentDirection = Direction::Down;
                break;
            case Qt::Key_Left:
                if (state.currentDirection != Direction::Right) state.currentDirection = Direction::Left;
                break;
            case Qt::Key_Right:
                if (state.currentDirection != Direction::Left) state.currentDirection = Direction::Right;
                break;
        }
    }

    void timerEvent(QTimerEvent *event) override {
        moveSnake();
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
        state.snake.insert(state.snake.begin(), head);
        state.snake.pop_back(); // Remove the tail
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    int rows = 20; // Example number of rows
    int cols = 20; // Example number of columns

    SnakeWidget *widget = new SnakeWidget(rows, cols);
    widget->resize(cols * 10, rows * 10); // Set the widget size based on grid size

    QMainWindow mainWindow;
    mainWindow.setCentralWidget(widget);
    mainWindow.setFixedSize(widget->size());
    mainWindow.setWindowTitle("Snake Game");
    mainWindow.show();

    return app.exec();
}

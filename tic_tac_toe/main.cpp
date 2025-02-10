#include <QApplication>
#include <QOpenGLWidget>
#include <QPainter>
#include <QMainWindow>
#include <QMouseEvent>
#include <QMessageBox>
#include <array>
#include <variant>

enum class ActionType {
    PlaceMarker,
    ResetGame
};

struct Action {
    ActionType type;
    int index; // Used for PlaceMarker action
};

struct State {
    std::array<char, 9> board;
    char currentPlayer;
};

bool checkWin(const std::array<char, 9>& board) {
    const int winPatterns[8][3] = {
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, // Rows
        {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, // Columns
        {0, 4, 8}, {2, 4, 6}             // Diagonals
    };

    for (const auto& pattern : winPatterns) {
        if (board[pattern[0]] != ' ' &&
            board[pattern[0]] == board[pattern[1]] &&
            board[pattern[1]] == board[pattern[2]]) {
            return true;
        }
    }
    return false;
}

bool checkTie(const std::array<char, 9>& board) {
    return std::all_of(board.begin(), board.end(), [](char c) { return c != ' '; });
}

State reducer(const State& state, const Action& action) {
    State newState = state;

    switch (action.type) {
        case ActionType::PlaceMarker:
            if (newState.board[action.index] == ' ') {
                newState.board[action.index] = newState.currentPlayer;
                newState.currentPlayer = (newState.currentPlayer == 'X') ? 'O' : 'X';
            }
            break;
        case ActionType::ResetGame:
            newState.board.fill(' ');
            break;
    }

    return newState;
}

void checkGameOver(State& state) {
    if (checkWin(state.board)) {
        QMessageBox::information(nullptr, "Game Over", QString("%1 Wins!").arg(state.currentPlayer == 'X' ? 'O' : 'X'));
        state = reducer(state, {ActionType::ResetGame, 0});
    } else if (checkTie(state.board)) {
        QMessageBox::information(nullptr, "Game Over", "It's a Tie!");
        state = reducer(state, {ActionType::ResetGame, 0});
    }
}

class TicTacToeWidget : public QOpenGLWidget {
public:
    TicTacToeWidget(QWidget *parent = nullptr) : QOpenGLWidget(parent) {
        state = {std::array<char, 9>{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, 'X'};
    }

protected:
    void initializeGL() override {
        // Initialize OpenGL settings if needed
    }

    void paintGL() override {
        QPainter painter(this);
        int squareSize = width() / 3;
        QFont font("Arial", squareSize * 0.5, QFont::Bold);
        painter.setFont(font);

        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                QRect rect(j * squareSize, i * squareSize, squareSize, squareSize);
                painter.setBrush(QColor(200, 200, 255)); // Light blue background
                painter.drawRect(rect);

                if (state.board[i * 3 + j] != ' ') {
                    if (state.board[i * 3 + j] == 'X') {
                        painter.setPen(QColor(255, 100, 100)); // Light red text for X
                    } else {
                        painter.setPen(QColor(100, 255, 100)); // Light green text for O
                    }
                    painter.drawText(rect, Qt::AlignCenter, QString(state.board[i * 3 + j]));
                }
            }
        }

        // Draw black lines between grid cells
        painter.setPen(QColor(0, 0, 0)); // Black color for lines
        for (int i = 1; i < 3; ++i) {
            painter.drawLine(i * squareSize, 0, i * squareSize, height()); // Vertical lines
            painter.drawLine(0, i * squareSize, width(), i * squareSize); // Horizontal lines
        }
    }

    void mousePressEvent(QMouseEvent *event) override {
        int squareSize = width() / 3;
        int row = event->position().y() / squareSize;
        int col = event->position().x() / squareSize;

        if (row < 3 && col < 3) {
            int index = row * 3 + col;
            state = reducer(state, {ActionType::PlaceMarker, index});
            update();
            checkGameOver(state);
            updateWindowTitle();
        }
    }

private:
    State state;

    void updateWindowTitle() {
        QString title = QString("Tic Tac Toe - %1's Turn").arg(state.currentPlayer);
        window()->setWindowTitle(title);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    TicTacToeWidget *widget = new TicTacToeWidget;
    widget->resize(300, 300);

    QMainWindow mainWindow;
    mainWindow.setCentralWidget(widget);
    mainWindow.setFixedSize(widget->size());
    mainWindow.setWindowTitle("Tic Tac Toe - X's Turn");
    mainWindow.show();

    return app.exec();
}

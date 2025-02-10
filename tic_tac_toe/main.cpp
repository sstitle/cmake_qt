#include <QApplication>
#include <QOpenGLWidget>
#include <QPainter>
#include <QMainWindow>
#include <QMouseEvent>
#include <QMessageBox>
#include <bitset>
#include <variant>

enum class Player : char { None = ' ', X = 'X', O = 'O' };

struct PlaceMarkerAction {
    const unsigned int index;
};

struct ResetGameAction {};

using Action = std::variant<PlaceMarkerAction, ResetGameAction>;

struct State {
    std::bitset<9> xBoard;
    std::bitset<9> oBoard;
    Player currentPlayer;
};

bool checkWin(const std::bitset<9>& board) {
    const std::array<int, 8> winPatterns = {
        0b111000000, 0b000111000, 0b000000111, // Rows
        0b100100100, 0b010010010, 0b001001001, // Columns
        0b100010001, 0b001010100              // Diagonals
    };
    return std::any_of(winPatterns.begin(), winPatterns.end(), [&board](const int pattern) {
        return (board.to_ulong() & pattern) == pattern;
    });
}

bool checkTie(const State& state) {
    return (state.xBoard | state.oBoard).count() == 9;
}

State reducer(const State& state, const Action& action) {
    State newState = state;

    std::visit([&newState](auto&& act) {
        using T = std::decay_t<decltype(act)>;
        if constexpr (std::is_same_v<T, PlaceMarkerAction>) {
            if (!newState.xBoard[act.index] && !newState.oBoard[act.index]) {
                if (newState.currentPlayer == Player::X) {
                    newState.xBoard.set(act.index);
                    newState.currentPlayer = Player::O;
                } else {
                    newState.oBoard.set(act.index);
                    newState.currentPlayer = Player::X;
                }
            }
        } else if constexpr (std::is_same_v<T, ResetGameAction>) {
            newState.xBoard.reset();
            newState.oBoard.reset();
            newState.currentPlayer = Player::X;
        }
    }, action);

    return newState;
}

void showGameOverMessage(const QString& message) {
    QMessageBox::warning(nullptr, "Game Over", message);
}

void checkGameOver(State& state) {
    if (checkWin(state.xBoard)) {
        showGameOverMessage("X Wins!");
        state = reducer(state, ResetGameAction{});
    } else if (checkWin(state.oBoard)) {
        showGameOverMessage("O Wins!");
        state = reducer(state, ResetGameAction{});
    } else if (checkTie(state)) {
        showGameOverMessage("It's a Tie!");
        state = reducer(state, ResetGameAction{});
    }
}

class TicTacToeWidget : public QOpenGLWidget {
public:
    TicTacToeWidget(QWidget *parent = nullptr) : QOpenGLWidget(parent) {
        state = {std::bitset<9>(), std::bitset<9>(), Player::X};
    }

protected:
    void initializeGL() override {}

    void paintGL() override {
        QPainter painter(this);
        const int squareSize = width() / 3;
        const QFont font("Arial", squareSize * 0.5, QFont::Bold);
        painter.setFont(font);

        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                const QRect rect(j * squareSize, i * squareSize, squareSize, squareSize);
                painter.setBrush(QColor(200, 200, 255)); // Light blue background
                painter.drawRect(rect);

                const unsigned int index = i * 3 + j;
                if (state.xBoard[index]) {
                    painter.setPen(QColor(255, 100, 100));
                    painter.drawText(rect, Qt::AlignCenter, "X");
                } else if (state.oBoard[index]) {
                    painter.setPen(QColor(100, 255, 100));
                    painter.drawText(rect, Qt::AlignCenter, "O");
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
        const int squareSize = width() / 3;
        const int row = event->position().y() / squareSize;
        const int col = event->position().x() / squareSize;

        if (row < 3 && col < 3) {
            const unsigned int index = row * 3 + col;
            state = reducer(state, PlaceMarkerAction{index});
            update();
            checkGameOver(state);
            updateWindowTitle();
        }
    }

private:
    State state;

    void updateWindowTitle() {
        const QString title = QString("Tic Tac Toe - %1's Turn").arg(state.currentPlayer == Player::X ? 'X' : 'O');
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

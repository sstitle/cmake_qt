#include "SnakeState.hpp"
#include "SnakeWidget.hpp"
#include <QApplication>
#include <QMainWindow>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  constexpr int rows = 25; // Example number of rows
  constexpr int cols = 25; // Example number of columns

  const GameState initialState = initializeGameState(rows, cols);

  auto *widget = new SnakeWidget(initialState);
  widget->resize(
      cols * SQUARE_SIZE_PIXELS,
      rows * SQUARE_SIZE_PIXELS); // Set the widget size based on grid size

  QMainWindow mainWindow;
  mainWindow.setCentralWidget(widget);
  mainWindow.setFixedSize(widget->size());
  mainWindow.setWindowTitle("Snake Game");
  mainWindow.show();

  return app.exec();
}

#include <QPainter>
#include <QKeyEvent>
#include <QMessageBox>
#include <QApplication>


#include "SnakeWidget.hpp"

SnakeWidget::SnakeWidget(const GameState &initialState, QWidget *parent)
    : QOpenGLWidget(parent), state_(initialState) {
  setFocusPolicy(Qt::StrongFocus); // Ensure the widget can catch input
  startTimer(100); // Start a timer to move the snake every 100ms
}

void SnakeWidget::initializeGL() {}

void SnakeWidget::paintGL() {
  QPainter painter(this);

  // Draw the grid
  for (int i = 0; i < state_.rows; ++i) {
    for (int j = 0; j < state_.cols; ++j) {
      const QRect rect(j * SQUARE_SIZE_PIXELS, i * SQUARE_SIZE_PIXELS,
                       SQUARE_SIZE_PIXELS, SQUARE_SIZE_PIXELS);
      painter.setBrush(QColor(200, 200, 255)); // Light blue background
      painter.drawRect(rect);
    }
  }

  // Draw the snake
  for (size_t i = 0; i < state_.snake.size(); ++i) {
    const auto &segment = state_.snake[i];
    const QRect rect(segment.second * SQUARE_SIZE_PIXELS,
                     segment.first * SQUARE_SIZE_PIXELS, SQUARE_SIZE_PIXELS,
                     SQUARE_SIZE_PIXELS);
    if (i == 0) {
      // Draw the head with a different color and direction indicator
      painter.setBrush(QColor(255, 255, 0)); // Yellow color for the head
      painter.drawRect(rect);

      // Draw half of the head in the direction of movement
      QRect halfRect;
      painter.setBrush(
          QColor(0, 0, 0)); // Black color for the direction indicator
      switch (state_.currentDirection) {
      case Direction::Up:
        halfRect =
            QRect(rect.left(), rect.top(), rect.width(), rect.height() / 2);
        break;
      case Direction::Down:
        halfRect = QRect(rect.left(), rect.center().y(), rect.width(),
                         rect.height() / 2);
        break;
      case Direction::Left:
        halfRect =
            QRect(rect.left(), rect.top(), rect.width() / 2, rect.height());
        break;
      case Direction::Right:
        halfRect = QRect(rect.center().x(), rect.top(), rect.width() / 2,
                         rect.height());
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
  const QRect rewardRect(state_.reward.second * SQUARE_SIZE_PIXELS,
                         state_.reward.first * SQUARE_SIZE_PIXELS,
                         SQUARE_SIZE_PIXELS, SQUARE_SIZE_PIXELS);
  painter.drawRect(rewardRect);
}

void SnakeWidget::keyPressEvent(QKeyEvent *event) {
  if (state_.moveMade)
    return; // Only allow one move per time step
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

void SnakeWidget::timerEvent(QTimerEvent *event) {
  RequestNextFrameAction request;
  state_ = reduce(state_, request);
  update();
}

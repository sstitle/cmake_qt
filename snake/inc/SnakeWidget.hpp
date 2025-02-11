#ifndef SNAKEWIDGET_HPP
#define SNAKEWIDGET_HPP

#include <QOpenGLWidget>
#include "SnakeState.hpp"

class SnakeWidget : public QOpenGLWidget {
public:
  SnakeWidget(const GameState &initialState, QWidget *parent = nullptr);

protected:
  void initializeGL() override;
  void paintGL() override;
  void keyPressEvent(QKeyEvent *event) override;
  void timerEvent(QTimerEvent *event) override;

private:
  GameState state_;
};

#endif // SNAKEWIDGET_HPP

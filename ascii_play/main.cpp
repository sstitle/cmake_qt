#include <QApplication>
#include <QFont>
#include <QMainWindow>
#include <QOpenGLWidget>
#include <QPainter>
#include <QTimer>
#include <cstdlib>
#include <ctime>
#include <vector>

class AsciiWidget : public QOpenGLWidget {

public:
  AsciiWidget(QWidget *parent = nullptr) : QOpenGLWidget(parent) {
    srand(static_cast<unsigned int>(
        time(nullptr))); // Seed for random color generation
    generateAsciiData(); // Initial generation of ASCII data
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &AsciiWidget::updateAsciiData);
    timer->start(100);
  }

protected:
  void initializeGL() override {
    // Initialize OpenGL settings if needed
  }

  void paintGL() override {
    QPainter painter(this);
    int squareSize = 50;
    QFont font("Monospace", squareSize * 0.8); // Font size close to square size
    font.setStyleHint(QFont::TypeWriter);
    painter.setFont(font);

    for (int i = 0; i < rows; ++i) {
      for (int j = 0; j < cols; ++j) {
        const auto &data = asciiData[i * cols + j];
        painter.setBrush(
            data.color); // Set the brush to fill the square with color
        painter.setPen(Qt::NoPen); // No border for the square

        QRect rect(j * squareSize, i * squareSize, squareSize, squareSize);
        painter.drawRect(rect);

        painter.setPen(Qt::white); // Set pen color to white for text
        painter.drawText(
            rect, Qt::AlignCenter,
            QString(QChar(rand() % 95 + 32))); // Random ASCII character
      }
    }
  }

private:
  struct AsciiData {
    int asciiCode;
    QColor color;
  };

  std::vector<AsciiData> asciiData;
  const int cols = 8;
  const int rows = 8;
  const int asciiStart = 32; // Starting from space character

  void generateAsciiData() {
    asciiData.clear();
    for (int i = 0; i < rows; ++i) {
      for (int j = 0; j < cols; ++j) {
        int asciiCode = asciiStart + i * cols + j;
        if (asciiCode > 126)
          break; // Stop if beyond printable ASCII

        QColor color(rand() % 256, rand() % 256, rand() % 256);
        asciiData.push_back({asciiCode, color});
      }
    }
  }

  void updateAsciiData() {
    generateAsciiData();
    update(); // Trigger a repaint
  }
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  AsciiWidget *widget = new AsciiWidget;
  widget->resize(400, 400);

  QMainWindow mainWindow;
  mainWindow.setCentralWidget(widget);
  mainWindow.setFixedSize(widget->size());
  mainWindow.show();

  return app.exec();
}

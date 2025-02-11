#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QImage>
#include <QLabel>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QPainter>
#include <QSizePolicy>
#include <QTabWidget>
#include <QTimer>
#include <memory>

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
  std::unique_ptr<QOpenGLShaderProgram> program;
  GLuint vbo, ebo;
  GLuint textures[2];
  float fadeFactor;
  QTimer timer;

public:
  MyGLWidget() : fadeFactor(0.0f) {
    connect(&timer, &QTimer::timeout, this, &MyGLWidget::updateFadeFactor);
    timer.start(8);
  }

protected:
  void initializeGL() override {
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Vertex shader
    const char *vertexShaderSource = R"(
            #version 120
            attribute vec2 position;
            varying vec2 texcoord;
            void main() {
                gl_Position = vec4(position, 0.0, 1.0);
                texcoord = position * vec2(0.5, -0.5) + vec2(0.5, 0.5);
            }
        )";

    // Fragment shader
    const char *fragmentShaderSource = R"(
            #version 120
            uniform float fade_factor;
            uniform sampler2D textures[2];
            varying vec2 texcoord;
            void main() {
                gl_FragColor = mix(
                    texture2D(textures[0], texcoord),
                    texture2D(textures[1], texcoord),
                    fade_factor
                );
            }
        )";

    program = std::make_unique<QOpenGLShaderProgram>();
    if (!program->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                          vertexShaderSource)) {
      qWarning() << "Vertex shader compilation failed:" << program->log();
    }
    if (!program->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                          fragmentShaderSource)) {
      qWarning() << "Fragment shader compilation failed:" << program->log();
    }
    if (!program->link()) {
      qWarning() << "Shader program linking failed:" << program->log();
    }

    // Vertex data for the four corners of the screen
    static const GLfloat g_vertex_buffer_data[] = {-1.0f, -1.0f, 1.0f, -1.0f,
                                                   -1.0f, 1.0f,  1.0f, 1.0f};
    static const GLushort g_element_buffer_data[] = {0, 1, 2, 3};

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data),
                 g_vertex_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_element_buffer_data),
                 g_element_buffer_data, GL_STATIC_DRAW);

    // Create and load textures
    glGenTextures(2, textures);
    createAndLoadTexture(textures[0], ":/hello1.tga");
    createAndLoadTexture(textures[1], ":/hello2.tga");
  }

  void resizeGL(int w, int h) override { glViewport(0, 0, w, h); }

  void paintGL() override {
    glClear(GL_COLOR_BUFFER_BIT);

    program->bind();

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    int posLocation = program->attributeLocation("position");
    program->enableAttributeArray(posLocation);
    glVertexAttribPointer(posLocation, 2, GL_FLOAT, GL_FALSE,
                          2 * sizeof(GLfloat), nullptr);

    program->setUniformValue("fade_factor", fadeFactor);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    program->setUniformValue("textures[0]", 0);
    program->setUniformValue("textures[1]", 1);

    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, nullptr);

    program->disableAttributeArray(posLocation);
    program->release();
  }

private:
  void createAndLoadTexture(GLuint texture, const QString &resourcePath) {
    QImage image;
    if (!image.load(resourcePath)) {
      qWarning() << "Failed to load texture from" << resourcePath;
      return;
    }

    // Convert the image to a format suitable for OpenGL
    QImage glImage = image.convertToFormat(QImage::Format_RGBA8888);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, glImage.width(), glImage.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, glImage.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }

  void updateFadeFactor() {
    static float time = 0.0f;
    fadeFactor = 0.5f * (1.0f + sin(time));
    time += 0.01f;
    update();
  }
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  QTabWidget tabWidget;
  tabWidget.setWindowTitle("Image and GLWidget Viewer");

  // Assuming you have two images to display
  QLabel imageLabel1;
  QPixmap pixmap1(":/hello1.tga");
  imageLabel1.setPixmap(pixmap1.scaled(imageLabel1.size(), Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation));
  imageLabel1.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  imageLabel1.setAlignment(Qt::AlignCenter);
  tabWidget.addTab(&imageLabel1, "Image 1");

  QLabel imageLabel2;
  QPixmap pixmap2(":/hello2.tga");
  imageLabel2.setPixmap(pixmap2.scaled(imageLabel2.size(), Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation));
  imageLabel2.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  imageLabel2.setAlignment(Qt::AlignCenter);
  tabWidget.addTab(&imageLabel2, "Image 2");

  MyGLWidget glWidget;
  glWidget.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  tabWidget.addTab(&glWidget, "GLWidget");
  tabWidget.show();

  return app.exec();
}

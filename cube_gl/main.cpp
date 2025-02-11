#include <QApplication>
#include <QDebug>
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QTimer>
#include <QVector3D>

class CubeGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
  std::unique_ptr<QOpenGLShaderProgram> program;
  GLuint vbo, ebo;
  QMatrix4x4 modelMatrix;
  QTimer timer;
  float angleX, angleY;

public:
  CubeGLWidget() : angleX(0.0f), angleY(0.0f) {
    connect(&timer, &QTimer::timeout, this, &CubeGLWidget::updateRotation);
    timer.start(16); // roughly 60 FPS
  }

protected:
  void initializeGL() override {
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);

    // Vertex shader
    const char *vertexShaderSource = R"(
            #version 120
            attribute vec3 position;
            uniform mat4 model;
            uniform mat4 view;
            uniform mat4 projection;
            void main() {
                gl_Position = projection * view * model * vec4(position, 1.0);
            }
        )";

    // Fragment shader
    const char *fragmentShaderSource = R"(
            #version 120
            void main() {
                gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0); // White color for wireframe
            }
        )";

    program = std::make_unique<QOpenGLShaderProgram>();
    if (!program->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                          vertexShaderSource)) {
      qFatal("Vertex shader compilation failed: %s",
             qPrintable(program->log()));
    }
    if (!program->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                          fragmentShaderSource)) {
      qFatal("Fragment shader compilation failed: %s",
             qPrintable(program->log()));
    }
    if (!program->link()) {
      qFatal("Shader program linking failed: %s", qPrintable(program->log()));
    }

    // Cube vertex data
    static const GLfloat cubeVertices[] = {
        -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f, 1.0f};

    static const GLushort cubeIndices[] = {
        0, 1, 1, 2, 2, 3, 3, 0, // Back face
        4, 5, 5, 6, 6, 7, 7, 4, // Front face
        0, 4, 1, 5, 2, 6, 3, 7  // Connecting edges
    };

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices,
                 GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices,
                 GL_STATIC_DRAW);
  }

  void resizeGL(int w, int h) override { glViewport(0, 0, w, h); }

  void paintGL() override {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program->bind();

    QMatrix4x4 viewMatrix;
    viewMatrix.lookAt(QVector3D(0.0f, 0.0f, 5.0f), QVector3D(0.0f, 0.0f, 0.0f),
                      QVector3D(0.0f, 1.0f, 0.0f));

    QMatrix4x4 projectionMatrix;
    projectionMatrix.perspective(45.0f, float(width()) / height(), 0.1f,
                                 100.0f);

    program->setUniformValue("model", modelMatrix);
    program->setUniformValue("view", viewMatrix);
    program->setUniformValue("projection", projectionMatrix);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    int posLocation = program->attributeLocation("position");
    program->enableAttributeArray(posLocation);
    glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE,
                          3 * sizeof(GLfloat), nullptr);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Set to wireframe mode
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_SHORT, nullptr);

    program->disableAttributeArray(posLocation);
    program->release();
  }

private:
  void updateRotation() {
    angleX += 1.0f;
    angleY += 0.5f;
    if (angleX >= 360.0f)
      angleX -= 360.0f;
    if (angleY >= 360.0f)
      angleY -= 360.0f;
    modelMatrix.setToIdentity();
    modelMatrix.rotate(angleX, QVector3D(1.0f, 0.0f, 0.0f));
    modelMatrix.rotate(angleY, QVector3D(0.0f, 1.0f, 0.0f));
    update();
  }
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  CubeGLWidget cubeWidget;
  cubeWidget.resize(800, 600);
  cubeWidget.show();

  return app.exec();
}

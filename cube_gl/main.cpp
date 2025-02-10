#include <QApplication>
#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QTimer>
#include <QMainWindow>
#include <QTime>
#include <QObject>
#include <QOpenGLFunctions>

class CubeGLWidget : public QOpenGLWidget, protected QOpenGLFunctions { // Inherit from QOpenGLFunctions

public:
    CubeGLWidget(QWidget *parent = nullptr) : QOpenGLWidget(parent), shaderProgram(nullptr) {
        QTimer *timer = new QTimer(this);
        QObject::connect(timer, &QTimer::timeout, this, QOverload<>::of(&CubeGLWidget::update)); // Use QOverload for connect
        timer->start(16); // Approximately 60 FPS
    }

protected:
    void initializeGL() override {
        initializeOpenGLFunctions(); // Initialize OpenGL functions
        glClearColor(0.0, 0.0, 0.0, 1.0);

        // Vertex Shader
        const char *vertexShaderSource = R"(
            #version 120
            attribute vec4 position;
            void main() {
                gl_Position = position;
            }
        )";

        // Fragment Shader
        const char *fragmentShaderSource = R"(
            #version 120
            uniform float iTime;
            uniform vec2 iResolution;
            void mainImage(out vec4 fragColor, in vec2 fragCoord) {
                // Normalized pixel coordinates (from 0 to 1)
                vec2 uv = fragCoord / iResolution.xy;

                // Time varying pixel color
                vec3 col = 0.5 + 0.5 * cos(iTime + uv.xyx + vec3(0, 2, 4));

                // Output to screen
                fragColor = vec4(col, 1.0);
            }
            void main() {
                mainImage(gl_FragColor, gl_FragCoord.xy);
            }
        )";

        shaderProgram = new QOpenGLShaderProgram(this);
        shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
        shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
        shaderProgram->link();
    }

    void resizeGL(int w, int h) override {
        glViewport(0, 0, w, h);
    }

    void paintGL() override {
        glClear(GL_COLOR_BUFFER_BIT);

        shaderProgram->bind();
        shaderProgram->setUniformValue("iTime", float(QTime::currentTime().msecsSinceStartOfDay()) / 1000.0f);
        shaderProgram->setUniformValue("iResolution", QVector2D(width(), height()));

        GLfloat vertices[] = {
            -1.0f, -1.0f,
             1.0f, -1.0f,
            -1.0f,  1.0f,
             1.0f,  1.0f
        };

        GLuint VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glDisableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &VBO);

        shaderProgram->release();
    }

private:
    QOpenGLShaderProgram *shaderProgram;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    CubeGLWidget *widget = new CubeGLWidget;
    widget->resize(400, 400);

    QMainWindow mainWindow;
    mainWindow.setCentralWidget(widget);
    mainWindow.setFixedSize(widget->size());
    mainWindow.show();

    return app.exec();
}




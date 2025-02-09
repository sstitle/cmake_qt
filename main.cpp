#include <QApplication>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    QOpenGLShaderProgram *program;
    GLuint vbo;
public:
    ~MyGLWidget() {
        makeCurrent();
        glDeleteBuffers(1, &vbo);
        delete program;
        doneCurrent();
    }
protected:
    void initializeGL() override {
        initializeOpenGLFunctions();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        // Vertex shader
        const char *vertexShaderSource = R"(
            #version 120
            attribute vec2 position;
            attribute vec3 vertexColor;
            varying vec3 color;
            void main() {
                gl_Position = vec4(position, 0.0, 1.0);
                color = vertexColor;
            }
        )";

        // Fragment shader
        const char *fragmentShaderSource = R"(
            #version 120
            varying vec3 color;
            void main() {
                gl_FragColor = vec4(color, 1.0);
            }
        )";

        program = new QOpenGLShaderProgram();
        if (!program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource)) {
            qWarning() << "Vertex shader compilation failed:" << program->log();
        }
        if (!program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource)) {
            qWarning() << "Fragment shader compilation failed:" << program->log();
        }
        if (!program->link()) {
            qWarning() << "Shader program linking failed:" << program->log();
        }

        // Vertex data
        GLfloat vertices[] = {
            // Positions      // Colors
            0.0f,  1.0f,     1.0f, 0.0f, 0.0f,  // First vertex, red
           -1.0f, -1.0f,     0.0f, 1.0f, 0.0f,  // Second vertex, green
            1.0f, -1.0f,     0.0f, 0.0f, 1.0f   // Third vertex, blue
        };

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    }

    void resizeGL(int w, int h) override {
        glViewport(0, 0, w, h);
    }

    void paintGL() override {
        glClear(GL_COLOR_BUFFER_BIT);

        program->bind();

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        int posLocation = program->attributeLocation("position");
        program->enableAttributeArray(posLocation);
        glVertexAttribPointer(posLocation, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);

        int colorLocation = program->attributeLocation("vertexColor");
        program->enableAttributeArray(colorLocation);
        glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*>(2 * sizeof(GLfloat)));

        glDrawArrays(GL_TRIANGLES, 0, 3);

        program->disableAttributeArray(posLocation);
        program->disableAttributeArray(colorLocation);
        program->release();
    }

};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MyGLWidget glWidget;
    glWidget.resize(400, 300);
    glWidget.show();

    return app.exec();
}

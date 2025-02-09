#include <QApplication>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
protected:
    void initializeGL() override {
        initializeOpenGLFunctions();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    }

    void resizeGL(int w, int h) override {
        glViewport(0, 0, w, h);
    }

    void paintGL() override {
        glClear(GL_COLOR_BUFFER_BIT);

        glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex2f(0.0f, 1.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex2f(-1.0f, -1.0f);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex2f(1.0f, -1.0f);
        glEnd();
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MyGLWidget glWidget;
    glWidget.resize(400, 300);
    glWidget.show();

    return app.exec();
}

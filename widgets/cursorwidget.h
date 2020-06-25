#ifndef CURSORWIDGET_H
#define CURSORWIDGET_H

#include <QOpenGLWidget>

class QOpenGLFunctions;

class CursorWidget : public QOpenGLWidget
{
public:
    CursorWidget();
protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
private:
    QOpenGLFunctions *ogl;
};

#endif // CURSORWIDGET_H

#ifndef WORLDWIDGET_H
#define WORLDWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

class WorldWidget : public QOpenGLWidget
{
public:
    WorldWidget();
protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
private:
    QOpenGLFunctions *ogl;
};

#endif // WORLDWIDGET_H

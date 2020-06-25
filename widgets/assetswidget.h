#ifndef ASSETSWIDGET_H
#define ASSETSWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

class AssetsWidget : public QOpenGLWidget
{
public:
    AssetsWidget();
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
private:
    QOpenGLFunctions *ogl;
};

#endif // ASSETSWIDGET_H

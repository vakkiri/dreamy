#ifndef ASSETSWIDGET_H
#define ASSETSWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>

class AssetsWidget : public QOpenGLWidget
{
public:
    AssetsWidget();
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
private:
    QOpenGLFunctions *ogl;
    QOpenGLTexture *tiles;
};

#endif // ASSETSWIDGET_H

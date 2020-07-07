#ifndef ASSETSWIDGET_H
#define ASSETSWIDGET_H

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <vector>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include "../types/types.h"

class QOpenGLTexture;
class QOpenGLShaderProgram;
class QOpenGLShader;
class QWheelEvent;
class QMouseEvent;
class CursorWidget;

class AssetsWidget : public QOpenGLWidget
{
public:
    AssetsWidget();
    ~AssetsWidget();

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void wheelEvent(QWheelEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    int getSelection();
    void setCursorWidget(CursorWidget *widget);

private:
    void initTiles();
    void initBuffers();
    void updateScale(float newScale);
    int posToTile(int x, int y);

    int selection;

    CursorWidget *cursor_widget;

    QOpenGLFunctions_3_3_Core *ogl;
    QOpenGLTexture *tile_texture;
    QOpenGLBuffer vbo;
    QOpenGLBuffer ibo;
    QOpenGLShaderProgram *program;
    QOpenGLShader *vertex_shader;
    QOpenGLShader *fragment_shader;

    std::vector<TextureData> tiles;

    QMatrix4x4 matrix;
    float scale;
    float minScale;
    float maxScale;

    void checkError(std::string action);
};

#endif // ASSETSWIDGET_H

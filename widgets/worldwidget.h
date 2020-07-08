#ifndef WORLDWIDGET_H
#define WORLDWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLBuffer>
#include <QMatrix4x4>

#include "../types/types.h"

class CursorWidget;
class QOpenGLTexture;
class QOpenGLShaderProgram;
class QOpenGLShader;

class WorldWidget : public QOpenGLWidget
{
public:
    WorldWidget();
    ~WorldWidget();

    void setCursorWidget(CursorWidget *widget);
    void setSnap(int snap);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;


private:
    void initTiles();
    void initBuffers();
    void updateSurface();
    void scaleBy(float amt);

    int snap;

    CursorWidget *cursor_widget;

    QOpenGLFunctions_3_3_Core *ogl;
    QOpenGLTexture *tile_texture;
    QOpenGLTexture *bg_texture;
    QOpenGLBuffer vbo;
    QOpenGLShaderProgram *program;
    QOpenGLShader *vertex_shader;
    QOpenGLShader *fragment_shader;

    std::vector<TileInfo> tiles;
    std::vector<TextureData> tile_info;

    QMatrix4x4 matrix;
    float scale;
    float tx;
    float ty;

    float translation;
    float minScale;
    float maxScale;

    float previewx;
    float previewy;
    float mousex;
    float mousey;

    int num_cursor_tiles;
    void checkError(std::string action);
};

#endif // WORLDWIDGET_H

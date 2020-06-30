#ifndef ASSETSWIDGET_H
#define ASSETSWIDGET_H

#include <QOpenGLWidget>
#include <vector>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include "../types/types.h"

class QOpenGLFunctions;
class QOpenGLTexture;
class QOpenGLShaderProgram;
class QOpenGLShader;

class AssetsWidget : public QOpenGLWidget
{
public:
    AssetsWidget();
    ~AssetsWidget();

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
private:
    void initTiles();
    void initBuffers();

    QOpenGLFunctions *ogl;
    QOpenGLTexture *tile_texture;
    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vbo;
    QOpenGLShaderProgram *program;
    QOpenGLShader *vertex_shader;
    QOpenGLShader *fragment_shader;

    std::vector<TileInfo> tiles;
};

#endif // ASSETSWIDGET_H

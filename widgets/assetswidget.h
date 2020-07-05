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

    QOpenGLFunctions_3_3_Core *ogl;
    QOpenGLTexture *tile_texture;
    QOpenGLBuffer vbo;
    QOpenGLBuffer ibo;
    QOpenGLShaderProgram *program;
    QOpenGLShader *vertex_shader;
    QOpenGLShader *fragment_shader;

    std::vector<TileInfo> tiles;

    QMatrix4x4 matrix;

    void checkError(std::string action);
};

#endif // ASSETSWIDGET_H

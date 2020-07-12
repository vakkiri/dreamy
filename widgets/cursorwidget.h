#ifndef CURSORWIDGET_H
#define CURSORWIDGET_H

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <vector>
#include <unordered_map>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include "../types/types.h"

class QOpenGLTexture;
class QOpenGLShaderProgram;
class QOpenGLShader;
class QWheelEvent;
class QMouseEvent;
class AssetsWidget;

class CursorWidget : public QOpenGLWidget
{
public:
    CursorWidget();
    ~CursorWidget();

    void setAssetsWidget(AssetsWidget *widget);
    void setSnap(int snap);
    std::vector<AssetInstance>& getAssets();
    void resetCursor(Asset& asset);


protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;


private:
    void initBuffers();
    void updateSurface();
    int posToTile(int x, int y);

    int snap;

    AssetsWidget *assets_widget;

    QOpenGLFunctions_3_3_Core *ogl;
    std::unordered_map<std::string, QOpenGLTexture*> textures;
    QOpenGLBuffer vbo;
    QOpenGLBuffer ibo;
    QOpenGLShaderProgram *program;
    QOpenGLShader *vertex_shader;
    QOpenGLShader *fragment_shader;

    std::vector<AssetInstance> assets;

    QMatrix4x4 matrix;
    float scale;
    float minScale;
    float maxScale;

    AssetInstance preview;

    void checkError(std::string action);
};

#endif // CURSORWIDGET_H

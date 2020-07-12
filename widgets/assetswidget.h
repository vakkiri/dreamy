#ifndef ASSETSWIDGET_H
#define ASSETSWIDGET_H

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <vector>
#include <QVector>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <unordered_map>
#include "../types/types.h"

class QOpenGLTexture;
class QOpenGLShaderProgram;
class QOpenGLShader;
class QWheelEvent;
class QMouseEvent;
class CursorWidget;

class AssetsWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    AssetsWidget();
    ~AssetsWidget();

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void wheelEvent(QWheelEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    Asset& getSelection();
    void setCursorWidget(CursorWidget *widget);

private:
    void initBuffers();
    void addAssetsToBuffer(QVector<float>& buffer, std::string group_name);
    void updateScale(float newScale);
    void selectAsset(int x, int y);

    Asset selection;
    std::string selected_group;
    CursorWidget *cursor_widget;

    QOpenGLFunctions_3_3_Core *ogl;
    std::unordered_map<std::string, QOpenGLTexture*> textures;
    QOpenGLBuffer vbo;
    QOpenGLBuffer ibo;
    QOpenGLShaderProgram *program;
    QOpenGLShader *vertex_shader;
    QOpenGLShader *fragment_shader;

    QMatrix4x4 matrix;
    float scale;
    float minScale;
    float maxScale;

    void checkError(std::string action);

public slots:
    void setGroup(int index);
};

#endif // ASSETSWIDGET_H

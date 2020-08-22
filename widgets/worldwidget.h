#ifndef WORLDWIDGET_H
#define WORLDWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <vector>
#include <unordered_map>

#include "../types/types.h"

class CursorWidget;
class QOpenGLTexture;
class QOpenGLShaderProgram;
class QOpenGLShader;

class WorldWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    WorldWidget();
    ~WorldWidget();

    void setCursorWidget(CursorWidget *widget);
    void setSnap(int snap);
    std::vector<AssetInstance>& getAssets();
    void clearAssets();
    void addAsset(AssetInstance asset);
    void updateSurface();
    void setEditMode(EditMode new_mode);
    void mousePressPortal(QMouseEvent *event);
    void mouseReleasePortal(QMouseEvent *event);
    void mouseReleaseAdd(QMouseEvent *event);
protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;


private:
    void initBuffers();
    void scaleBy(float amt);

    std::vector<Portal> portals;

    int snap;

    CursorWidget *cursor_widget;

    QOpenGLFunctions_3_3_Core *ogl;
    std::unordered_map<std::string, QOpenGLTexture*> textures;
    QOpenGLBuffer vbo;
    QOpenGLBuffer colour_vbo;
    QOpenGLShaderProgram *program;
    QOpenGLShaderProgram *colour_program;
    QOpenGLShader *vertex_shader;
    QOpenGLShader *fragment_shader;
    QOpenGLShader *coloured_vertex_shader;
    QOpenGLShader *coloured_fragment_shader;

    std::vector<AssetInstance> assets;

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
    float last_real_mousex;
    float last_real_mousey;

    QPoint portal_start;
    QPoint portal_end;
    bool adding_portal;

    bool middle_click;

    bool add_solid;
    bool view_solid;

    int num_cursor_tiles;
    unsigned int num_solid_tiles;

    int current_layer;

    void checkError(std::string action);
    EditMode edit_mode;

public slots:
    void updateAddSolid(int state);
    void updateViewSolid(int state);
    void updateCurrentLayer(int layer);
};

#endif // WORLDWIDGET_H

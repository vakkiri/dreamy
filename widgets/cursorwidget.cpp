#include <iostream>
#include <cmath>
#include <QOpenGLContext>
#include <QOpenGLShader>
#include <QOpenGLTexture>
#include <QMouseEvent>
#include "cursorwidget.h"
#include "assetswidget.h"
#include "const.h"
#include "../util/assetcontainer.h"
#include "../util/utils.h"

#define VERTEX_POS 0
#define TEX_POS 1

CursorWidget::CursorWidget()
{
    setMaximumWidth(RIGHT_PANEL_WIDTH);
    setMaximumHeight(RIGHT_PANEL_WIDTH);
    setMouseTracking(true);
    assets_widget = nullptr;
    scale = 4;
    minScale = 1;
    maxScale = 4;
    snap = 16;
}

CursorWidget::~CursorWidget() {
    makeCurrent();
    delete tile_texture;
    vbo.destroy();
}

void CursorWidget::setAssetsWidget(AssetsWidget *widget) {
    assets_widget = widget;
}

void CursorWidget::initializeGL() {
    ogl = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
    ogl->initializeOpenGLFunctions();
    ogl->glClearColor(220.f/255.f, 221.f/255.f, 222.f/255.f, 1.f);
    ogl->glViewport(0.f, 0.f, width(), height());
    ogl->glEnable(GL_BLEND);
    ogl->glDisable(GL_DEPTH_TEST);
    ogl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    checkError("Initializing OGL");

    vbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vbo.create();
    checkError("Creating vbo");

    vertex_shader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    fragment_shader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    checkError("Creating shaders");

    vertex_shader->compileSourceFile(QString("shaders/textured_rect_shader.glvs"));
    checkError("Compiling vertex shader");
    fragment_shader->compileSourceFile(QString("shaders/textured_rect_shader.glfs"));
    checkError("Compiling fragment shader");

    program = new QOpenGLShaderProgram();
    program->addShader(vertex_shader);
    program->addShader(fragment_shader);
    program->bindAttributeLocation("vertexPos", VERTEX_POS);
    program->bindAttributeLocation("inTexCoord", TEX_POS);
    program->link();
    checkError("Linking program");

    program->bind();
    matrix.setToIdentity();
    matrix.ortho(0, this->width(), this->height(), 0, 1.0, -1.0);
    matrix.scale(scale);
    program->setUniformValue("pcMatrix", matrix);
    checkError("Setting matrix uniform");

    tile_texture = new QOpenGLTexture(editor_assets.get_image("tiles"));
    tile_texture->setMagnificationFilter(QOpenGLTexture::Filter::Nearest);
    program->setUniformValue("texUnit", 0);
    checkError("Creating tile");

    initTiles();
    checkError("Initializing tiles");

}

void CursorWidget::initTiles()
{
    // honestly this is really useless since the positions and texture coord are easily calculated
    // on the fly. might remove.
    for (int i = 0; i < tile_texture->width(); i += TILE_WIDTH)
    {
        tile_info.push_back(TextureData{float(i) / tile_texture->width(), 0});
    }
}

void CursorWidget::initBuffers()
{
    QVector<float> vertex_data;
    float tex_w = float(TILE_WIDTH)/tile_texture->width();
    float tex_h = float(TILE_WIDTH)/tile_texture->height();

    /* add the preview */
    // background
    vertex_data.push_back(preview.s);
    vertex_data.push_back(preview.t);
    vertex_data.push_back(preview.x);
    vertex_data.push_back(preview.y);

    vertex_data.push_back(preview.s + tex_w);
    vertex_data.push_back(preview.t);
    vertex_data.push_back(preview.x + TILE_WIDTH);
    vertex_data.push_back(preview.y);

    vertex_data.push_back(preview.s + tex_w);
    vertex_data.push_back(preview.t + tex_h);
    vertex_data.push_back(preview.x + TILE_WIDTH);
    vertex_data.push_back(preview.y + TILE_WIDTH);

    vertex_data.push_back(preview.s);
    vertex_data.push_back(preview.t + tex_h);
    vertex_data.push_back(preview.x);
    vertex_data.push_back(preview.y + TILE_WIDTH);

    /* add the actual tiles */
    for (unsigned int i = 0; i < tiles.size(); ++i) {
        vertex_data.push_back(tiles[i].s);
        vertex_data.push_back(tiles[i].t);
        vertex_data.push_back(tiles[i].x);
        vertex_data.push_back(tiles[i].y);

        vertex_data.push_back(tiles[i].s + tex_w);
        vertex_data.push_back(tiles[i].t);
        vertex_data.push_back(tiles[i].x + TILE_WIDTH);
        vertex_data.push_back(tiles[i].y);

        vertex_data.push_back(tiles[i].s + tex_w);
        vertex_data.push_back(tiles[i].t + tex_h);
        vertex_data.push_back(tiles[i].x + TILE_WIDTH);
        vertex_data.push_back(tiles[i].y + TILE_WIDTH);

        vertex_data.push_back(tiles[i].s);
        vertex_data.push_back(tiles[i].t + tex_h);
        vertex_data.push_back(tiles[i].x);
        vertex_data.push_back(tiles[i].y + TILE_WIDTH);
    }

    vbo.bind();
    vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo.allocate(vertex_data.constData(), vertex_data.count() * sizeof(float));

    program->enableAttributeArray(VERTEX_POS);
    program->setAttributeBuffer(VERTEX_POS, GL_FLOAT, 2 * sizeof(float), 2, 4 * sizeof(float));

    program->enableAttributeArray(TEX_POS);
    program->setAttributeBuffer(TEX_POS, GL_FLOAT, 0, 2, 4 * sizeof(float));
}


void CursorWidget::resizeGL(int w, int h)
{
    ogl->glViewport(0.f, 0.f, w, h);
    initBuffers();
    matrix.setToIdentity();
    matrix.ortho(0, w, h, 0, 1.0, -1.0);
    matrix.scale(scale);
    program->setUniformValue("pcMatrix", matrix);
    checkError("Setting matrix uniform");
}

void CursorWidget::paintGL()
{
    ogl->glClear(GL_COLOR_BUFFER_BIT);
    tile_texture->bind();

    // draw the preview
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // draw the cursor tiles
    for (unsigned int i = 1; i < tiles.size() + 1; ++i) {
        glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
    }
    checkError("Drawing elements");
}

void CursorWidget::checkError(std::string action)
{
    int error = glGetError();
    if (error) {
        std::cout << "OGL Error: " << error << std::endl;
        std::cout << "Context: " << action << std::endl;
    }
}

void CursorWidget::updateSurface() {
    initBuffers();
    matrix.setToIdentity();
    matrix.ortho(0, width(), height(), 0, 1.0, -1.0);
    matrix.scale(scale);
    program->bind();
    program->setUniformValue("pcMatrix", matrix);
    checkError("Setting matrix uniform");
    update();
}

void CursorWidget::mouseReleaseEvent(QMouseEvent *event) {
    float x = event->x() / scale;
    float y = event->y() / scale;
    x = int(x) - (int(x) % int(snap));
    y = int(y) - (int(y) % int(snap));

    float s = tile_info[assets_widget->getSelection()].s;
    float t = tile_info[assets_widget->getSelection()].t;

    if (event->button() == Qt::LeftButton) {
        // add a tile
        tiles.push_back(TileInfo{s,t,x,y});
        updateSurface();
    } else if (event->button() == Qt::RightButton) {
        // delete the first matching tile
        for (int i = tiles.size() - 1; i >= 0; --i) {
            // i guess this method of deletion doesn't actually maintain order lol oops
            if (clickInTile(event->x() / scale, event->y() / scale, tiles[i].x, tiles[i].y)) {
                tiles[i] = tiles.back();
                tiles.pop_back();
                updateSurface();
                break;
            }
        }
    }
}

void CursorWidget::wheelEvent(QWheelEvent *event) {
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numPixels.isNull()) {
        if (numPixels.y() > 0 && scale < maxScale) {
            scale += 1;
            updateSurface();
        } else if (numPixels.y() < 0 && scale > minScale) {
            scale -= 1;
            updateSurface();
        }
    } else if (!numDegrees.isNull()) {
        if (numDegrees.y() > 0 && scale < maxScale) {
            scale += 1;
            updateSurface();
        } else if (numDegrees.y() < 0 && scale > minScale) {
            scale -= 1;
            updateSurface();
        }
    }

    event->accept();
}

void CursorWidget::mouseMoveEvent(QMouseEvent *event) {
    float x = event->x() / scale;
    float y = event->y() / scale;
    x = int(x) - (int(x) % int(snap));
    y = int(y) - (int(y) % int(snap));

    preview.x = x;
    preview.y = y;
    preview.s = tile_info[assets_widget->getSelection()].s;
    preview.t = tile_info[assets_widget->getSelection()].t;

    updateSurface();
    event->accept();
}

void CursorWidget::setSnap(int snap) {
    this->snap = snap;
}

std::vector<TileInfo>& CursorWidget::getTiles() {
    return tiles;
}

void CursorWidget::resetCursor(int tileIndex) {
    float s = tile_info[tileIndex].s;
    float t = tile_info[tileIndex].t;

    tiles.clear();
    tiles.push_back(TileInfo{s, t, 0, 0});
    updateSurface();
}

#include <QOpenGLTexture>
#include <QOpenGLShader>
#include <QVector>
#include <QWheelEvent>

#include <iostream>

#include "../util/assetcontainer.h"
#include "assetswidget.h"
#include "cursorwidget.h"
#include "const.h"

#define VERTEX_POS 0
#define TEX_POS 1

AssetsWidget::AssetsWidget()
{
    setMaximumWidth(RIGHT_PANEL_WIDTH);
    minScale = 1.0;
    maxScale = 4.0;
    scale = 2.0;
    selection = 0;
    cursor_widget = nullptr;
}

AssetsWidget::~AssetsWidget()
{
    makeCurrent();
    delete tile_texture;
    vbo.destroy();
}

void AssetsWidget::initializeGL()
{
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

    initBuffers();
    checkError("Initializing buffers");
}

void AssetsWidget::initTiles()
{
    // honestly this is really useless since the positions and texture coord are easily calculated
    // on the fly. might remove.
    for (int i = 0; i < tile_texture->width(); i += TILE_WIDTH)
    {
        tiles.push_back(TextureData{float(i) / tile_texture->width(), 0});
    }
}

void AssetsWidget::initBuffers()
{
    QVector<float> vertex_data;
    float tex_w = float(TILE_WIDTH)/tile_texture->width();
    float tex_h = float(TILE_WIDTH)/tile_texture->height();
    float x = 0;
    float y = 0;

    for (unsigned int i = 0; i < tiles.size(); ++i) {
        vertex_data.push_back(tiles[i].s);
        vertex_data.push_back(tiles[i].t);
        vertex_data.push_back(x);
        vertex_data.push_back(y);

        vertex_data.push_back(tiles[i].s + tex_w);
        vertex_data.push_back(tiles[i].t);
        vertex_data.push_back(x + TILE_WIDTH);
        vertex_data.push_back(y);

        vertex_data.push_back(tiles[i].s + tex_w);
        vertex_data.push_back(tiles[i].t + tex_h);
        vertex_data.push_back(x + TILE_WIDTH);
        vertex_data.push_back(y + TILE_WIDTH);

        vertex_data.push_back(tiles[i].s);
        vertex_data.push_back(tiles[i].t + tex_h);
        vertex_data.push_back(x);
        vertex_data.push_back(y + TILE_WIDTH);

        x += TILE_WIDTH;
        if (x + TILE_WIDTH > width() / scale) {
            x = 0;
            y += TILE_WIDTH;
        }
    }

    vbo.bind();
    vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo.allocate(vertex_data.constData(), vertex_data.count() * sizeof(float));

    program->enableAttributeArray(VERTEX_POS);
    program->setAttributeBuffer(VERTEX_POS, GL_FLOAT, 2 * sizeof(float), 2, 4 * sizeof(float));

    program->enableAttributeArray(TEX_POS);
    program->setAttributeBuffer(TEX_POS, GL_FLOAT, 0, 2, 4 * sizeof(float));
}

void AssetsWidget::resizeGL(int w, int h)
{
    ogl->glViewport(0.f, 0.f, w, h);
    initBuffers();
    matrix.setToIdentity();
    matrix.ortho(0, w, h, 0, 1.0, -1.0);
    matrix.scale(scale);
    program->setUniformValue("pcMatrix", matrix);
    checkError("Setting matrix uniform");
}

void AssetsWidget::paintGL()
{
    ogl->glClear(GL_COLOR_BUFFER_BIT);
    tile_texture->bind();
    for (unsigned int i = 0; i < tiles.size(); ++i) {
        glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
    }
    checkError("Drawing elements");
}

void AssetsWidget::checkError(std::string action)
{
    int error = glGetError();
    if (error) {
        std::cout << "OGL Error: " << error << std::endl;
        std::cout << "Context: " << action << std::endl;
    }
}

void AssetsWidget::updateScale(float newScale) {
    scale = newScale;

    initBuffers();
    matrix.setToIdentity();
    matrix.ortho(0, width(), height(), 0, 1.0, -1.0);
    matrix.scale(scale);
    program->bind();
    program->setUniformValue("pcMatrix", matrix);
    checkError("Setting matrix uniform");
    update();
}

void AssetsWidget::wheelEvent(QWheelEvent *event) {
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numPixels.isNull()) {
        if (numPixels.y() > 0 && scale < maxScale) {
            updateScale(scale + 1.f);
        } else if (numPixels.y() < 0 && scale > minScale) {
            updateScale(scale - 1.f);
        }
    } else if (!numDegrees.isNull()) {
        if (numDegrees.y() > 0 && scale < maxScale) {
            updateScale(scale + 1.f);
        } else if (numDegrees.y() < 0 && scale > minScale) {
            updateScale(scale - 1.f);
        }
    }

    event->accept();
}

void AssetsWidget::mouseReleaseEvent(QMouseEvent *event) {
    selection = posToTile(event->x(), event->y());
    if (event->button() == Qt::RightButton) {
        cursor_widget->resetCursor(selection);
    }
}

int AssetsWidget::posToTile(int x, int y) {
    int row_size = width() / (TILE_WIDTH * scale);
    int row = y / (TILE_WIDTH * scale);
    int col = x / (TILE_WIDTH * scale);
    return (row * row_size) + col;
}

int AssetsWidget::getSelection() {
    return selection;
}

void AssetsWidget::setCursorWidget(CursorWidget *widget) {
    cursor_widget = widget;
}


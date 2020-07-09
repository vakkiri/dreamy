#include <iostream>
#include <algorithm>
#include <cmath>
#include <QOpenGLContext>
#include <QOpenGLShader>
#include <QOpenGLTexture>
#include <QMouseEvent>
#include "worldwidget.h"
#include "cursorwidget.h"
#include "const.h"
#include "../util/assetcontainer.h"
#include "../util/utils.h"

#define VERTEX_POS 0
#define TEX_POS 1

WorldWidget::WorldWidget()
{
    cursor_widget = nullptr;
    scale = 1;
    minScale = 1;
    maxScale = 4;
    snap = 16;
    previewx = 0;
    previewy = 0;
    tx = 0;
    ty = 0;
    middle_click = false;
    last_real_mousex = 0;
    last_real_mousey = 0;
    add_solid = true;
    view_solid = true;
    current_layer = 0;
    setMouseTracking(true);
}

WorldWidget::~WorldWidget() {
    makeCurrent();
    delete tile_texture;
    vbo.destroy();
}

void WorldWidget::setCursorWidget(CursorWidget *widget) {
    cursor_widget = widget;
}

void WorldWidget::initializeGL() {
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
    bg_texture = new QOpenGLTexture(editor_assets.get_image("background"));
    bg_texture->setMagnificationFilter(QOpenGLTexture::Filter::Nearest);
    program->setUniformValue("texUnit", 0);
    program->setUniformValue("solid", true);
    checkError("Creating tile");

    initTiles();
    checkError("Initializing tiles");

}

void WorldWidget::initTiles()
{
    // honestly this is really useless since the positions and texture coord are easily calculated
    // on the fly. might remove.
    for (int i = 0; i < tile_texture->width(); i += TILE_WIDTH)
    {
        tile_info.push_back(TextureData{float(i) / tile_texture->width(), 0});
    }
}

void WorldWidget::initBuffers()
{
    QVector<float> vertex_data;
    QVector<float> vertex_data_solid;
    float tex_w = float(TILE_WIDTH)/tile_texture->width();
    float tex_h = float(TILE_WIDTH)/tile_texture->height();
    float bgx = (width() / (2 * scale)) - (bg_texture->width() / 2) - tx;
    float bgy = (height() / (2* scale)) - (bg_texture->height() / 2) - ty;

    // background
    vertex_data.push_back(tx/bg_texture->width());
    vertex_data.push_back(0);
    vertex_data.push_back(bgx);
    vertex_data.push_back(bgy);

    vertex_data.push_back(1 + tx/bg_texture->width());
    vertex_data.push_back(0);
    vertex_data.push_back(bgx + bg_texture->width());
    vertex_data.push_back(bgy);

    vertex_data.push_back(1 + tx/bg_texture->width());
    vertex_data.push_back(1);
    vertex_data.push_back(bgx + bg_texture->width());
    vertex_data.push_back(bgy + bg_texture->height());

    vertex_data.push_back(tx/bg_texture->width());
    vertex_data.push_back(1);
    vertex_data.push_back(bgx);
    vertex_data.push_back(bgy + bg_texture->height());

    // cursor preview
    std::vector<TileInfo>& tiles_to_add = cursor_widget->getTiles();
    num_cursor_tiles = tiles_to_add.size();
    float minx = 10000; // what is MAX_FLOAT lol
    float miny = 10000;
    for(auto t : tiles_to_add) {
        minx = std::min(minx, t.x);
        miny = std::min(miny, t.y);
    }
    for(auto t : tiles_to_add) {
        float x = t.x + previewx - minx - tx;
        float y = t.y + previewy - miny - ty;
        x = int(x) - (int(x) % 16);
        y = int(y) - (int(y) % 16);

        vertex_data.push_back(t.s);
        vertex_data.push_back(t.t);
        vertex_data.push_back(x);
        vertex_data.push_back(y);

        vertex_data.push_back(t.s + tex_w);
        vertex_data.push_back(t.t);
        vertex_data.push_back(x + TILE_WIDTH);
        vertex_data.push_back(y);

        vertex_data.push_back(t.s + tex_w);
        vertex_data.push_back(t.t + tex_h);
        vertex_data.push_back(x + TILE_WIDTH);
        vertex_data.push_back(y + TILE_WIDTH);

        vertex_data.push_back(t.s);
        vertex_data.push_back(t.t + tex_h);
        vertex_data.push_back(x);
        vertex_data.push_back(y + TILE_WIDTH);
    }

    // tiles
    num_solid_tiles = 0;
    for (unsigned int i = 0; i < tiles.size(); ++i) {
        QVector<float> *vectorptr;
        if (tiles[i].solid) {
            vectorptr = &vertex_data_solid;
            num_solid_tiles += 1;
        } else {
            vectorptr = &vertex_data;
        }
        vectorptr->push_back(tiles[i].s);
        vectorptr->push_back(tiles[i].t);
        vectorptr->push_back(tiles[i].x);
        vectorptr->push_back(tiles[i].y);

        vectorptr->push_back(tiles[i].s + tex_w);
        vectorptr->push_back(tiles[i].t);
        vectorptr->push_back(tiles[i].x + TILE_WIDTH);
        vectorptr->push_back(tiles[i].y);

        vectorptr->push_back(tiles[i].s + tex_w);
        vectorptr->push_back(tiles[i].t + tex_h);
        vectorptr->push_back(tiles[i].x + TILE_WIDTH);
        vectorptr->push_back(tiles[i].y + TILE_WIDTH);

        vectorptr->push_back(tiles[i].s);
        vectorptr->push_back(tiles[i].t + tex_h);
        vectorptr->push_back(tiles[i].x);
        vectorptr->push_back(tiles[i].y + TILE_WIDTH);
    }

    vertex_data.append(vertex_data_solid);

    vbo.bind();
    vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo.allocate(vertex_data.constData(), vertex_data.count() * sizeof(float));

    program->enableAttributeArray(VERTEX_POS);
    program->setAttributeBuffer(VERTEX_POS, GL_FLOAT, 2 * sizeof(float), 2, 4 * sizeof(float));

    program->enableAttributeArray(TEX_POS);
    program->setAttributeBuffer(TEX_POS, GL_FLOAT, 0, 2, 4 * sizeof(float));
}


void WorldWidget::resizeGL(int w, int h)
{
    ogl->glViewport(0.f, 0.f, w, h);
    initBuffers();
    matrix.setToIdentity();
    matrix.ortho(0, w, h, 0, 1.0, -1.0);
    matrix.scale(scale);
    program->setUniformValue("pcMatrix", matrix);
    checkError("Setting matrix uniform");
}

void WorldWidget::paintGL()
{
    ogl->glClear(GL_COLOR_BUFFER_BIT);
    //background
    bg_texture->bind();
    // don't draw the background as "solid"
    program->setUniformValue("solid", false);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    //cursor + tiles
    tile_texture->bind();
    program->setUniformValue("solid", add_solid);
    for (int i = 1; i < num_cursor_tiles + 1; ++i) {
        glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
    }

    program->setUniformValue("solid", false);

    for (unsigned int i = num_cursor_tiles + 1; i < tiles.size() + num_cursor_tiles + 1; ++i) {
        // toggle solid visibility on for the solid tiles, which are always placed at the end of the buffer
        if (tiles.size() + num_cursor_tiles - i < num_solid_tiles && view_solid) {
            program->setUniformValue("solid", true);
        }
        glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
    }
    checkError("Drawing elements");
}

void WorldWidget::checkError(std::string action)
{
    int error = glGetError();
    if (error) {
        std::cout << "OGL Error: " << error << std::endl;
        std::cout << "Context: " << action << std::endl;
    }
}

void WorldWidget::updateSurface() {
    // make sure we never move to negative coordings (they don't play nicely with tilemap arrays...)
    tx = std::min(0.f, tx);
    ty = std::min(0.f, ty);

    initBuffers();
    matrix.setToIdentity();
    matrix.ortho(0, width(), height(), 0, 1.0, -1.0);
    matrix.scale(scale);
    matrix.translate(QVector3D(tx, ty, 0));
    program->bind();
    program->setUniformValue("pcMatrix", matrix);
    checkError("Setting matrix uniform");
    update();
}

void WorldWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::MiddleButton) {
        middle_click = true;
    }
    event->accept();
}

void WorldWidget::mouseReleaseEvent(QMouseEvent *event) {
    float x = event->x() / scale;
    float y = event->y() / scale;
    x -= tx;
    y -= ty;
    x = int(x) - (int(x) % int(snap));
    y = int(y) - (int(y) % int(snap));

    if (event->button() == Qt::LeftButton) {
        // add all cursor tiles
        std::vector<TileInfo>& tiles_to_add = cursor_widget->getTiles();
        // find  minimum x and y
        // The point of this is a slight QOL impprovmenet since we can assume that we aren't interested in placing
        // any "empty" spaces to the left or above our cursor.
        float minx = 10000; // what is MAX_FLOAT lol
        float miny = 10000;
        for(auto t : tiles_to_add) {
            minx = std::min(minx, t.x);
            miny = std::min(miny, t.y);
        }
        for(auto t : tiles_to_add) {
            tiles.push_back(TileInfo{t.s,t.t,t.x + x - minx,t.y + y - miny, add_solid, current_layer});
        }
        updateSurface();
    } else if (event->button() == Qt::RightButton) {
        // delete the first matching tile
        for (int i = tiles.size() - 1; i >= 0; --i) {
            // i guess this method of deletion doesn't actually maintain order lol oops
            if (clickInTile(event->x() / scale - tx, event->y() / scale - ty, tiles[i].x, tiles[i].y)) {
                tiles[i] = tiles.back();
                tiles.pop_back();
                updateSurface();
                break;
            }
        }
    } else if (event->button() == Qt::MiddleButton) {
        middle_click = false;
    }
}

void WorldWidget::scaleBy(float amt) {
    float prex = mousex / scale;
    float prey = mousey / scale;
    float postx;
    float posty;

    scale += amt;

    postx = mousex / scale;
    posty = mousey / scale;

    tx += (postx - prex) / (scale-amt);
    ty += (posty - prey) / (scale-amt);

    updateSurface();
}

void WorldWidget::wheelEvent(QWheelEvent *event) {
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numPixels.isNull()) {
        if (numPixels.y() > 0 && scale < maxScale) {
            scaleBy(1);
        } else if (numPixels.y() < 0 && scale > minScale) {
            scaleBy(-1);
        }
    } else if (!numDegrees.isNull()) {
        if (numDegrees.y() > 0 && scale < maxScale) {
            scaleBy(1);
        } else if (numDegrees.y() < 0 && scale > minScale) {
            scaleBy(-1);
        }
    }

    event->accept();
}

void WorldWidget::mouseMoveEvent(QMouseEvent *event) {
    float x = event->x() / scale;
    float y = event->y() / scale;
    mousex = x - tx * scale;
    mousey = y - ty * scale;
    previewx = x;
    previewy = y;

    if (middle_click) {
        tx += (event->x() - last_real_mousex) / scale;
        ty += (event->y() - last_real_mousey) / scale;
    }

    last_real_mousex = event->x();
    last_real_mousey = event->y();

    updateSurface();
    event->accept();
}

void WorldWidget::setSnap(int snap) {
    this->snap = snap;
}

void WorldWidget::updateAddSolid(int state) {
    add_solid = state == Qt::CheckState::Checked;
    updateSurface();
}

void WorldWidget::updateViewSolid(int state) {
    view_solid = state == Qt::CheckState::Checked;
    updateSurface();
}

void WorldWidget::updateCurrentLayer(int layer) {
    current_layer = layer;
}

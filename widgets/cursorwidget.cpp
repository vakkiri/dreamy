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
    preview.group = "tiles";
    preview.x = 0;
    preview.y = 0;
    preview.s = 0;
    preview.t = 0;
    preview.w = 16;
    preview.h = 16;
}

CursorWidget::~CursorWidget() {
    makeCurrent();
    for (auto tex : textures) {
        delete tex.second;
    }
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

    for (auto asset : editor_assets.get_assets()) {
        QOpenGLTexture* new_tex = new QOpenGLTexture(editor_assets.get_image(asset.first));
        new_tex->setMagnificationFilter(QOpenGLTexture::Filter::Nearest);
        textures[asset.first] = new_tex;
    }
    program->setUniformValue("texUnit", 0);
    checkError("Creating tile");

    checkError("Initializing tiles");

}

void CursorWidget::initBuffers()
{
    QVector<float> vertex_data;
    float ds = float(preview.w)/textures[preview.group]->width();
    float dt = float(preview.h)/textures[preview.group]->height();

    /* add the preview */
    vertex_data.push_back(preview.s);
    vertex_data.push_back(preview.t);
    vertex_data.push_back(preview.x);
    vertex_data.push_back(preview.y);

    vertex_data.push_back(preview.s + ds);
    vertex_data.push_back(preview.t);
    vertex_data.push_back(preview.x + preview.w);
    vertex_data.push_back(preview.y);

    vertex_data.push_back(preview.s + ds);
    vertex_data.push_back(preview.t + dt);
    vertex_data.push_back(preview.x + preview.w);
    vertex_data.push_back(preview.y + preview.h);

    vertex_data.push_back(preview.s);
    vertex_data.push_back(preview.t + dt);
    vertex_data.push_back(preview.x);
    vertex_data.push_back(preview.y + preview.h);

    /* add the actual tiles */
    for (unsigned int i = 0; i < assets.size(); ++i) {
        ds = float(assets[i].w)/textures[assets[i].group]->width();
        dt = float(assets[i].h)/textures[assets[i].group]->height();

        vertex_data.push_back(assets[i].s);
        vertex_data.push_back(assets[i].t);
        vertex_data.push_back(assets[i].x);
        vertex_data.push_back(assets[i].y);

        vertex_data.push_back(assets[i].s + ds);
        vertex_data.push_back(assets[i].t);
        vertex_data.push_back(assets[i].x + assets[i].w);
        vertex_data.push_back(assets[i].y);

        vertex_data.push_back(assets[i].s + ds);
        vertex_data.push_back(assets[i].t + dt);
        vertex_data.push_back(assets[i].x + assets[i].w);
        vertex_data.push_back(assets[i].y + assets[i].h);

        vertex_data.push_back(assets[i].s);
        vertex_data.push_back(assets[i].t + dt);
        vertex_data.push_back(assets[i].x);
        vertex_data.push_back(assets[i].y + assets[i].h);
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

    // draw the preview
    textures[preview.group]->bind();
    std::string last_group = preview.group;
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // draw the cursor tiles
    for (unsigned int i = 1; i < assets.size() + 1; ++i) {
        if (assets[i-1].group != last_group) {
            textures[assets[i-1].group]->bind();
            last_group = assets[i-1].group;
        }
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

    Asset asset = assets_widget->getSelection();

    float s = asset.s / textures[asset.group]->width();
    float t = asset.t / textures[asset.group]->height();

    if (event->button() == Qt::LeftButton) {
        // add a tile
        assets.push_back(AssetInstance{asset.group, x, y, s, t, asset.w, asset.h});
        updateSurface();
    } else if (event->button() == Qt::RightButton) {
        // delete the first matching tile
        for (int i = assets.size() - 1; i >= 0; --i) {
            // i guess this method of deletion doesn't actually maintain order lol oops
            if (clickInRect(event->x() / scale, event->y() / scale, assets[i].x, assets[i].y, assets[i].w, assets[i].h)) {
                assets[i] = assets.back();
                assets.pop_back();
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
    Asset& asset = assets_widget->getSelection();

    float x = event->x() / scale;
    float y = event->y() / scale;
    x = int(x) - (int(x) % int(snap));
    y = int(y) - (int(y) % int(snap));

    preview.x = x;
    preview.y = y;
    preview.s = asset.s / textures[asset.group]->width();
    preview.t = asset.t / textures[asset.group]->height();
    preview.w = asset.w;
    preview.h = asset.h;
    preview.group = asset.group;

    updateSurface();
    event->accept();
}

void CursorWidget::setSnap(int snap) {
    this->snap = snap;
}

std::vector<AssetInstance>& CursorWidget::getAssets() {
    return assets;
}

void CursorWidget::resetCursor(Asset& asset) {
    float s = asset.s / textures[asset.group]->width();
    float t = asset.t / textures[asset.group]->height();

    assets.clear();
    // imagine not having a copy constructor...
    assets.push_back(AssetInstance{asset.group, 0, 0, s, t, asset.w, asset.h});
    updateSurface();
}

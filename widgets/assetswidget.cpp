#include <QOpenGLTexture>
#include <QOpenGLShader>
#include <QVector>
#include <QWheelEvent>

#include <iostream>
#include <cmath>

#include "../util/assetcontainer.h"
#include "../util/utils.h"
#include "assetswidget.h"
#include "cursorwidget.h"
#include "const.h"
#include "../glo.h"

#define VERTEX_POS 0
#define TEX_POS 1

AssetsWidget::AssetsWidget()
{
    setMaximumWidth(RIGHT_PANEL_WIDTH);
    minScale = 1.0;
    maxScale = 4.0;
    scale = 2.0;
    selection = Asset{"tiles", 0, 0, 16.f, 16.f, 0};
    cursor_widget = nullptr;
    selected_group = "tiles";
}

AssetsWidget::~AssetsWidget()
{
    makeCurrent();
    for (auto tex : textures) {
        delete tex.second;
    }
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

    for (auto asset : editor_assets.get_assets()) {
        QOpenGLTexture* new_tex = new QOpenGLTexture(editor_assets.get_image(asset.first));
        new_tex->setMagnificationFilter(QOpenGLTexture::Filter::Nearest);
        textures[asset.first] = new_tex;
    }

    program->setUniformValue("texUnit", 0);
    checkError("Creating tile");

    initBuffers();
    checkError("Initializing buffers");
}

void AssetsWidget::addAssetsToBuffer(QVector<float>& buffer, std::string group_name) {
    std::vector<Asset> assets = editor_assets.get_assets(group_name);
    float x = 0;
    float y = 0;
    float t;
    QOpenGLTexture* tex = textures[group_name];
    float max_height = 0;
    for (auto asset : assets) {
        float ds = float(asset.w) / tex->width();
        float dt = float(asset.h) / tex->height();

        // for tiles, the t value is based on tileset
        if (asset.type == 0 || asset.type == 1) {
            t = asset.t * tileset;
        }
        else {
            t = asset.t;
        }

        buffer.push_back(asset.s);
        buffer.push_back(t);
        buffer.push_back(x);
        buffer.push_back(y);

        buffer.push_back(asset.s + ds);
        buffer.push_back(t);
        buffer.push_back(x + asset.w);
        buffer.push_back(y);

        buffer.push_back(asset.s + ds);
        buffer.push_back(t + dt);
        buffer.push_back(x + asset.w);
        buffer.push_back(y + asset.h);

        buffer.push_back(asset.s);
        buffer.push_back(t + dt);
        buffer.push_back(x);
        buffer.push_back(y + asset.h);

        if (asset.h > max_height) {
            max_height = asset.h;
        }
        x += asset.w;
        if (x + asset.w > width() / scale) {
            x = 0;
            y += max_height;
            max_height = 0;
        }
    }
}

void AssetsWidget::initBuffers()
{
    vertex_data.clear();
    addAssetsToBuffer(vertex_data, selected_group);

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
    QOpenGLTexture *texture = textures[selected_group];
    texture->bind();
    for (unsigned int i = 0; i < editor_assets.get_assets(selected_group).size(); ++i) {
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
    selectAsset(event->x(), event->y());

    if (event->button() == Qt::RightButton) {
        cursor_widget->resetCursor(selection);
    }
}

void AssetsWidget::selectAsset(int x, int y) {
    x /= scale;
    y /= scale;

    std::vector<Asset> assets = editor_assets.get_assets(selected_group);
    for (int i = 0; i < vertex_data.size(); i += 16) {
        float assetx = vertex_data[i+2];
        float assety = vertex_data[i+3];
        float assetw = vertex_data[i+6] - vertex_data[i+2];
        float asseth = vertex_data[i+11] - vertex_data[i+3];

        if (clickInRect(x, y, assetx, assety, assetw, asseth)) {
            int asset_index = i / 16;
            selection.s = assets[asset_index].s;
            selection.t = assets[asset_index].t;
            selection.w = assets[asset_index].w;
            selection.h = assets[asset_index].h;
            selection.group = selected_group;
            selection.type = assets[asset_index].type;
            break;
        }
    }
}

Asset& AssetsWidget::getSelection() {
    return selection;
}

void AssetsWidget::setCursorWidget(CursorWidget *widget) {
    cursor_widget = widget;
}

void AssetsWidget::setGroup(int index) {
    switch(index) {
        case 0: {
            selected_group = "tiles";
            updateScale(scale);
            break;
        }
        case 1: {
            selected_group = "objects";
            updateScale(scale);
            break;
        }
        case 2: {
            selected_group = "scenery";
            updateScale(scale);
            break;
        }
        default: {
            std::cout << "Unknown asset group: " << index << std::endl;
            break;
        }
    }
}

#include <QOpenGLTexture>
#include <QOpenGLShader>
#include <QVector>

#include <iostream>

#include "../util/assetcontainer.h"
#include "assetswidget.h"
#include "const.h"

#define VERTEX_POS 0
#define TEX_POS 1
AssetsWidget::AssetsWidget()
{
    setMaximumWidth(RIGHT_PANEL_WIDTH);
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
    ogl->glClearColor(0.f/255.f, 200.f/255.f, 100.f/255.f, 1.f);
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
    program->setUniformValue("pcMatrix", matrix);
    checkError("Setting matrix uniform");

    tile_texture = new QOpenGLTexture(editor_assets.get_image("tiles"));
    program->setUniformValue("texUnit", 0);
    checkError("Creating tile");

    initTiles();
    checkError("Initializing tiles");

    initBuffers();
    checkError("Initializing buffers");
}

void AssetsWidget::initTiles()
{
    int texture_w;

    texture_w = tile_texture->width();

    for (int i = 0; i < texture_w; i += TILE_WIDTH)
    {
        tiles.push_back(TileInfo{float(i) / texture_w, 0});
    }
}

void AssetsWidget::initBuffers()
{
    QVector<float> vertex_data;
    float tex_w = 16.f/tile_texture->width();

    for (unsigned int i = 0; i < tiles.size(); ++i) {
        vertex_data.push_back(tex_w * i);
        vertex_data.push_back(tex_w * i);
        vertex_data.push_back(TILE_WIDTH * i);
        vertex_data.push_back(TILE_WIDTH * i);

        vertex_data.push_back(tex_w * (i+1));
        vertex_data.push_back(tex_w);
        vertex_data.push_back(TILE_WIDTH * (i+1));
        vertex_data.push_back(TILE_WIDTH * i);

        vertex_data.push_back(tex_w * (i+1));
        vertex_data.push_back(tex_w * (i+1));
        vertex_data.push_back(TILE_WIDTH * (i+1));
        vertex_data.push_back(TILE_WIDTH * (i+1));

        vertex_data.push_back(tex_w * i);
        vertex_data.push_back(tex_w * (i+1));
        vertex_data.push_back(TILE_WIDTH * i);
        vertex_data.push_back(TILE_WIDTH * (i+1));
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

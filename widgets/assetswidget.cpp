#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLShader>
#include <iostream>

#include "../util/assetcontainer.h"
#include "assetswidget.h"
#include "const.h"

AssetsWidget::AssetsWidget()
{
    setMaximumWidth(RIGHT_PANEL_WIDTH);
}

AssetsWidget::~AssetsWidget()
{
    makeCurrent();
    delete tile_texture;
    vao.destroy();
    vbo.destroy();
}

void AssetsWidget::initializeGL()
{
    ogl = QOpenGLContext::currentContext()->functions();
    ogl->glClearColor(0.f/255.f, 0.f/255.f, 0.f/255.f, 1.f);

    vao.create();
    vao.bind();
    vbo.create();
    vbo.bind();

    vertex_shader = new QOpenGLShader(QOpenGLShader::Vertex);
    fragment_shader = new QOpenGLShader(QOpenGLShader::Fragment);
    vertex_shader->compileSourceFile(QString("shaders/textured_rect_shader.glvs"));
    fragment_shader->compileSourceFile(QString("shaders/textured_rect_shader.glfs"));

    initTiles();
    initBuffers();
}

void AssetsWidget::initTiles()
{
    int texture_w;
    int tile_w;

    tile_texture = new QOpenGLTexture(editor_assets.get_image("tiles"));
    texture_w = tile_texture->width();
    tile_w = 16;

    for (int i = 0; i < texture_w; i += 16)
    {
        tiles.push_back(TileInfo{float(i), 0});
    }

}

void AssetsWidget::initBuffers()
{

}

void AssetsWidget::resizeGL(int w, int h)
{
    (void) w;
    (void) h;
    initBuffers();
}

void AssetsWidget::paintGL()
{
    ogl->glClear(GL_COLOR_BUFFER_BIT);
}

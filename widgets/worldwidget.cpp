#include <iostream>
#include <algorithm>
#include <cmath>
#include <QOpenGLContext>
#include <QOpenGLShader>
#include <QOpenGLTexture>
#include <QMouseEvent>
#include <QLineEdit>
#include <QFormLayout>
#include <QDialogButtonBox>
#include "worldwidget.h"
#include "cursorwidget.h"
#include "const.h"
#include "../util/assetcontainer.h"
#include "../util/utils.h"
#include "../layouts/mainlayout.h"
#include "../glo.h"

#define VERTEX_POS 0
#define TEX_POS 1
#define COLOUR_VERTEX_POS 2

PortalDialogue::PortalDialogue() {
    layout = new QFormLayout(this);

    dest_level = new QLineEdit(this);
    QString level_label = QString("Destination Level");
    layout->addRow(level_label, dest_level);

    dest_x = new QLineEdit(this);
    QString x_label = QString("Destination X");
    layout->addRow(x_label, dest_x);

    dest_y = new QLineEdit(this);
    QString y_label = QString("Destination Y");
    layout->addRow(y_label, dest_y);

    buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, this);
    layout->addRow(buttons);

    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

PortalDialogue::~PortalDialogue() {
    delete dest_x;
    delete dest_y;
    delete dest_level;
    delete layout;
    delete buttons;
}

float PortalDialogue::targetx() {
    return dest_x->text().toFloat();
}

float PortalDialogue::targety() {
    return dest_y->text().toFloat();
}

int PortalDialogue::target_level() {
    return dest_level->text().toInt();
}

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
    adding_portal = false;
    edit_mode = ADD_MODE;
    setMouseTracking(true);
}

WorldWidget::~WorldWidget() {
    makeCurrent();
    for (auto tex : textures) {
        delete tex.second;
    }
    vbo.destroy();
    colour_vbo.destroy();
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

    colour_vbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    colour_vbo.create();
    checkError("Creating colour vbo");

    vertex_shader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    fragment_shader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    coloured_vertex_shader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    coloured_fragment_shader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    checkError("Creating shaders");

    vertex_shader->compileSourceFile(QString("shaders/textured_rect_shader.glvs"));
    checkError("Compiling vertex shader");
    fragment_shader->compileSourceFile(QString("shaders/textured_rect_shader.glfs"));
    checkError("Compiling fragment shader");
    coloured_vertex_shader->compileSourceFile(QString("shaders/coloured_rect_shader.glvs"));
    checkError("Compiling vertex shader");
    coloured_fragment_shader->compileSourceFile(QString("shaders/coloured_rect_shader.glfs"));
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
    program->setUniformValue("texUnit", 0);
    program->setUniformValue("solid", true);
    checkError("Setting matrix uniform");

    colour_program = new QOpenGLShaderProgram();
    colour_program->addShader(coloured_vertex_shader);
    colour_program->addShader(coloured_fragment_shader);
    colour_program->bindAttributeLocation("cvertexPos", COLOUR_VERTEX_POS);
    colour_program->link();
    checkError("Linking program");

    colour_program->bind();
    colour_program->setUniformValue("cpcMatrix", matrix);
    checkError("Setting matrix uniform");

    for (auto asset : editor_assets.get_assets()) {
        QOpenGLTexture* new_tex = new QOpenGLTexture(editor_assets.get_image(asset.first));
        new_tex->setMagnificationFilter(QOpenGLTexture::Filter::Nearest);
        textures[asset.first] = new_tex;
    }


    checkError("Creating tile");

    checkError("Initializing tiles");
}

void WorldWidget::initBuffers()
{
    QVector<float> vertex_data;
    QVector<float> vertex_data_solid;
    QVector<float> portal_vertex_data;
    QOpenGLTexture* bg_texture = textures["background"];

    float bgx = (width() / (2 * scale)) - (bg_texture->width() / 2) - tx;
    float bgy = (height() / (2* scale)) - (bg_texture->height() / 2) - ty;
    float bg_ds = (-tx/bg_texture->width()) * 0.5;  // parallax bg scroll

    // background
    vertex_data.push_back(bg_ds);
    vertex_data.push_back(0);
    vertex_data.push_back(bgx);
    vertex_data.push_back(bgy);

    vertex_data.push_back(1 + bg_ds);
    vertex_data.push_back(0);
    vertex_data.push_back(bgx + bg_texture->width());
    vertex_data.push_back(bgy);

    vertex_data.push_back(1 + bg_ds);
    vertex_data.push_back(1);
    vertex_data.push_back(bgx + bg_texture->width());
    vertex_data.push_back(bgy + bg_texture->height());

    vertex_data.push_back(bg_ds);
    vertex_data.push_back(1);
    vertex_data.push_back(bgx);
    vertex_data.push_back(bgy + bg_texture->height());

    // cursor preview
    std::vector<AssetInstance>& assets_to_add = cursor_widget->getAssets();
    num_cursor_tiles = assets_to_add.size();
    float minx = 10000; // what is MAX_FLOAT lol
    float miny = 10000;
    for(auto t : assets_to_add) {
        minx = std::min(minx, t.x);
        miny = std::min(miny, t.y);
    }

    for(auto asset : assets_to_add) {
        float x = asset.x + previewx - minx - tx;
        float y = asset.y + previewy - miny - ty;
        float ds = float(asset.w) / textures[asset.group]->width();
        float dt = float(asset.h) / textures[asset.group]->height();
        float t;
        x = int(x) - (int(x) % snap);
        y = int(y) - (int(y) % snap);

        // for tiles, the t value is based on tileset
        if (asset.type == 0 || asset.type == 1) {
            t = asset.t * tileset;
        }
        else {
            t = asset.t;
        }

        vertex_data.push_back(asset.s);
        vertex_data.push_back(t);
        vertex_data.push_back(x);
        vertex_data.push_back(y);

        vertex_data.push_back(asset.s + ds);
        vertex_data.push_back(t);
        vertex_data.push_back(x + asset.w);
        vertex_data.push_back(y);

        vertex_data.push_back(asset.s + ds);
        vertex_data.push_back(t + dt);
        vertex_data.push_back(x + asset.w);
        vertex_data.push_back(y + asset.h);

        vertex_data.push_back(asset.s);
        vertex_data.push_back(t + dt);
        vertex_data.push_back(x);
        vertex_data.push_back(y + asset.h);
    }

    // world assets
    num_solid_tiles = 0;
    for (unsigned int i = 0; i < assets.size(); ++i) {
        QVector<float> *vectorptr = &vertex_data;   // this is tech debt from when there were multiple vectors and i don't feel like fixing it atm
        float ds = float(assets[i].w) / textures[assets[i].group]->width();
        float dt = float(assets[i].h) / textures[assets[i].group]->height();
        float t;

        // for tiles, the t value is based on tileset
        if (assets[i].type == 0 || assets[i].type == 1) {
            t = assets[i].t * tileset;
        }
        else {
            t = assets[i].t;
        }

        vectorptr->push_back(assets[i].s);
        vectorptr->push_back(t);
        vectorptr->push_back(assets[i].x);
        vectorptr->push_back(assets[i].y);

        vectorptr->push_back(assets[i].s + ds);
        vectorptr->push_back(t);
        vectorptr->push_back(assets[i].x + assets[i].w);
        vectorptr->push_back(assets[i].y);

        vectorptr->push_back(assets[i].s + ds);
        vectorptr->push_back(t + dt);
        vectorptr->push_back(assets[i].x + assets[i].w);
        vectorptr->push_back(assets[i].y + assets[i].h);

        vectorptr->push_back(assets[i].s);
        vectorptr->push_back(t + dt);
        vectorptr->push_back(assets[i].x);
        vectorptr->push_back(assets[i].y + assets[i].h);
    }

    vertex_data.append(vertex_data_solid);

    vbo.bind();
    vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo.allocate(vertex_data.constData(), vertex_data.count() * sizeof(float));

    program->bind();
    program->enableAttributeArray(VERTEX_POS);
    program->setAttributeBuffer(VERTEX_POS, GL_FLOAT, 2 * sizeof(float), 2, 4 * sizeof(float));

    program->enableAttributeArray(TEX_POS);
    program->setAttributeBuffer(TEX_POS, GL_FLOAT, 0, 2, 4 * sizeof(float));

    // PORTAL DATA
    for (auto p : portals) {
        portal_vertex_data.push_back(p.x);
        portal_vertex_data.push_back(p.y);
        portal_vertex_data.push_back(p.x + p.w);
        portal_vertex_data.push_back(p.y);
        portal_vertex_data.push_back(p.x + p.w);
        portal_vertex_data.push_back(p.y + p.h);
        portal_vertex_data.push_back(p.x);
        portal_vertex_data.push_back(p.y + p.h);
    }

    colour_program->bind();
    colour_vbo.bind();
    colour_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    colour_vbo.allocate(portal_vertex_data.constData(), portal_vertex_data.count() * sizeof(float));
    colour_program->enableAttributeArray(COLOUR_VERTEX_POS);
    colour_program->setAttributeBuffer(COLOUR_VERTEX_POS, GL_FLOAT, 0, 2, 2 * sizeof(float));
}


void WorldWidget::resizeGL(int w, int h)
{
    ogl->glViewport(0.f, 0.f, w, h);
    initBuffers();
    matrix.setToIdentity();
    matrix.ortho(0, w, h, 0, 1.0, -1.0);
    matrix.scale(scale);
    colour_program->bind();
    colour_program->setUniformValue("cpcMatrix", matrix);
    program->bind();
    program->setUniformValue("pcMatrix", matrix);

    checkError("Setting matrix uniform");
}

void WorldWidget::paintGL()
{
    std::string last_group = "background";

    program->bind();

    ogl->glClear(GL_COLOR_BUFFER_BIT);
    //background
    textures["background"]->bind();
    // don't draw the background as "solid"
    program->setUniformValue("solid", false);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    //cursor + tiles
    std::vector<AssetInstance>& assets_to_add = cursor_widget->getAssets();
    for (int i = 1; i < num_cursor_tiles + 1; ++i) {
        program->setUniformValue("solid", add_solid && assets_to_add[i-1].type == 0);
        if (assets_to_add[i-1].group != last_group) {
            last_group = assets_to_add[i-1].group;
            textures[assets_to_add[i-1].group]->bind();
        }
        glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
    }

    for (unsigned int i = num_cursor_tiles + 1; i < assets.size() + num_cursor_tiles + 1; ++i) {
        int asset_index = i-1-num_cursor_tiles;
        program->setUniformValue("solid", assets[asset_index].type == 1 && view_solid);

        if (assets[asset_index].group != last_group) {
            last_group = assets[asset_index].group;
            textures[assets[asset_index].group]->bind();
        }
        // toggle solid visibility on for the solid tiles, which are always placed at the end of the buffer
        if (assets.size() + num_cursor_tiles - i < num_solid_tiles && view_solid) {
            program->setUniformValue("solid", true);
        }

        glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
    }

    colour_program->bind();
    colour_program->setUniformValue("vcolor", 0.7f, 0.1f, 0.3f, 0.5f);
    for (unsigned int i = 0; i < portals.size(); ++i) {
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
    colour_program->bind();
    colour_program->setUniformValue("cpcMatrix", matrix);
    checkError("Setting matrix uniform");
    update();
}

void WorldWidget::mousePressPortal(QMouseEvent *event) {
    float x = event->x() / scale;
    float y = event->y() / scale;
    x -= tx;
    y -= ty;
    if (event->button() == Qt::LeftButton) {
        portal_start = QPoint(x, y);
        adding_portal = true;
    }
}

void WorldWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::MiddleButton) {
        middle_click = true;
    } else if (edit_mode == PORTAL_MODE) {
        mousePressPortal(event);
    }
    event->accept();
}

void WorldWidget::mouseReleasePortal(QMouseEvent *event) {
    float x = event->x() / scale;
    float y = event->y() / scale;
    x -= tx;
    y -= ty;
    if (event->button() == Qt::LeftButton) {
        portal_end = QPoint(x, y);
        float startx, starty, endx, endy;
        startx = std::min(portal_start.x(), portal_end.x());
        endx = std::max(portal_start.x(), portal_end.x());
        starty = std::min(portal_start.y(), portal_end.y());
        endy = std::max(portal_start.y(), portal_end.y());
        float w = endx - startx;
        float h = endy - starty;

        PortalDialogue dialogue;
        if (dialogue.exec() == QDialog::Accepted) {
            std::cout << dialogue.targetx() << " : " << dialogue.targety() << std::endl;
            portals.push_back(Portal{startx, starty, w, h, dialogue.targetx(), dialogue.targety(), dialogue.target_level()});
            updateSurface();
        }
    } else if (event->button() == Qt::RightButton) {
        for (unsigned int i = 0; i < portals.size(); ++i) {
            if (clickInRect(x, y, portals[i].x, portals[i].y, portals[i].w, portals[i].h)) {
                portals[i] = portals.back();
                portals.pop_back();
                updateSurface();
                break;
            }
        }
    }
}

void WorldWidget::mouseReleaseAdd(QMouseEvent *event) {
    float x = event->x() / scale;
    float y = event->y() / scale;
    x -= tx;
    y -= ty;
    x = int(x) - (int(x) % int(snap));
    y = int(y) - (int(y) % int(snap));

    if (event->button() == Qt::LeftButton) {
        // add all cursor tiles
        std::vector<AssetInstance>& assets_to_add = cursor_widget->getAssets();
        // find  minimum x and y
        // The point of this is a slight QOL impprovmenet since we can assume that we aren't interested in placing
        // any "empty" spaces to the left or above our cursor.
        float minx = 10000; // what is MAX_FLOAT lol
        float miny = 10000;
        for(auto asset : assets_to_add) {
            minx = std::min(minx, asset.x);
            miny = std::min(miny, asset.y);
        }

        for(auto asset : assets_to_add) {
            // special case for how solid tiles are handled - type 0 (passable tile) becomes type 1 (sold tile)
            if (asset.type == 0 && asset.group == "tiles" && add_solid) {
                assets.push_back(AssetInstance{asset.group, asset.x + x - minx, asset.y + y - miny, asset.s, asset.t, asset.w, asset.h, 1});
            } else {
                assets.push_back(AssetInstance{asset.group, asset.x + x - minx, asset.y + y - miny, asset.s, asset.t, asset.w, asset.h, asset.type});
            }
        }

        updateSurface();
    } else if (event->button() == Qt::RightButton) {
        // delete the first matching tile
        for (int i = assets.size() - 1; i >= 0; --i) {
            // i guess this method of deletion doesn't actually maintain order lol oops
            if (clickInRect(event->x() / scale - tx, event->y() / scale - ty, assets[i].x, assets[i].y, assets[i].w, assets[i].h)) {
                assets[i] = assets.back();
                assets.pop_back();
                updateSurface();
                break;
            }
        }
    }
}

void WorldWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (edit_mode == ADD_MODE) {
        mouseReleaseAdd(event);
    } else if (edit_mode == PORTAL_MODE) {
        mouseReleasePortal(event);
    }
    if (event->button() == Qt::MiddleButton) {
        middle_click = false;
    }

    adding_portal = false;  // make sure we unset this even if something weird happened and we weren't in portal mode
}

void WorldWidget::scaleBy(float amt) {
    scale += amt;

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
    mousex = x - tx;
    mousey = y - ty;
    previewx = x;
    previewy = y;

    if (middle_click) {
        tx += (event->x() - last_real_mousex) / scale;
        ty += (event->y() - last_real_mousey) / scale;
    }

    last_real_mousex = event->x();
    last_real_mousey = event->y();

    parent_layout->setMouseLabel(mousex, mousey);
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

std::vector<AssetInstance>& WorldWidget::getAssets() {
    return assets;
}

std::vector<Portal>& WorldWidget::getPortals() {
    return portals;
}

void WorldWidget::clearAssets() {
    assets.clear();
}

void WorldWidget::addAsset(AssetInstance asset) {
    assets.push_back(asset);
}

void WorldWidget::addPortal(Portal portal) {
    portals.push_back(portal);
}

void WorldWidget::setEditMode(EditMode new_mode) {
    edit_mode = new_mode;
}

void WorldWidget::setParentLayout(MainLayout *layout) {
    parent_layout = layout;
}

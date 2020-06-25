#include "worldwidget.h"

WorldWidget::WorldWidget()
{
}

void WorldWidget::initializeGL() {
    ogl = QOpenGLContext::currentContext()->functions();
    ogl->glClearColor(79.f/255.f, 164.f/255.f, 184/255.f, 1.f);
}

void WorldWidget::resizeGL(int w, int h) {
    (void) w;
    (void) h;
}

void WorldWidget::paintGL() {
    ogl->glClear(GL_COLOR_BUFFER_BIT);
}

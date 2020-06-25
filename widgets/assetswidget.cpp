#include "assetswidget.h"
#include "const.h"

AssetsWidget::AssetsWidget()
{
    setMaximumWidth(RIGHT_PANEL_WIDTH);
}

void AssetsWidget::initializeGL() {
    ogl = QOpenGLContext::currentContext()->functions();
    ogl->glClearColor(164.f/255.f, 79.f/255.f, 184/255.f, 1.f);
}

void AssetsWidget::resizeGL(int w, int h) {
    (void) w;
    (void) h;
}

void AssetsWidget::paintGL() {
    ogl->glClear(GL_COLOR_BUFFER_BIT);
}

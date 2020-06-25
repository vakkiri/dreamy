#include <iostream>
#include <QOpenGLFunctions>
#include <QOpenGLContext>
#include "cursorwidget.h"
#include "const.h"

CursorWidget::CursorWidget()
{
    setMaximumWidth(RIGHT_PANEL_WIDTH);
    setMaximumHeight(RIGHT_PANEL_WIDTH);
}

void CursorWidget::initializeGL() {
    ogl = QOpenGLContext::currentContext()->functions();
    ogl->glClearColor(79.f/255.f, 164.f/255.f, 184/255.f, 1.f);
}

void CursorWidget::resizeGL(int w, int h) {
    (void) w;
    (void) h;
}

void CursorWidget::paintGL() {
    ogl->glClear(GL_COLOR_BUFFER_BIT);
}

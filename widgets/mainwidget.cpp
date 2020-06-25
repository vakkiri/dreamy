#include "mainwidget.h"
#include "layouts/mainlayout.h"

MainWidget::MainWidget(QWidget *parent) : QWidget(parent)
{
    layout = new MainLayout();

    setLayout(layout);
}

MainWidget::~MainWidget() {
    delete layout;
}

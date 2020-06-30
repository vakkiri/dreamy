#include <iostream>
#include <QComboBox>

#include "rightlayout.h"

#include "../widgets/cursorwidget.h"
#include "../widgets/assetswidget.h"

RightLayout::RightLayout()
{
    std::cout << "hello i am the right layout" << std::endl;
    cursor_widget = new CursorWidget();
    assets_widget = new AssetsWidget();
    asset_selector = new QComboBox();
    asset_selector->addItem("Tiles");
    addWidget(cursor_widget);
    addWidget(asset_selector);
    addWidget(assets_widget);
}

RightLayout::~RightLayout() {
    delete cursor_widget;
    delete assets_widget;
    delete asset_selector;
}

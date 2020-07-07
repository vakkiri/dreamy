#include <iostream>
#include <QSlider>
#include <QComboBox>
#include "mainlayout.h"
#include "rightlayout.h"
#include "centerlayout.h"
#include "leftlayout.h"
#include "../widgets/cursorwidget.h"
#include "../widgets/assetswidget.h"
#include "../widgets/worldwidget.h"

#include <QLabel>

MainLayout::MainLayout()
{
    std::cout << "hello I am the main layout" << std::endl;
    center_layout = new CenterLayout();
    left_layout = new LeftLayout();
    right_layout = new RightLayout();

    // RIGHT LAYOUT
    cursor_widget = new CursorWidget();
    assets_widget = new AssetsWidget();
    asset_selector = new QComboBox();
    asset_selector->addItem("Tiles");
    quantize_slider = new QSlider(Qt::Orientation::Horizontal);
    quantize_slider->setTickInterval(1);
    quantize_slider->setMaximum(32);
    quantize_slider->setMinimum(1);
    quantize_slider->setMaximumWidth(320);
    quantize_slider->setMinimumWidth(320);
    quantize_slider->setValue(16);
    quantize_label = new QLabel();
    quantize_label->setText(QString("Snap: ") + QString::number(quantize_slider->value()));
    quantize_label->setFixedHeight(16);
    right_layout->addWidget(quantize_label);
    right_layout->addWidget(quantize_slider);
    right_layout->addWidget(cursor_widget);
    right_layout->addWidget(asset_selector);
    right_layout->addWidget(assets_widget);

    cursor_widget->setAssetsWidget(assets_widget);
    assets_widget->setCursorWidget(cursor_widget);

    connect(quantize_slider, &QSlider::valueChanged, cursor_widget, &CursorWidget::setSnap);
    connect(quantize_slider, SIGNAL(valueChanged(int)), this, SLOT (updateQuantizeLabel(int)));

    // CENTER LAYOUT
    world_widget = new WorldWidget();
    world_widget->setCursorWidget(cursor_widget);
    center_layout->addWidget(world_widget);

    addLayout(center_layout, 1, 1);
    addLayout(left_layout, 1, 0);
    addLayout(right_layout, 1, 2);

}

MainLayout::~MainLayout() {
    delete center_layout;
    delete left_layout;
    delete right_layout;

    delete cursor_widget;
    delete assets_widget;
    delete asset_selector;
    delete quantize_label;
    delete quantize_slider;

    delete world_widget;
}

void MainLayout::updateQuantizeLabel(int value) {
    quantize_label->setText(QString("Snap: ") + QString::number(value));
}

#include <iostream>
#include <string>
#include <QSlider>
#include <QCheckBox>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>

#include "mainlayout.h"
#include "leftlayout.h"
#include "../widgets/cursorwidget.h"
#include "../widgets/assetswidget.h"
#include "../widgets/worldwidget.h"
#include "../glo.h"

#include <QLabel>

MainLayout::MainLayout()
{
    center_layout = new QVBoxLayout();
    left_layout = new LeftLayout();
    right_layout = new QVBoxLayout();

    // RIGHT LAYOUT
    cursor_widget = new CursorWidget();
    assets_widget = new AssetsWidget();
    asset_selector = new QComboBox();
    asset_selector->addItem("Tiles");
    asset_selector->addItem("Objects");
    asset_selector->addItem("Scenery");
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
    connect(asset_selector, SIGNAL(currentIndexChanged(int)), assets_widget, SLOT (setGroup(int)));

    // CENTER LAYOUT
    world_widget = new WorldWidget();
    world_widget->setCursorWidget(cursor_widget);
    main_quantize_slider = new QSlider(Qt::Orientation::Horizontal);
    main_quantize_slider->setTickInterval(1);
    main_quantize_slider->setMaximum(32);
    main_quantize_slider->setMinimum(1);
    main_quantize_slider->setValue(16);
    main_quantize_label = new QLabel();
    main_quantize_label->setText(QString("Snap: ") + QString::number(quantize_slider->value()));
    main_quantize_label->setFixedHeight(16);

    layer_slider = new QSlider(Qt::Orientation::Horizontal);
    layer_slider->setTickInterval(1);
    layer_slider->setMaximum(16);
    layer_slider->setMinimum(1);
    layer_slider->setValue(2);
    layer_label = new QLabel();
    layer_label->setText(QString("Layer: ") + QString::number(layer_slider->value()));
    layer_label->setFixedHeight(16);

    tileset_slider = new QSlider(Qt::Orientation::Horizontal);
    tileset_slider->setTickInterval(1);
    tileset_slider->setMaximum(1);
    tileset_slider->setMinimum(0);
    tileset_slider->setValue(tileset);
    tileset_label = new QLabel();
    tileset_label->setText(QString("Tileset: ") + QString::number(tileset_slider->value()));
    tileset_label->setFixedHeight(16);
    tileset_slider->setFixedWidth(32);

    add_solid_box = new QCheckBox("Add &colliders", world_widget);
    view_solid_box = new QCheckBox("&View colliders", world_widget);
    add_solid_box->setChecked(true);
    view_solid_box->setChecked(true);

    center_slider_layout = new QHBoxLayout();
    center_slider_layout->addWidget(main_quantize_label);
    center_slider_layout->addWidget(main_quantize_slider);
    center_slider_layout->addWidget(layer_label);
    center_slider_layout->addWidget(layer_slider);
    center_slider_layout->addWidget(tileset_label);
    center_slider_layout->addWidget(tileset_slider);

    mouse_label = new QLabel(QString("0,0"));
    mouse_label->setFixedHeight(16);

    world_widget->setParentLayout(this);
    center_layout->addLayout(center_slider_layout);
    center_layout->addWidget(add_solid_box);
    center_layout->addWidget(view_solid_box);
    center_layout->addWidget(world_widget);
    center_layout->addWidget(mouse_label);

    connect(main_quantize_slider, &QSlider::valueChanged, world_widget, &WorldWidget::setSnap);
    connect(main_quantize_slider, SIGNAL(valueChanged(int)), this, SLOT (updateMainQuantizeLabel(int)));
    connect(layer_slider, SIGNAL(valueChanged(int)), this, SLOT (updateLayerLabel(int)));
    connect(layer_slider, SIGNAL(valueChanged(int)), world_widget, SLOT (updateCurrentLayer(int)));
    connect(tileset_slider, SIGNAL(valueChanged(int)), this, SLOT (updateTileset(int)));
    connect(add_solid_box, SIGNAL(stateChanged(int)), world_widget, SLOT(updateAddSolid(int)));
    connect(view_solid_box, SIGNAL(stateChanged(int)), world_widget, SLOT(updateViewSolid(int)));

    // All layouts
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

void MainLayout::updateTileset(int value) {
    tileset_label->setText(QString("Tileset: ") + QString::number(value));
    tileset = value;
}

void MainLayout::updateMainQuantizeLabel(int value) {
    main_quantize_label->setText(QString("Snap: ") + QString::number(value));
}

void MainLayout::updateLayerLabel(int value) {
    layer_label->setText(QString("Layer: ") + QString::number(value));
}

WorldWidget* MainLayout::getWorld() {
    return world_widget;
}

void MainLayout::setMouseLabel(float x, float y) {
    std::string s = "x: " + std::to_string(int(x)) + "    y: " + std::to_string(int(y));
    mouse_label->setText(QString(s.c_str()));
}

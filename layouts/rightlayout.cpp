#include <iostream>
#include <QComboBox>
#include <QSlider>
#include <QLabel>

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
    quantize_slider = new QSlider(Qt::Orientation::Horizontal);
    quantize_slider->setTickInterval(1);
    quantize_slider->setMaximum(32);
    quantize_slider->setMinimum(1);
    quantize_slider->setMaximumWidth(320);
    quantize_slider->setMinimumWidth(320);
    quantize_slider->setValue(16);
    quantize_label = new QLabel();
    quantize_label->setText(QString("Snap: ") + QString::number(quantize_slider->value()));
    quantize_label->setFixedHeight(24);
    addWidget(quantize_label);
    addWidget(quantize_slider);
    addWidget(cursor_widget);
    addWidget(asset_selector);
    addWidget(assets_widget);

    cursor_widget->setAssetsWidget(assets_widget);

    connect(quantize_slider, &QSlider::valueChanged, cursor_widget, &CursorWidget::setSnap);
    connect(quantize_slider, SIGNAL(valueChanged(int)), this, SLOT (updateQuantizeLabel(int)));
}

RightLayout::~RightLayout() {
    delete cursor_widget;
    delete assets_widget;
    delete asset_selector;
    delete quantize_label;
    delete quantize_slider;
}

void RightLayout::updateQuantizeLabel(int value) {
    quantize_label->setText(QString("Snap: ") + QString::number(value));
}

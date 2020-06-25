#include <iostream>
#include "leftlayout.h"
#include "../widgets/settingswidget.h"

LeftLayout::LeftLayout()
{
    std::cout << "hello I am the left layout" << std::endl;
    settings_widget = new SettingsWidget();
    addWidget(settings_widget);
}

LeftLayout::~LeftLayout() {

}

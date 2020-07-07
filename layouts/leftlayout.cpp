#include <iostream>
#include "leftlayout.h"
#include "../widgets/settingswidget.h"

LeftLayout::LeftLayout()
{
    settings_widget = new SettingsWidget();
    addWidget(settings_widget);
}

LeftLayout::~LeftLayout() {

}

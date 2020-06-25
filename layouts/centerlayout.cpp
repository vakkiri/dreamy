#include <iostream>
#include "centerlayout.h"
#include "../widgets/worldwidget.h"

CenterLayout::CenterLayout()
{
    std::cout << "hello I am the center layout" << std::endl;
    world_widget = new WorldWidget();
    addWidget(world_widget);
}

CenterLayout::~CenterLayout() {

}

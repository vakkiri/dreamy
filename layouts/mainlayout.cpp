#include <iostream>
#include "mainlayout.h"
#include "rightlayout.h"
#include "centerlayout.h"
#include "leftlayout.h"

MainLayout::MainLayout()
{
    std::cout << "hello I am the main layout" << std::endl;
    center_layout = new CenterLayout();
    left_layout = new LeftLayout();
    right_layout = new RightLayout();
}

MainLayout::~MainLayout() {
    delete center_layout;
    delete left_layout;
    delete right_layout;
}

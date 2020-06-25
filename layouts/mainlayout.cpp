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

    addLayout(center_layout, 1, 1);
    addLayout(left_layout, 1, 0);
    addLayout(right_layout, 1, 2);
}

MainLayout::~MainLayout() {
    delete center_layout;
    delete left_layout;
    delete right_layout;
}

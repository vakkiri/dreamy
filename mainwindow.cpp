#include <iostream>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "widgets/mainwidget.h"
#include "widgets/cursorwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    main_widget = new MainWidget();

    setCentralWidget(main_widget);
    addToolBar("Toolbar");

    cur_filename = "unnamed.lvl";
    cur_filepath = "~/";

    // Actions
    new_action = new QAction(tr("&New"), this);
    new_action->setShortcuts(QKeySequence::New);
    new_action->setStatusTip(tr("Create a new empty level"));
    connect(new_action, &QAction::triggered, this, &MainWindow::new_level);

    open_action = new QAction(tr("&Open"), this);
    open_action->setShortcut(QKeySequence::Open);
    open_action->setStatusTip(tr("Open an existing level"));
    connect(open_action, &QAction::triggered, this, &MainWindow::open);

    save_action = new QAction(tr("&Save"), this);
    save_action->setShortcut(QKeySequence::Save);
    save_action->setStatusTip(tr("Save current level with current filename"));
    connect(save_action, &QAction::triggered, this, &MainWindow::save);

    saveas_action = new QAction(tr("S&ave"), this);
    saveas_action->setShortcut(QKeySequence::SaveAs);
    saveas_action->setStatusTip(tr("Save current level with a new filename"));
    connect(saveas_action, &QAction::triggered, this, &MainWindow::saveas);

    // Menus
    file_menu = menuBar()->addMenu(tr("&File"));
    file_menu->addAction(new_action);
    file_menu->addAction(open_action);
    file_menu->addAction(save_action);
    file_menu->addAction(saveas_action);

}

MainWindow::~MainWindow()
{
    delete ui;
    delete main_widget;
}

void MainWindow::new_level() {
    std::cout << "new level" << std::endl;
}

void MainWindow::open() {
    std::cout << "open" << std::endl;
}

void MainWindow::save() {
    std::cout << "save" << std::endl;
}

void MainWindow::saveas() {
    std::cout << "saveas" << std::endl;
}


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
}

MainWindow::~MainWindow()
{
    delete ui;
    delete main_widget;
}


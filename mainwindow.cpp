#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "layouts/mainlayout.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    layout = new MainLayout();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete layout;
}


#include "mainwindow.h"

#include <QApplication>
#include <QSurface>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle(QString("Dreamy"));

    w.show();
    return a.exec();
}

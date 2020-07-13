#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <vector>
#include "../types/types.h"

class MainLayout;
class WorldWidget;

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();
    WorldWidget* getWorld();

private:
    MainLayout *layout;

};

#endif // MAINWIDGET_H

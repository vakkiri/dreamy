#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

class MainLayout;

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();
signals:
private:
    MainLayout *layout;

};

#endif // MAINWIDGET_H

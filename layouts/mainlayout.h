#ifndef MAINLAYOUT_H
#define MAINLAYOUT_H

#include <QGridLayout>

class CenterLayout;
class RightLayout;
class LeftLayout;

class MainLayout : public QGridLayout
{
public:
    MainLayout();
    virtual ~MainLayout();
private:
    CenterLayout *center_layout;
    LeftLayout *left_layout;
    RightLayout *right_layout;
};

#endif // MAINLAYOUT_H

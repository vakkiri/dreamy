#ifndef CENTERLAYOUT_H
#define CENTERLAYOUT_H

#include <QVBoxLayout>

class WorldWidget;

class CenterLayout : public QVBoxLayout
{
public:
    CenterLayout();
    virtual ~CenterLayout();
private:
    WorldWidget *world_widget;
};

#endif // CENTERLAYOUT_H

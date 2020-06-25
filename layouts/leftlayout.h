#ifndef LEFTLAYOUT_H
#define LEFTLAYOUT_H

#include <QVBoxLayout>

class SettingsWidget;

class LeftLayout : public QVBoxLayout
{
public:
    LeftLayout();
    virtual ~LeftLayout();
private:
    SettingsWidget *settings_widget;
};

#endif // MAINLAYOUT_H

#ifndef MAINLAYOUT_H
#define MAINLAYOUT_H

#include <QGridLayout>

class CenterLayout;
class RightLayout;
class LeftLayout;

class CursorWidget;
class AssetsWidget;
class QComboBox;
class QSlider;
class QLabel;

class WorldWidget;

class MainLayout : public QGridLayout
{
    Q_OBJECT
public:
    MainLayout();
    virtual ~MainLayout();
private:
    CenterLayout *center_layout;
    LeftLayout *left_layout;
    RightLayout *right_layout;

    // right stuff
    CursorWidget *cursor_widget;
    AssetsWidget *assets_widget;
    QComboBox *asset_selector;

    QSlider *quantize_slider;
    QLabel *quantize_label;

    QSlider *main_quantize_slider;
    QLabel *main_quantize_label;

    // center stuff
    WorldWidget *world_widget;


public slots:
    void updateQuantizeLabel(int value);
    void updateMainQuantizeLabel(int value);
};

#endif // MAINLAYOUT_H

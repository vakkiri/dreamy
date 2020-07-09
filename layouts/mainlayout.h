#ifndef MAINLAYOUT_H;
#define MAINLAYOUT_H

#include <QGridLayout>

class QVBoxLayout;
class LeftLayout;

class QCheckBox;
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
    QVBoxLayout *center_layout;
    LeftLayout *left_layout;
    QVBoxLayout *right_layout;

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
    QCheckBox *add_solid_box;
    QCheckBox *view_solid_box;


public slots:
    void updateQuantizeLabel(int value);
    void updateMainQuantizeLabel(int value);
};

#endif // MAINLAYOUT_H

#ifndef MAINLAYOUT_H
#define MAINLAYOUT_H

#include <QGridLayout>
#include <vector>
#include "../types/types.h"

class QVBoxLayout;
class QHBoxLayout;
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
    WorldWidget* getWorld();

    void setMouseLabel(float x, float y);
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

    // center stuff
    WorldWidget *world_widget;
    QCheckBox *add_solid_box;
    QCheckBox *view_solid_box;
    QSlider *main_quantize_slider;
    QLabel *main_quantize_label;
    QSlider *layer_slider;
    QLabel *layer_label;
    QHBoxLayout *center_slider_layout;
    QLabel *mouse_label;

public slots:
    void updateQuantizeLabel(int value);
    void updateMainQuantizeLabel(int value);
    void updateLayerLabel(int value);
};

#endif // MAINLAYOUT_H

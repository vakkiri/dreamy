#ifndef RIGHTLAYOUT_H
#define RIGHTLAYOUT_H

#include <QVBoxLayout>

class CursorWidget;
class AssetsWidget;
class QComboBox;
class QSlider;
class QLabel;

class RightLayout : public QVBoxLayout
{
    Q_OBJECT
public:
    RightLayout();
    virtual ~RightLayout();
private:
    CursorWidget *cursor_widget;
    AssetsWidget *assets_widget;
    QComboBox *asset_selector;

    QSlider *quantize_slider;
    QLabel *quantize_label;

public slots:
    void updateQuantizeLabel(int value);
};

#endif // RIGHTLAYOUT_H

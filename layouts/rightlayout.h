#ifndef RIGHTLAYOUT_H
#define RIGHTLAYOUT_H

#include <QVBoxLayout>

class CursorWidget;
class AssetsWidget;
class QComboBox;

class RightLayout : public QVBoxLayout
{
public:
    RightLayout();
    virtual ~RightLayout();
private:
    CursorWidget *cursor_widget;
    AssetsWidget *assets_widget;
    QComboBox *asset_selector;
};

#endif // RIGHTLAYOUT_H

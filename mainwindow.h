#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <string>

class QMenu;
class QAction;

class MainWidget;
class WorldWidget;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void saveFile(std::string path);

    Ui::MainWindow *ui;
    MainWidget *main_widget;
    WorldWidget *world;

    QMenu *file_menu;

    QAction *new_action;
    QAction *save_action;
    QAction *saveas_action;
    QAction *open_action;

    std::string cur_filename;
    std::string cur_filepath;
private slots:
    void save();
    void open();
    void saveas();
    void new_level();
};
#endif // MAINWINDOW_H

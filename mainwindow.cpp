#include <iostream>
#include <fstream>
#include <cstring>
#include <QFileDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "widgets/mainwidget.h"
#include "widgets/cursorwidget.h"
#include "util/assetcontainer.h"
#include "widgets/worldwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    main_widget = new MainWidget();
    world = main_widget->getWorld();

    setCentralWidget(main_widget);
    addToolBar("Toolbar");

    cur_filename = "unnamed.lvl";
    cur_filepath = "./";

    // Actions
    new_action = new QAction(tr("&New"), this);
    new_action->setShortcuts(QKeySequence::New);
    new_action->setStatusTip(tr("Create a new empty level"));
    connect(new_action, &QAction::triggered, this, &MainWindow::new_level);

    open_action = new QAction(tr("&Open"), this);
    open_action->setShortcut(QKeySequence::Open);
    open_action->setStatusTip(tr("Open an existing level"));
    connect(open_action, &QAction::triggered, this, &MainWindow::open);

    save_action = new QAction(tr("&Save"), this);
    save_action->setShortcut(QKeySequence::Save);
    save_action->setStatusTip(tr("Save current level with current filename"));
    connect(save_action, &QAction::triggered, this, &MainWindow::save);

    saveas_action = new QAction(tr("S&ave"), this);
    saveas_action->setShortcut(QKeySequence::SaveAs);
    saveas_action->setStatusTip(tr("Save current level with a new filename"));
    connect(saveas_action, &QAction::triggered, this, &MainWindow::saveas);

    // Menus
    file_menu = menuBar()->addMenu(tr("&File"));
    file_menu->addAction(new_action);
    file_menu->addAction(open_action);
    file_menu->addAction(save_action);
    file_menu->addAction(saveas_action);

}

MainWindow::~MainWindow()
{
    delete ui;
    delete main_widget;
}

void MainWindow::new_level() {
    world->clearAssets();
}

void MainWindow::open() {
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Files(*.lvl)"));
    std::ifstream file;
    file.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
    std::vector<char> buffer;
    int length;

    try {
        file.open(file_name.toStdString().c_str(), std::ios::binary);
        file.unsetf(std::ios::skipws);
        file.seekg (0, file.end);
        length = file.tellg();
        file.seekg (0, file.beg);
        buffer.reserve(length);

        file.read(buffer.data(), length);

        file.close();

        world->clearAssets();

        int16_t val;
        char* cur = buffer.data();
        std::memcpy(&val, cur, sizeof(int16_t));

        while (val != -1) {
            AssetInstance new_asset;

            if (val == 0 || val == 1) { // tile
                TileSavedAsset tile;
                std::memcpy(&tile, cur, sizeof(tile));

                new_asset.group = "tiles";
                new_asset.x = tile.x;
                new_asset.y = tile.y;
                new_asset.t = editor_assets.get_assets("tiles")[tile.index].t / editor_assets.get_image("tiles").height();
                new_asset.s = editor_assets.get_assets("tiles")[tile.index].s / editor_assets.get_image("tiles").width();
                new_asset.w = editor_assets.get_assets("tiles")[tile.index].w;
                new_asset.h = editor_assets.get_assets("tiles")[tile.index].h;

                new_asset.type = tile.type;


                world->addAsset(new_asset);
                cur += sizeof(tile);
            } else if (val == 2) {      // player
                ObjectSavedAsset player;
                std::memcpy(&player, cur, sizeof(player));

                new_asset.group = "objects";
                new_asset.x = player.x;
                new_asset.y = player.y;
                new_asset.t = editor_assets.get_assets("objects")[0].t; // this is srsly bad lol
                new_asset.s = editor_assets.get_assets("objects")[0].s; // really need a mapping to this info
                new_asset.w = editor_assets.get_assets("objects")[0].w;
                new_asset.h = editor_assets.get_assets("objects")[0].h;
                new_asset.type = player.type;

                world->addAsset(new_asset);

                cur += sizeof(player);
            } else if (val == 3 || val == 4) {      // monster
                ObjectSavedAsset monster;
                std::memcpy(&monster, cur, sizeof(monster));

                new_asset.group = "objects";
                new_asset.x = monster.x;
                new_asset.y = monster.y;
                new_asset.t = editor_assets.get_assets("objects")[val-2].t;
                new_asset.s = editor_assets.get_assets("objects")[val-2].s;
                new_asset.w = editor_assets.get_assets("objects")[val-2].w;
                new_asset.h = editor_assets.get_assets("objects")[val-2].h;
                new_asset.type = monster.type;

                world->addAsset(new_asset);
                cur += sizeof(monster);
            } else {
                std::cout << "Unkown object type: " << val << std::endl;
                break;
            }

            std::memcpy(&val, cur, sizeof(int16_t));
        }

    } catch(std::ofstream::failure e) {
        std::cout << "Couldn't open file to save: " << e.what() << std::endl;
    }
}


void MainWindow::saveFile(std::string path) {
    AssetOutput assetinfo;
    std::vector<AssetInstance>& assets = world->getAssets();
    std::ofstream file;
    file.exceptions ( std::ifstream::failbit | std::ifstream::badbit );

    try {
        file.open(path.c_str(), std::ios::out | std::ios::binary);
        int16_t end_marker = -1;

        for (auto asset : assets) {
            assetinfo.type = int16_t(asset.type);
            assetinfo.x = int16_t(asset.x);
            assetinfo.y = int16_t(asset.y);

            file.write((char*) &assetinfo, sizeof(assetinfo));

            if (asset.type == 0 || asset.type == 1) {
                // the index of a tile is its position in the tilemap, where tiles fit in 16px slots
                int16_t tile_index = int16_t(asset.s * editor_assets.get_image(asset.group).width() / 16.f);
                file.write((char*) &tile_index, sizeof(int16_t));
            }
        }
        file.write((char*) &end_marker, sizeof(end_marker));

        file.close();
    } catch(std::ofstream::failure e) {
        std::cout << "Couldn't open file to save: " << e.what() << std::endl;
    }
}

void MainWindow::save() {
    saveFile(cur_filepath + cur_filename);
}

void MainWindow::saveas() {
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Files(*.lvl)"));
    saveFile(file_name.toStdString());
}


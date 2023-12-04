#pragma once

#include <QMainWindow>
#include <QPushButton>
#include "aspectratiowidget/aspectratiowidget.hpp"
#include "glrenderer.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

private:
    void connect_UI_elements();
    void connect_play_button();
    void connect_reset_button();
    void on_play_scene();
    void on_reset_scene();

    GLRenderer *glRenderer;
    AspectRatioWidget *aspectRatioWidget;
    QPushButton *play_scene;
    QPushButton *reset_scene;
};

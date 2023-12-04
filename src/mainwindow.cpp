#include "mainwindow.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSettings>
#include <QGroupBox>


MainWindow::MainWindow()
{
    glRenderer = new GLRenderer;
    aspectRatioWidget = new AspectRatioWidget(this);
    aspectRatioWidget->setAspectWidget(glRenderer, 3.f/4.f);
    QHBoxLayout *hLayout = new QHBoxLayout;

    QVBoxLayout *vLayout = new QVBoxLayout(); // vertical alignment
    vLayout->setAlignment(Qt::AlignTop);
    hLayout->addLayout(vLayout);
    hLayout->addWidget(aspectRatioWidget, 1);
    this->setLayout(hLayout);

    play_scene = new QPushButton();
    play_scene->setText(QStringLiteral("Play/Pause Scene"));

    reset_scene = new QPushButton();
    reset_scene->setText(QStringLiteral("Reset Scene"));

    vLayout->addWidget(play_scene);
    vLayout->addWidget(reset_scene);

    connect_UI_elements();
}

void MainWindow::connect_UI_elements(){
    connect_play_button();
    connect_reset_button();
}

void MainWindow::connect_play_button(){
    connect(play_scene, &QPushButton::clicked, this, &MainWindow::on_play_scene);
}

void MainWindow::connect_reset_button(){
    connect(reset_scene, &QPushButton::clicked, this, &MainWindow::on_reset_scene);
}

void MainWindow::on_play_scene(){
    glRenderer->play_scene();
}

void MainWindow::on_reset_scene(){
    glRenderer->reset_scene();
}

MainWindow::~MainWindow() {}


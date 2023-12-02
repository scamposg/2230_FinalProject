#include "mainwindow.h"

#include <QHBoxLayout>

MainWindow::MainWindow()
{
    glRenderer = new GLRenderer;

    QHBoxLayout *container = new QHBoxLayout;
    container->addWidget(glRenderer);
    this->setLayout(container);
}

MainWindow::~MainWindow() {}


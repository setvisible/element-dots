/* - ElementDots - Copyright (C) 2017 Sebastien Vavassori
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "about.h"
#include "globals.h"
#include "gamewidget.h"

#include <QtWidgets/QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle(QString("%0 v%1").arg(STR_APPLICATION_NAME).arg(STR_APPLICATION_VERSION));
    this->setWindowIcon(QIcon(":/res/logo/Webalys-Kameleon.pics-Chessboard.ico"));

    connect(ui->radioButton_acid,   SIGNAL(released()), this, SLOT(onRadioChanged()));
    connect(ui->radioButton_air,    SIGNAL(released()), this, SLOT(onRadioChanged()));
    connect(ui->radioButton_earth,  SIGNAL(released()), this, SLOT(onRadioChanged()));
    connect(ui->radioButton_fire,   SIGNAL(released()), this, SLOT(onRadioChanged()));
    connect(ui->radioButton_oil,    SIGNAL(released()), this, SLOT(onRadioChanged()));
    connect(ui->radioButton_plasma, SIGNAL(released()), this, SLOT(onRadioChanged()));
    connect(ui->radioButton_rock,   SIGNAL(released()), this, SLOT(onRadioChanged()));
    connect(ui->radioButton_sand,   SIGNAL(released()), this, SLOT(onRadioChanged()));
    connect(ui->radioButton_water,  SIGNAL(released()), this, SLOT(onRadioChanged()));

    connect(ui->clearButton, SIGNAL(released()), ui->gamewidget, SLOT(clear()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

/***********************************************************************************
 ***********************************************************************************/
void MainWindow::about()
{
    QMessageBox msgBox(QMessageBox::NoIcon, tr("About %0").arg(STR_APPLICATION_NAME), aboutHtml());
    msgBox.exec();
}


void MainWindow::onRadioChanged()
{
    QRadioButton *radio = static_cast<QRadioButton *>(sender());
    if (radio && radio->isChecked()) {
        QString name = radio->objectName();
        name.remove(0, 12); // 12 chars in "radioButton_"
        ui->gamewidget->setCurrentBrush(name);
    }
}

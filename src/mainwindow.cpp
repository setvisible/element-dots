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

    ui->radioButton_acid->setMaterial( Material::Acid );
    ui->radioButton_air->setMaterial( Material::Air );
    ui->radioButton_earth->setMaterial( Material::Earth );
    ui->radioButton_fire->setMaterial( Material::Fire );
    ui->radioButton_oil->setMaterial( Material::Oil );
    ui->radioButton_plasma->setMaterial( Material::Plasma );
    ui->radioButton_rock->setMaterial( Material::Rock );
    ui->radioButton_sand->setMaterial( Material::Sand );
    ui->radioButton_water->setMaterial( Material::Water );

    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));

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
    connect(ui->randomFillButton, SIGNAL(released()), ui->gamewidget, SLOT(fillRandomly()));
    connect(ui->applyOptionButton, SIGNAL(released()), this, SLOT(apply()));
    connect(ui->resetButton, SIGNAL(released()), this, SLOT(reset()));

    this->reset();

    // Enforce the default "Earth" material
    ui->radioButton_earth->setChecked(false);
    ui->radioButton_earth->setChecked(true);
    ui->gamewidget->setCurrentMaterial(Material::Earth);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/***********************************************************************************
 ***********************************************************************************/
void MainWindow::reset()
{
    ui->heightSpinBox->setValue(160);
    ui->widthSpinBox->setValue(160);
    ui->threadsSpinBox->setValue(3);
    apply();
}


void MainWindow::onRadioChanged()
{
    MaterialRadioButton *radio = static_cast<MaterialRadioButton *>(sender());
    if (radio && radio->isChecked()) {
        Material mat = radio->material();
        ui->gamewidget->setCurrentMaterial(mat);
    }
}

void MainWindow::apply()
{
    const int w = ui->widthSpinBox->value();
    const int h = ui->heightSpinBox->value();
    const int threads = ui->threadsSpinBox->value();
    if (w > 0 && h > 0) {
        ui->gamewidget->setWorldSize(w, h);
    }else {
        QMessageBox::warning(this, tr("Error"), tr("The world must have width > 0 and height > 0.") );
    }
    ui->gamewidget->setThreadsNumber(threads);
}

void MainWindow::about()
{
    QMessageBox msgBox(QMessageBox::NoIcon, tr("About %0").arg(STR_APPLICATION_NAME), aboutHtml());
    msgBox.exec();
}

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

#ifndef MATERIAL_RADIO_BUTTON_H
#define MATERIAL_RADIO_BUTTON_H

#include <QtWidgets/QRadioButton>
#include <QtGui/QImage>

#include "gamematerial.h"

class MaterialRadioButton : public QRadioButton
{
    Q_OBJECT

public:
    explicit MaterialRadioButton(QWidget *parent = Q_NULLPTR);
    explicit MaterialRadioButton(const QString &text, QWidget *parent = Q_NULLPTR);
    ~MaterialRadioButton();

    Material material() const;
    void setMaterial(const Material material);

protected:
    bool hitButton(const QPoint &) const Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

private:
    Material m_material;
    QImage imageRadioButton;
    QImage imageRightRadioButton;

    void init();
};

#endif // MATERIAL_RADIO_BUTTON_H

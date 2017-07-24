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

#include "materialradiobutton.h"

#include <QtGui/QPainter>

/* XPM */
static const char * const radiobutton_xpm[] = {
    "4 4 3 1",
    "  c None",
    ". c #000000",
    "+ c #3192D6",
    ".++.",
    "+.+ ",
    ".+.+",
    "+.+."};

/* XPM */
static const char * const radiobutton_right_xpm[] = {
    "4 4 3 1",
    "  c None",
    ". c #000000",
    "+ c #3192D6",
    " ++.",
    "  .+",
    "   .",
    "  +."};

/*!
 * \class MaterialRadioButton
 * \brief The MaterialRadioButton widget provides a material thumbnail with a text label.
 *
 */

MaterialRadioButton::MaterialRadioButton(QWidget *parent) : QRadioButton(parent)
{
    init();
}

MaterialRadioButton::MaterialRadioButton(const QString &text, QWidget *parent) : QRadioButton(parent)
{
    init();
    setText(text);
}

MaterialRadioButton::~MaterialRadioButton()
{
}

void MaterialRadioButton::init()
{
    setMaterial(Material::Air);

    imageRadioButton = QImage(radiobutton_xpm);
    imageRightRadioButton = QImage(radiobutton_right_xpm);

    this->setCursor(Qt::PointingHandCursor);
    this->setMinimumHeight(21);

    QFont font = this->font();
    font.setBold(true);
    font.setPointSizeF(9.0);
    this->setFont(font);

    QSizePolicy policy(QSizePolicy::Preferred, QSizePolicy::Preferred, QSizePolicy::PushButton);
    policy.setHeightForWidth(true);
    this->setSizePolicy(policy);

    this->setIconSize(QSize(20, 20));
}

Material MaterialRadioButton::material() const
{
    return m_material;
}

void MaterialRadioButton::setMaterial(const Material material)
{
    m_material = material;
}

/*!
 *  \reimp
 */
bool MaterialRadioButton::hitButton(const QPoint &pos) const
{
    return this->rect().contains(pos);
}


/*!
 *  \reimp
 */
void MaterialRadioButton::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    int h = iconSize().height();
    QRect pixRect(rect().x(), rect().y() + ((rect().height() - h) / 2), h, h);
    QRect pixRightRect(rect().width()-h+rect().x(), rect().y() + ((rect().height() - h) / 2), h, h);

    QPen pen;
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    if (this->isChecked()) {
        pen.setWidth(3);
        pen.setColor(Qt::black);
        brush.setColor(palette().highlight().color().lighter(200));
    } else {
        pen.setWidth(1);
        pen.setColor(palette().shadow().color());
        brush.setColor(palette().background().color());
    }
    if (this->underMouse()) {
        pen.setColor(palette().highlight().color().lighter(150));
        brush.setColor(palette().highlight().color().lighter(200));
    }
    if (this->isDown()) {
        pen.setColor(palette().highlight().color());
        brush.setColor(palette().highlight().color().lighter(150));
    }

    /* Draw the background */
    {
        p.save();
        p.setBrush(brush);
        p.drawRect(rect());
        p.restore();
    }

    /* Draw the icon */
    {
        p.save();
        const QColor color0 = materialColor(m_material, ColorVariation::Color0);
        const QColor color1 = materialColor(m_material, ColorVariation::Color1);
        imageRadioButton.setColor(1, color0.rgba());
        imageRadioButton.setColor(2, color1.rgba());
        imageRightRadioButton.setColor(1, color0.rgba());
        imageRightRadioButton.setColor(2, color1.rgba());
        p.drawImage(pixRect, imageRadioButton);
        p.drawImage(pixRightRect, imageRightRadioButton);
        p.restore();
    }


    /* Draw the border */
    {
        QRect frame(rect().x(), rect().y(), rect().width()-1, rect().height()-1);
        p.save();
        p.setPen(pen);
        p.drawRect(frame);
        p.restore();
    }

    /* Draw the text */
    if (!text().isEmpty()){
        p.save();
        QRect textRect = rect();
        textRect.setLeft(textRect.left() + iconSize().width() + 4);
        QTextOption to;
        to.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        p.drawText(textRect, text(), to);
        p.restore();
    }

}

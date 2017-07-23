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
#include "utils.h"

#include <QtGui/QPainter>

/*!
 * \class MaterialRadioButton
 * \brief The MaterialRadioButton widget provides a material thumbnail with a text label.
 *
 */

MaterialRadioButton::MaterialRadioButton(QWidget *parent) : QRadioButton(parent)
  , m_brush(Brush::Air)
{
    init();
}

MaterialRadioButton::MaterialRadioButton(const QString &text, QWidget *parent) : QRadioButton(parent)
  , m_brush(Brush::Air)
{
    init();
    setText(text);
}

MaterialRadioButton::~MaterialRadioButton()
{
}

void MaterialRadioButton::init()
{
    setCursor(Qt::PointingHandCursor);
}

Brush MaterialRadioButton::brush() const
{
    return m_brush;
}

void MaterialRadioButton::setBrush(const Brush brush)
{
    m_brush = brush;
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

    QRect r;
    int h = iconSize().height();
    r.setRect(rect().x(), rect().y() + ((rect().height() - h) / 2), h, h);

    QRect innerRect;
    innerRect.setRect(r.x()+1, r.y()+1, r.width()-2, r.height()-2);

    /* Draw the icon */
    {
        p.save();
        QRect pixRect;
        pixRect.setRect( r.x()+2, r.y()+2, r.width()-4, r.height()-4);
        const double value = brushRandomBreakValue(m_brush);
        const QColor color0 = brushColor(m_brush, ColorVariation::Color0);
        const QColor color1 = brushColor(m_brush, ColorVariation::Color1);
        QImage image(pixRect.size(), QImage::Format_RGB32);
        for (int x = 0; x < pixRect.width(); ++x) {
            for (int y = 0; y < pixRect.height(); ++y) {
                image.setPixelColor(x, y, (random() < value) ? color0 : color1);
            }
        }
        p.drawImage(pixRect, image);
        p.restore();
    }

    /* Draw the icon's border */
    {
        p.save();
        p.setPen(palette().dark().color());
        p.drawRect(r);
        p.setPen(palette().base().color());
        p.drawRect(innerRect);
        p.restore();

        if (this->isChecked()) {
            p.save();
            const int thickness = 2;
            QRect borderRect;
            borderRect.setRect( r.x() + thickness/2, r.y() + thickness/2,
                                r.width() - thickness , r.height() - thickness);
            QPen pen;
            pen.setWidth(thickness);
            pen.setColor(palette().foreground().color());
            p.setPen(pen);
            p.drawRect(borderRect);
            p.restore();
        }
    }

    /* Draw the text */
    if (!text().isEmpty()){
        p.save();
        QRect textRect = this->rect();
        textRect.setLeft(textRect.left() + iconSize().width() + 4);
        QFont f = p.font();
        if (this->isDown()) {
            f.setItalic(true);
        }
        if (this->isChecked()) {
            f.setBold(true);
        }
        p.setFont(f);
        QTextOption to;
        to.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        p.drawText(textRect, text(), to);
        p.restore();
    }


}




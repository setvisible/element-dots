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

#include "gamewidget.h"

#include "gameengine.h"
#include "gameworld.h"
#include "utils.h"

#include <QtCore/QDebug>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtCore/qmath.h>


GameWidget::GameWidget(QWidget *parent) : QWidget(parent)
  , m_engine(new GameEngine(this))
{
    setCursor(Qt::CrossCursor);
    m_gridColor = "#000";

    connect(m_engine, SIGNAL(changed()), this, SLOT(update()));
}

GameWidget::~GameWidget()
{
}

void GameWidget::clear()
{
    m_engine->clear();
}

/***********************************************************************************
 ***********************************************************************************/
Material GameWidget::currentMaterial() const
{
    return m_engine->currentMaterial();
}

void GameWidget::setCurrentMaterial(const Material mat)
{
    m_engine->setCurrentMaterial(mat);
}


/***********************************************************************************
 ***********************************************************************************/
void GameWidget::mousePressEvent(QMouseEvent *event)
{
    Q_ASSERT(m_engine);
    GameWorld* world = m_engine->world();
    if (!world) return;

    if (event->buttons() & Qt::LeftButton) {
        const double cellWidth = (double)width()/world->width();
        const double cellHeight = (double)height()/world->height();
        const int posX = qFloor(event->x()/cellWidth);
        const int posY = qFloor(event->y()/cellHeight);

        m_engine->moveMouseTo(posX, posY);

        m_engine->setMousePressed(true);
    }
}

void GameWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_engine->setMousePressed(false);
}

void GameWidget::mouseMoveEvent(QMouseEvent *event)
{
    Q_ASSERT(m_engine);
    GameWorld* world = m_engine->world();
    if (!world) return;

    const double cellWidth = (double)width()/world->width();
    const double cellHeight = (double)height()/world->height();
    const int posX = qFloor(event->x()/cellWidth);
    const int posY = qFloor(event->y()/cellHeight);

    m_engine->moveMouseTo(posX, posY);


}

/***********************************************************************************
 ***********************************************************************************/
void GameWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    paintGrid(p);
    paintUniverse(p);
}

void GameWidget::paintGrid(QPainter &p)
{
    Q_ASSERT(m_engine);
    const GameWorld* world = m_engine->world();
    if (!world) return;

    QRect borders(0, 0, width()-1, height()-1);
    QColor gridColor = m_gridColor;
    gridColor.setAlpha(10);
    p.setPen(gridColor);

    const qreal cellWidth = (qreal)width()/world->width();
    const qreal cellHeight = (qreal)height()/world->height();

    for (int x = world->width(); x >= 0; x--) {
        const int left = qFloor(cellWidth * x);
        p.drawLine(left, 0, left, height());
    }
    for (int y = world->height(); y >= 0; y--) {
        const int top  = qFloor(cellHeight * y);
        p.drawLine(0, top, width(), top);
    }
    p.drawRect(borders);
}

void GameWidget::paintUniverse(QPainter &p)
{
    Q_ASSERT(m_engine);
    GameWorld* world = m_engine->world();
    if (!world) return;

    const qreal cellWidth = (qreal)width()/world->width();
    const qreal cellHeight = (qreal)height()/world->height();

    for (int y = world->height(); y >= 0; y--) {
        for (int x = world->width(); x >= 0; x--) {

            const Material mat = world->dot(x,y);
            const ColorVariation c = world->colorVariation(x,y);

            /* Permute colors to rendering liquid effect */
            const Material mat1 = world->dot(x,y-1);
            const ColorVariation c1 = world->colorVariation(x,y-1);
            if (isLiquid(mat) && mat == mat1 && random()<0.1) {
                if (c1 != c) {
                    world->setColorVariation(x,y,c1);
                    world->setColorVariation(x,y-1,c);
                }
            }

            /* Draw the dot */
            if (mat != Material::Air) {
                const int left = qFloor(cellWidth * x);
                const int top  = qFloor(cellHeight * y);
                const QRect r(left, top, qCeil(cellWidth), qCeil(cellHeight));

                const QColor color1 = materialColor(mat, c);
                p.fillRect(r, QBrush(color1));
            }

        }
    }
}


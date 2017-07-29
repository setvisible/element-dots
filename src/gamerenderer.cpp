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

#include "gamerenderer.h"
#include "gameworld.h"
#include "utils.h"

#include <QtGui/QPainter>
#include <QtCore/qmath.h>

/*! \class GameRenderer
 *  The class GameRenderer contains renderer Helper functions.
 */

void GameRenderer::paintGrid(GameRenderer::Tile &tile, const QColor &gridColor)
{
    QPainter p(&tile.pixmap);

    QSharedPointer<GameWorld> world = tile.world;

    QRect borders(0, 0, tile.totalSize.width()-1, tile.totalSize.height()-1);
    p.setPen(gridColor);

    const qreal cellWidth = (qreal)tile.totalSize.width()/world->width();
    const qreal cellHeight = (qreal)tile.totalSize.height()/world->height();

    for (int x = world->width(); x >= 0; --x) {
        const int left = qFloor(cellWidth * x);
        p.drawLine(left, 0, left, tile.totalSize.height());
    }
    for (int y = world->height(); y >= 0; --y) {
        const int top  = qFloor(cellHeight * y);
        p.drawLine(0, top, tile.totalSize.width(), top);
    }
    p.drawRect(borders);

}

void GameRenderer::paintTile(GameRenderer::Tile &tile)
{
    QSharedPointer<GameWorld> world = tile.world;
    Q_ASSERT( tile.x1 >=0 );
    Q_ASSERT( tile.y1 >=0 );
    Q_ASSERT( tile.x1 < tile.x2 );
    Q_ASSERT( tile.y1 < tile.y2 );
    Q_ASSERT( tile.x2 <= world->width() );
    Q_ASSERT( tile.y2 <= world->height() );

    QPainter p(&tile.pixmap);

    const qreal cellWidth = (qreal)tile.totalSize.width()/world->width();
    const qreal cellHeight = (qreal)tile.totalSize.height()/world->height();


    for (int y = tile.y1; y < tile.y2; ++y) {
        for (int x = tile.x1; x < tile.x2; ++x) {

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
                const int left = qFloor(cellWidth * (x - tile.x1));
                const int top  = qFloor(cellHeight * (y - tile.y1));
                const QRect r(left, top, qCeil(cellWidth), qCeil(cellHeight));

                const QColor color1 = materialColor(mat, c);
                p.fillRect(r, color1);
            }
        }
    }
    return;
}


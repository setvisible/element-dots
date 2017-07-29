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
#include "gamerenderer.h"
#include "perfs.h"

#include <QtCore/QDebug>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QPixmapCache>
#include <QtConcurrent/QtConcurrent>
#include <QtCore/qmath.h>
#include <QtCore/QTime>

CREATE_PERFS_MEASUREMENT(666);

static const char* C_PIXMAP_KEY_DOTS = "big_image_dots";
static const char* C_PIXMAP_KEY_GRID = "big_image_grid";

GameWidget::GameWidget(QWidget *parent) : QWidget(parent)
  , m_engine(new GameEngine(this))
  , m_threads(3)
{
    setCursor(Qt::CrossCursor);
    m_gridColor = "#000";
    m_gridColor.setAlpha(10);

    connect(m_engine, SIGNAL(changed()), this, SLOT(paintFrame()));
    connect(m_engine, SIGNAL(sizeChanged()), this, SLOT(paint()));

    m_engine->setSize(160, 160);
}

GameWidget::~GameWidget()
{
}

void GameWidget::clear()
{
    m_engine->clear();
}

void GameWidget::fillRandomly()
{
    m_engine->fillRandomly();
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
int GameWidget::worldWidth() const
{
    return m_engine->width();
}

int GameWidget::worldHeight() const
{
    return m_engine->height();
}

void GameWidget::setWorldSize(const int width, const int height)
{
    m_engine->setSize(width, height);
}

/***********************************************************************************
 ***********************************************************************************/
int GameWidget::threadsNumber() const
{
    return m_threads;
}

void GameWidget::setThreadsNumber(const int threads)
{
    m_threads = threads;
}

/***********************************************************************************
 ***********************************************************************************/
void GameWidget::mousePressEvent(QMouseEvent *event)
{
    Q_ASSERT(m_engine);
    QSharedPointer<GameWorld> world = m_engine->world();
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
    QSharedPointer<GameWorld> world = m_engine->world();

    const double cellWidth = (double)width()/world->width();
    const double cellHeight = (double)height()/world->height();
    const int posX = qFloor(event->x()/cellWidth);
    const int posY = qFloor(event->y()/cellHeight);

    m_engine->moveMouseTo(posX, posY);
}

/***********************************************************************************
 ***********************************************************************************/
void GameWidget::paintEvent(QPaintEvent *)
{
    /*
     * We store the current frame as a QPixmap in a QPixmapCache.
     * Indeed, paintEvent() is called very often,
     * generally more frequently than the frame rate.
     */

    /*
     * Q: Why do we use QPixmap (rather than QBitmap or QImage)?
     * From the QImage documentation:
     *    QImage is designed and optimized for I/O, and for direct pixel
     *    access and manipulation, while QPixmap is designed and optimized
     *    for showing images on screen."
     */

    PERFS_MEASURE_START(666);

    QPixmap pm;
    if (!QPixmapCache::find(C_PIXMAP_KEY_DOTS, &pm)) {
        pm = generatePixmapDots();
        QPixmapCache::insert(C_PIXMAP_KEY_DOTS, pm);
    }
    QPixmap pgrid;
    if (!QPixmapCache::find(C_PIXMAP_KEY_GRID, &pgrid)) {
        pgrid = generatePixmapGrid();
        QPixmapCache::insert(C_PIXMAP_KEY_GRID, pgrid);
    }

    QPainter widgetPainter(this);
    widgetPainter.drawPixmap(0, 0, pgrid);
    widgetPainter.drawPixmap(0, 0, pm);

    PERFS_MEASURE_STOP(666);
}

void GameWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    paint();
}

/***********************************************************************************
 ***********************************************************************************/
void GameWidget::paint()
{
    QPixmapCache::remove(C_PIXMAP_KEY_DOTS);
    QPixmapCache::remove(C_PIXMAP_KEY_GRID);
    update();
}

void GameWidget::paintFrame()
{
    QPixmapCache::remove(C_PIXMAP_KEY_DOTS);
    update();
}


inline QPixmap GameWidget::generatePixmapGrid()
{
    GameRenderer::Tile tile;
    tile.world =  m_engine->world();
    QPixmap pix(this->width(), this->height());
    pix.fill(this->palette().background().color());
    tile.pixmap = pix;
    tile.totalSize = this->size();
    GameRenderer::paintGrid(tile, m_gridColor);
    return tile.pixmap;
}

inline QPixmap GameWidget::generatePixmapDots()
{
    /*
     * QPainter's methods (drawLine(), drawRect(), fillRect()...)
     * are very expensive. We take advantage of thread concurrency
     * on multi-core machine to make the rendering faster.
     *
     * We use a Map/Reduce Heuristic.
     *
     * The world's surface is divided in several sub-surfaces ("tiles").
     * Each tile is sent to the QtConcurrent's map methods.
     * The Qt's map method sends each tile in a separated thread
     * to be drawn concurrently.
     *
     * Finally we copy the tiles in an unique image during the "reduce" process.
     */

    QPixmap pm(this->size());
    pm.fill(Qt::transparent);

    Q_ASSERT(m_engine && m_engine->world());

    const int count = (m_threads > 0) ? qCeil(qSqrt(m_threads)) + 1 : 1;
    Q_ASSERT(count>0);

    const int tileWidth = qCeil((qreal)m_engine->world()->width()/count);
    const int tileHeight = qCeil((qreal)m_engine->world()->height()/count);
    const qreal pixmapWidth = (qreal)this->width()/count;
    const qreal pixmapHeight = (qreal)this->height()/count;

    // Create a list containing imageCount images.
    QList<GameRenderer::Tile> tiles;
    for (int i = 0; i < count; ++i) {
        for (int j = 0; j < count; ++j) {
            GameRenderer::Tile tile;
            tile.world =  m_engine->world();
            tile.x1 = i*tileWidth;
            tile.y1 = j*tileHeight;
            tile.x2 = qMin((i+1)*tileWidth, m_engine->world()->width());
            tile.y2 = qMin((j+1)*tileHeight, m_engine->world()->height());
            tile.offset = QPoint( qCeil(pixmapWidth*i), qCeil(pixmapHeight*j) );

            QPixmap pix(qCeil(pixmapWidth), qCeil(pixmapHeight));
            pix.fill(Qt::transparent);
            tile.pixmap = pix;

            tile.totalSize = this->size();
            tiles << tile;
        }
    }

    /* Map */
    QtConcurrent::blockingMap(tiles, &GameRenderer::paintTile);

    /* Reduce */
    QPainter collagePainter(&pm);
    foreach (const GameRenderer::Tile &tile, tiles) {
        QRectF source(0.0, 0.0, tile.pixmap.width(), tile.pixmap.height());
        QRectF target(tile.offset.x(), tile.offset.y(), tile.pixmap.width(), tile.pixmap.height());
        collagePainter.drawPixmap(target, tile.pixmap, source);
    }

    return pm;
}

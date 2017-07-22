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

#include "gameworld.h"

#include <QtCore/QDebug>
#include <QtCore/QList>
#include <QtCore/QRectF>
#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtCore/qmath.h>

#define C_WORLD_WIDTH_IN_DOTS   100
#define C_WORLD_HEIGHT_IN_DOTS  100

#define C_EXPLOSION_BLAST_WIDTH_IN_DOTS     3
#define C_EXPLOSION_BLAST_HEIGHT_IN_DOTS   20

#define C_INTERVAL_UPDATE_IN_MILLISECOND    30 // 30ms -> ~33Hz
#define C_INTERVAL_FOUNTAIN_IN_MILLISECOND 200 // 200ms -> 5Hz

static double random() // returned value between 0 and 1
{
    Q_ASSERT(RAND_MAX > 0);
    return (double)(qrand())/RAND_MAX;
}

GameWidget::GameWidget(QWidget *parent) : QWidget(parent)
  , m_updateTimer(new QTimer(this))
  , m_fountainTimer(new QTimer(this))
  , m_world(new GameWorld(this))
  , m_isSpawningDots(false)
  , m_mousePosX(0)
  , m_mousePosY(0)
{
    /* initialize the pseudo-random number generator with a seed value. */
    qsrand(QTime::currentTime().msec());

    /* initialize the game */
    m_world->setSize(C_WORLD_WIDTH_IN_DOTS, C_WORLD_HEIGHT_IN_DOTS);

    m_fountains << Fountain{(int)(0.6 * m_world->width()/2), (int)(m_world->width()/5), Brush::Water};
    m_fountains << Fountain{(int)(1.0 * m_world->width()/2), (int)(m_world->width()/5), Brush::Sand};
    m_fountains << Fountain{(int)(1.4 * m_world->width()/2), (int)(m_world->width()/5), Brush::Oil};

    /* initialize the widget */
    m_currentBrush = Brush::Water;
    m_gridColor = "#000";

    m_updateTimer->setInterval(C_INTERVAL_UPDATE_IN_MILLISECOND);
    connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(updateGame()));
    m_updateTimer->start();

    m_fountainTimer->setInterval(C_INTERVAL_FOUNTAIN_IN_MILLISECOND);
    connect(m_fountainTimer, SIGNAL(timeout()), this, SLOT(spawnFountain()));
    m_fountainTimer->start();

}

GameWidget::~GameWidget()
{
}

/***********************************************************************************
 ***********************************************************************************/
void GameWidget::clear()
{
    m_world->clear();
    update();
}

void GameWidget::startGame()
{
    /// \todo timer->start();
}

void GameWidget::stopGame()
{
    /// \todo timer->stop();
}


/***********************************************************************************
 ***********************************************************************************/
Brush GameWidget::currentBrush() const
{
    return m_currentBrush;
}

void GameWidget::setCurrentBrush(const QString &name)
{
    m_currentBrush = toBrush(name);
}

/***********************************************************************************
 ***********************************************************************************/
void GameWidget::updateGame()
{
    for (int y = m_world->height() - 1; y >= 0; y--) {
        for (int x = 0; x < m_world->width(); x++) {

            /// \todo if (m_worldLock[y * gameAreaSizeWidth + x]==true) continue;

            if ( y >= m_world->height() - 1 ) {
                killDot(x,y);
            }

            const Brush d = m_world->dot(x, y);

            // const Brush dl =m_world_new->dot(x-1, y);
            // const Brush dr =m_world_new->dot(x+1 ,y);
            const Brush dbc = m_world->dot(x, y+1);
            const Brush dtc = m_world->dot(x, y-1);

            switch (d) {
            case Brush::Earth:
            case Brush::Air:
            case Brush::Rock:
                continue;
                break;

            case Brush::Acid:
            {

                if (dbc == Brush::Air) {
                    if (random()<0.9)
                        moveDot(x,y,x,y+1,Brush::Air,d);
                } else if (dbc == Brush::Fire) {
                    moveDot(x,y,x,y+1,Brush::Plasma, Brush::Acid);
                } else if (dbc == Brush::Water) {
                    if (random()<0.7)
                        moveDot(x,y,x,y+1,Brush::Water,d);
                } else if (dbc == Brush::Sand) {
                    if (random()<0.05)
                        killDot(x,y);
                } else if (dbc == Brush::Rock
                           || m_world->dot(x-1,y) == Brush::Rock
                           || m_world->dot(x+1,y) == Brush::Rock) {
                    liquid(x,y,d);
                } else if (dbc != Brush::Air && dbc!=d && random()<0.04) {
                    moveDot(x,y,x,y+1,Brush::Air,d);
                } else if (random()<0.05 && m_world->dot(x+1,y)!=d) {
                    moveDot(x,y,x+1,y,Brush::Air,d);
                } else if (random()<0.05 && m_world->dot(x-1,y)!=d) {
                    moveDot(x,y,x-1,y,Brush::Air,d);
                } else if (dbc == Brush::Oil) {
                    if (random()<0.005)
                        boom(x,y,Brush::Fire);
                } else if (dbc != Brush::Air)  {
                    liquid(x,y,d);
                }

            }
                break;
            case Brush::Fire:
            {
                if (dbc == Brush::Air && random()<0.7) {
                    moveDot(x,y,x,y+1,Brush::Air,d);
                } else if (dtc == Brush::Rock) {
                    killDot(x,y);
                } else if ((dbc == Brush::Oil || dbc == Brush::Acid) && random()<0.5) {
                    addDot(x+1,y-1,d);
                } else if ((dbc == Brush::Oil || dbc == Brush::Acid) && random()<0.5) {
                    addDot(x-1,y-1,d);
                } else if (dbc == Brush::Oil) {
                    if (random()<0.002)
                        killDot(x,y+1);
                    addDot(x,y-10-(20*random()),d);
                    addDot(x,y-1-(10*random()),d);
                } else if (dbc == Brush::Acid) {
                    if (random()<0.1)
                        boom(x,y+1,Brush::Fire);
                } else if (dbc == Brush::Rock && random()<0.03) {
                    killDot(x,y);
                } else if ((dbc == Brush::Air || dbc == Brush::Earth) && random()<0.02) {
                    addDot(x+1,y-1,d);
                } else if ((dbc == Brush::Air || dbc == Brush::Earth) && random()<0.02) {
                    addDot(x-1,y-1,d);
                } else if (dbc == Brush::Earth && random()<0.004) {
                    killDot(x,y+1);
                } else if (dbc==d && random()<0.4) {
                    moveDot(x,y,x,y-2,Brush::Air,d);
                } else if (dtc==d && m_world->dot(x,y-2)==d && m_world->dot(x,y-3)==d) {
                    killDot(x,y);
                }
            }
                break;
            case Brush::Oil:
            {
                if (dbc == Brush::Fire && random()<0.2) {
                    moveDot(x,y,x,y+1,Brush::Fire,Brush::Oil);
                } else if (dbc == Brush::Air) {
                    if (random()<0.7)
                        moveDot(x,y,x,y+1,Brush::Air,d);
                } else if (dbc == Brush::Fire && random()<0.1) {
                    addDot(x,y,Brush::Fire);
                } else if (dbc == Brush::Air && random()<0.05) {
                    addDot(x,y+1,d);
                } else if (dbc != Brush::Air) {
                    liquid(x,y,d);
                }
            }
                break;
            case Brush::Plasma:
            {
                if (random()<0.1)
                    killDot(x,y);
            }
                break;
            case Brush::Sand:
            {
                if (dbc == Brush::Air) {
                    if (random()<0.9)
                        moveDot(x,y,x,y+1,Brush::Air,d);
                } else if (dbc == Brush::Water) {
                    if (random()<0.6)
                        moveDot(x,y,x,y+1,Brush::Water,d);
                } else if (dbc == Brush::Acid) {
                    if (random()<0.1)
                        moveDot(x,y,x,y+1,Brush::Acid,d);
                } else if (dbc == Brush::Oil) {
                    if (random()<0.3)
                        moveDot(x,y,x,y+1,Brush::Oil,d);
                } else if (dbc == Brush::Fire) {
                    killDot(x,y+1);

                } else if (m_world->dot(x-1,y) == Brush::Air && random()<0.01) {
                    moveDot(x,y,x-1,y,Brush::Air,d);
                } else if (m_world->dot(x+1,y) == Brush::Air && random()<0.01) {
                    moveDot(x,y,x+1,y,Brush::Air,d);

                } else if (dbc != Brush::Air
                           && m_world->dot(x+1,y+1) == Brush::Air
                           && m_world->dot(x+1,y) == Brush::Air
                           && random()<0.3) {
                    moveDot(x,y,x+1,y,Brush::Air,d);

                } else if (dbc != Brush::Air
                           && m_world->dot(x+1,y) == Brush::Water
                           && random()<0.3) {
                    moveDot(x,y,x+1,y,Brush::Water,d);

                } else if (dbc != Brush::Air
                           && m_world->dot(x-1,y) == Brush::Water
                           && random()<0.3) {
                    moveDot(x,y,x-1,y,Brush::Water,d);

                } else if (dbc != Brush::Air
                           && m_world->dot(x+1,y) == Brush::Oil
                           && random()<0.3) {
                    moveDot(x,y,x+1,y,Brush::Oil,d);

                } else if (dbc != Brush::Air
                           && m_world->dot(x-1,y) == Brush::Oil
                           && random()<0.3) {
                    moveDot(x,y,x-1,y,Brush::Oil,d);

                } else if (dbc != Brush::Air
                           && m_world->dot(x-1,y) == Brush::Air
                           && m_world->dot(x-1,y+1) == Brush::Air
                           && random()<0.3) {
                    moveDot(x,y,x-1,y,Brush::Air,d);
                }
            }

                break;
            case Brush::Steam:
            {
                if ( dtc != Brush::Earth
                     && dtc != Brush::Rock
                     && dtc != d && random()<0.5) {
                    moveDot(x,y,x,y-1,dtc,d);
                } else if (random()<0.3
                           && dtc != Brush::Air
                           && m_world->dot(x-1,y) == Brush::Air
                           && m_world->dot(x-1,y+1)!=d) {
                    moveDot(x,y,x-1,y,Brush::Air, Brush::Steam);
                } else if (random()<0.3
                           && dtc != Brush::Air
                           && m_world->dot(x+1,y) == Brush::Air
                           && m_world->dot(x+1,y+1)!=d) {
                    moveDot(x,y,x+1,y,Brush::Air, Brush::Steam);
                } else if (random()<0.3
                           && dtc != Brush::Air
                           && m_world->dot(x+2,y) == Brush::Air
                           && m_world->dot(x+2,y+1)!=d) {
                    moveDot(x,y,x+2,y,Brush::Air, Brush::Steam);
                } else if (random()<0.3
                           && dtc != Brush::Air
                           && m_world->dot(x-2,y) == Brush::Air
                           && m_world->dot(x-2,y+1)!=d) {
                    moveDot(x,y,x-2,y,Brush::Air, Brush::Steam);
                }
                if (random()<0.03 || y<1) {
                    killDot(x,y);
                }
            }
                break;
            case Brush::Water:
            {
                if (dbc == Brush::Air) {
                    if (random()<0.95)
                        moveDot(x,y,x,y+1,Brush::Air,d);
                } else if (dbc == Brush::Fire) {
                    moveDot(x,y, x, y+1, Brush::Steam, d);
                } else if (m_world->dot(x+1,y) == Brush::Fire) {
                    addDot(x,y,Brush::Steam);
                    killDot(x+1,y);
                } else if (m_world->dot(x-1,y) == Brush::Fire) {
                    addDot(x, y, Brush::Steam);
                    killDot(x-1, y);
                } else if (dbc==Brush::Oil && random()<0.3) {
                    moveDot(x,y,x,y+1,Brush::Oil,d);
                } else if (dbc==Brush::Acid && random()<0.01) {
                    killDot(x,y+1);
                } else if (m_world->dot(x+1,y)==Brush::Oil && random()<0.1) {
                    moveDot(x+1,y,x,y,d,Brush::Oil);
                } else if (m_world->dot(x-1,y)==Brush::Oil && random()<0.1) {
                    moveDot(x-1,y,x,y,d,Brush::Oil);

                    // } else if (m_world_new->dot(x+1,y)==Brush::Acid && random()<0.4) {
                    //     moveDot(x+1,y,x,y,d,Brush::Acid);
                    // } else if (m_world_new->dot(x-1,y)==Brush::Acid && random()<0.4) {
                    //     moveDot(x-1,y,x,y,d,Brush::Acid);

                } else {
                    liquid(x,y,d);
                }
            }
                break;
            default:
                Q_UNREACHABLE();
                break;
            }
        }
    }
    update();
}

/***********************************************************************************
 ***********************************************************************************/
inline void GameWidget::boom(const int x, const int y, const Brush brush)
{
    for (int i = 0; i < C_EXPLOSION_BLAST_WIDTH_IN_DOTS; ++i) {
        for (int j = 0; j < C_EXPLOSION_BLAST_HEIGHT_IN_DOTS; ++j) {
            addDot(x+i, y-j, brush);
        }
    }
}

inline void GameWidget::liquid(const int x, const int y, const Brush brush)
{
    const Brush r1 = m_world->dot(x+1,y);
    const Brush r2 = m_world->dot(x+2,y);
    const Brush r3 = m_world->dot(x+3,y);
    const Brush l1 = m_world->dot(x-1,y);
    const Brush l2 = m_world->dot(x-2,y);
    const Brush l3 = m_world->dot(x-3,y);

    const int w = ((r1==brush) ? 1 : 0 )
            + ( (r2==brush) ? 1 : 0 )
            + ( (r3==brush) ? 1 : 0 )
            - ( (l1==brush) ? 1 : 0 )
            - ( (l2==brush) ? 1 : 0 )
            - ( (l3==brush) ? 1 : 0 );

    if (w<=0 && random()<0.5) {
        if      (r1==Brush::Air && m_world->dot(x+1,y-1)!=brush) moveDot(x,y,x+1,y,Brush::Air,brush);
        else if (r2==Brush::Air && m_world->dot(x+2,y-1)!=brush) moveDot(x,y,x+2,y,Brush::Air,brush);
        else if (r3==Brush::Air && m_world->dot(x+3,y-1)!=brush) moveDot(x,y,x+3,y,Brush::Air,brush);
    } else if (w>=0 && random()<0.5) {
        if      (l1==Brush::Air && m_world->dot(x-1,y-1)!=brush) moveDot(x,y,x-1,y,Brush::Air,brush);
        else if (l2==Brush::Air && m_world->dot(x-2,y-1)!=brush) moveDot(x,y,x-2,y,Brush::Air,brush);
        else if (l3==Brush::Air && m_world->dot(x-3,y-1)!=brush) moveDot(x,y,x-3,y,Brush::Air,brush);
    }
}

/***********************************************************************************
 ***********************************************************************************/
inline void GameWidget::addDot(const int x, const int y, const Brush brush)
{
    m_world->setDot(x,y,brush);

    ColorVariation c = (random() < brushRandomBreakValue(brush) )
            ? ColorVariation::Color0
            : ColorVariation::Color1;

    m_world->setColorVariation(x,y,c);

}

inline void GameWidget::moveDot(const int x, const int y,
                                const int nx, const int ny,
                                const Brush brush, const Brush nbrush)
{    
    m_world->setDot(x,y,brush);
    m_world->setDot(nx,ny,nbrush);
}

inline void GameWidget::killDot(const int x, const int y)
{
    m_world->setDot(x,y,Brush::Air);
}

/***********************************************************************************
 ***********************************************************************************/
inline void GameWidget::spawnDot(const int x, const int y, const Brush brush)
{
    switch (brush) {
    case Brush::Earth:
    case Brush::Plasma:
    case Brush::Rock:
    {
        const int total = 4;
        for (int i = 0; i <= total; ++i) {
            for (int j = 0; j <= total; ++j) {

                if ((i==0 || i==total) && (j==0 || j==total))
                    continue;

                addDot(x-(total/2)+i, y-(total/2)+j, brush);
            }
        }
    }
        break;
    case Brush::Acid:
    case Brush::Air:
    case Brush::Fire:
    case Brush::Oil:
    case Brush::Sand:
    case Brush::Steam:
    case Brush::Water:
        addDot(x, y, brush);

        break;
    default:
        Q_UNREACHABLE();
        break;
    }
}

void GameWidget::spawnFountain()
{
    for (int i = 0; i < m_fountains.count(); i++) {
        spawnDot(m_fountains.at(i).x + 2, m_fountains.at(i).y + 4, m_fountains.at(i).type);
    }

    if (m_isSpawningDots) {
        spawnDot(m_mousePosX, m_mousePosY, m_currentBrush);
    }
    update();
}


/***********************************************************************************
 ***********************************************************************************/

void GameWidget::mousePressEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) == Qt::LeftButton) {
        m_isSpawningDots = true;
    }

    double cellWidth = (double)width()/m_world->width();
    double cellHeight = (double)height()/m_world->height();
    m_mousePosY = qFloor(event->y()/cellHeight);
    m_mousePosX = qFloor(event->x()/cellWidth);

    spawnFountain();
}

void GameWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    m_isSpawningDots = false;
}

void GameWidget::mouseMoveEvent(QMouseEvent *event)
{
    double cellWidth = (double)width()/m_world->width();
    double cellHeight = (double)height()/m_world->height();

    m_mousePosY = qFloor(event->y()/cellHeight);
    m_mousePosX = qFloor(event->x()/cellWidth);
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
    QRect borders(0, 0, width()-1, height()-1);
    QColor gridColor = m_gridColor;
    gridColor.setAlpha(10);
    p.setPen(gridColor);
    double cellWidth = (double)width()/m_world->width();
    for(double k = cellWidth; k <= width(); k += cellWidth) {
        p.drawLine(k, 0, k, height());
    }
    double cellHeight = (double)height()/m_world->height();
    for(double k = cellHeight; k <= height(); k += cellHeight) {
        p.drawLine(0, k, width(), k);
    }
    p.drawRect(borders);
}

void GameWidget::paintUniverse(QPainter &p)
{
    double cellWidth = (double)width()/m_world->width();
    double cellHeight = (double)height()/m_world->height();

    for (int y = m_world->height() - 1; y >= 0; y--) {
        for (int x = 0; x < m_world->width(); x++) {

            Brush brush = m_world->dot(x,y);
            ColorVariation c = m_world->colorVariation(x,y);

            /// \todo Draw only new dots ?

            Q_ASSERT(brush >= Brush::Acid && brush <= Brush::Water);

            if (brush != Brush::Air) {
                qreal left = (qreal)(cellWidth*x);
                qreal top  = (qreal)(cellHeight*y);
                QRectF r(left, top, (qreal)cellWidth, (qreal)cellHeight);

                QColor color1 = brushColor(brush, c);
                p.fillRect(r, QBrush(color1));
            }

        }
    }
}


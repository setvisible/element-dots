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

#include <QtCore/QDebug>
#include <QtCore/QList>
#include <QtCore/QRectF>
#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtCore/qmath.h>

/*! \class GameWidget
 *
 * \subsection sec-coord-sys Coordinate System
 *
 * The coordinates in the widget are oriented as below:
 *
 * \code
 *     0 1 2 3 4   width=5
 *   0 +--------> x
 *   1 |
 *   2 |
 *   3 |
 *     V y   height=4
 * \endcode
 *
 */

static double random() // returned value between 0 and 1
{
    Q_ASSERT(RAND_MAX > 0);
    return (double)(qrand())/RAND_MAX;
}


GameWidget::GameWidget(QWidget *parent) : QWidget(parent)
  , m_updateTimer(new QTimer(this))
  , m_fountainTimer(new QTimer(this))
  , m_world(Q_NULLPTR)
  , m_world_old(Q_NULLPTR)
  , m_world_lock(Q_NULLPTR)
  , m_isSpawningDots(false)
  , m_mousePosX(0)
  , m_mousePosY(0)
{
    /* initialize the pseudo-random number generator with a seed value. */
    qsrand(QTime::currentTime().msec());

    m_gameAreaSizeHeight =  100;
    m_gameAreaSizeWidth = 100;
    m_currentBrush = Brush::Water;
    m_gridColor = "#000";


    m_updateTimer->setInterval(30); // 30 msec (~33Hz)
    connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(updateGame()));
    m_updateTimer->start();

    m_fountainTimer->setInterval(200); // 200 msec (5Hz)
    connect(m_fountainTimer, SIGNAL(timeout()), this, SLOT(spawnFountain()));
    m_fountainTimer->start();


    m_world     = new char[(m_gameAreaSizeHeight) * (m_gameAreaSizeWidth)];
    m_world_old = new char[(m_gameAreaSizeHeight) * (m_gameAreaSizeWidth)];
    m_world_lock = new bool[(m_gameAreaSizeHeight) * (m_gameAreaSizeWidth)];

    memset(m_world    , (char)Brush::Air, sizeof(char)*(m_gameAreaSizeHeight) * (m_gameAreaSizeWidth));
    memset(m_world_old, (char)Brush::Air, sizeof(char)*(m_gameAreaSizeHeight) * (m_gameAreaSizeWidth));
    memset(m_world_lock, false, sizeof(bool)*(m_gameAreaSizeHeight) * (m_gameAreaSizeWidth));

    m_spawn << Fountain{(int)(0.6 * m_gameAreaSizeWidth/2), (int)(m_gameAreaSizeWidth/5), Brush::Water};
    m_spawn << Fountain{(int)(1.0 * m_gameAreaSizeWidth/2), (int)(m_gameAreaSizeWidth/5), Brush::Sand};
    m_spawn << Fountain{(int)(1.4 * m_gameAreaSizeWidth/2), (int)(m_gameAreaSizeWidth/5), Brush::Oil};

}

GameWidget::~GameWidget()
{
    if (m_world     ) delete [] m_world;
    if (m_world_old ) delete [] m_world_old;
    if (m_world_lock) delete [] m_world_lock;
}

/***********************************************************************************
 ***********************************************************************************/
void GameWidget::clear()
{
    if (m_world     ) delete [] m_world;
    if (m_world_old ) delete [] m_world_old;
    if (m_world_lock) delete [] m_world_lock;

    m_world     = new char[(m_gameAreaSizeHeight) * (m_gameAreaSizeWidth)];
    m_world_old = new char[(m_gameAreaSizeHeight) * (m_gameAreaSizeWidth)];
    m_world_lock = new bool[(m_gameAreaSizeHeight) * (m_gameAreaSizeWidth)];

    memset(m_world    , (char)Brush::Air, sizeof(char)*(m_gameAreaSizeHeight) * (m_gameAreaSizeWidth));
    memset(m_world_old, (char)Brush::Air, sizeof(char)*(m_gameAreaSizeHeight) * (m_gameAreaSizeWidth));
    memset(m_world_lock, false, sizeof(bool)*(m_gameAreaSizeHeight) * (m_gameAreaSizeWidth));

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
void GameWidget::setCurrentBrush(const QString &name)
{
    if (      name == QLatin1String("acid"   ) ) { m_currentBrush = Brush::Acid   ; }
    else if ( name == QLatin1String("air"    ) ) { m_currentBrush = Brush::Air    ; }
    else if ( name == QLatin1String("earth"  ) ) { m_currentBrush = Brush::Earth  ; }
    else if ( name == QLatin1String("fire"   ) ) { m_currentBrush = Brush::Fire   ; }
    else if ( name == QLatin1String("oil"    ) ) { m_currentBrush = Brush::Oil    ; }
    else if ( name == QLatin1String("plasma" ) ) { m_currentBrush = Brush::Plasma ; }
    else if ( name == QLatin1String("rock"   ) ) { m_currentBrush = Brush::Rock   ; }
    else if ( name == QLatin1String("sand"   ) ) { m_currentBrush = Brush::Sand   ; }
    else if ( name == QLatin1String("steam"  ) ) { m_currentBrush = Brush::Steam  ; }
    else if ( name == QLatin1String("water"  ) ) { m_currentBrush = Brush::Water  ; }
    else {
        Q_UNREACHABLE();
        m_currentBrush = Brush::Water;
    }
}

QString GameWidget::color(const Brush d) const
{
    QString ret;
    switch (d) {
    case Brush::Acid:   ret = (random()<0.9) ? QLatin1String("#f1f") : QLatin1String("#e4e"); break;
    case Brush::Air:    ret = QLatin1String("#fff"); break;
    case Brush::Earth:  ret = (random()<0.5) ? QLatin1String("#0b0") : QLatin1String("#2d2"); break;
    case Brush::Fire:   ret = (random()<0.5) ? QLatin1String("#c41") : QLatin1String("#f60"); break;
    case Brush::Oil:    ret = (random()<0.7) ? QLatin1String("#212") : QLatin1String("#111"); break;
    case Brush::Plasma: ret = (random()<0.7) ? QLatin1String("#f3a") : QLatin1String("#ee0"); break;
    case Brush::Rock:   ret = (random()<0.5) ? QLatin1String("#777") : QLatin1String("#666"); break;
    case Brush::Sand:   ret = (random()<0.6) ? QLatin1String("#b73") : QLatin1String("#a82"); break;
    case Brush::Steam:  ret = (random()<0.5) ? QLatin1String("#bbd") : QLatin1String("#ccc"); break;
    case Brush::Water:  ret = (random()<0.5) ? QLatin1String("#12d") : QLatin1String("#12f"); break;
    default:
        Q_UNREACHABLE();
        break;
    }
    return ret;
}

/***********************************************************************************
 ***********************************************************************************/
void GameWidget::updateGame()
{
    for (int y = m_gameAreaSizeHeight - 1; y >= 0; y--) {
        for (int x = 0; x < m_gameAreaSizeWidth; x++) {

            /// \todo if (m_worldLock[y * gameAreaSizeWidth + x]==true) continue;

            if ( y >= m_gameAreaSizeHeight - 1 ) {
                killDot(x,y);
            }

            const Brush d = dot(x, y);

            // const Brush dl = dot(x-1, y);
            // const Brush dr = dot(x+1 ,y);
            const Brush dbc = dot(x, y+1);
            const Brush dtc = dot(x, y-1);

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
                           || dot(x-1,y) == Brush::Rock
                           || dot(x+1,y) == Brush::Rock) {
                    liquid(x,y,d);
                } else if (dbc != Brush::Air && dbc!=d && random()<0.04) {
                    moveDot(x,y,x,y+1,Brush::Air,d);
                } else if (random()<0.05 && dot(x+1,y)!=d) {
                    moveDot(x,y,x+1,y,Brush::Air,d);
                } else if (random()<0.05 && dot(x-1,y)!=d) {
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
                } else if (dtc==d && dot(x,y-2)==d && dot(x,y-3)==d) {
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

                } else if (dot(x-1,y) == Brush::Air && random()<0.01) {
                    moveDot(x,y,x-1,y,Brush::Air,d);
                } else if (dot(x+1,y) == Brush::Air && random()<0.01) {
                    moveDot(x,y,x+1,y,Brush::Air,d);

                } else if (dbc != Brush::Air
                           && dot(x+1,y+1) == Brush::Air
                           && dot(x+1,y) == Brush::Air
                           && random()<0.3) {
                    moveDot(x,y,x+1,y,Brush::Air,d);

                } else if (dbc != Brush::Air
                           && dot(x+1,y) == Brush::Water
                           && random()<0.3) {
                    moveDot(x,y,x+1,y,Brush::Water,d);

                } else if (dbc != Brush::Air
                           && dot(x-1,y) == Brush::Water
                           && random()<0.3) {
                    moveDot(x,y,x-1,y,Brush::Water,d);

                } else if (dbc != Brush::Air
                           && dot(x+1,y) == Brush::Oil
                           && random()<0.3) {
                    moveDot(x,y,x+1,y,Brush::Oil,d);

                } else if (dbc != Brush::Air
                           && dot(x-1,y) == Brush::Oil
                           && random()<0.3) {
                    moveDot(x,y,x-1,y,Brush::Oil,d);

                } else if (dbc != Brush::Air
                           && dot(x-1,y) == Brush::Air
                           && dot(x-1,y+1) == Brush::Air
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
                           && dot(x-1,y) == Brush::Air
                           && dot(x-1,y+1)!=d) {
                    moveDot(x,y,x-1,y,Brush::Air, Brush::Steam);
                } else if (random()<0.3
                           && dtc != Brush::Air
                           && dot(x+1,y) == Brush::Air
                           && dot(x+1,y+1)!=d) {
                    moveDot(x,y,x+1,y,Brush::Air, Brush::Steam);
                } else if (random()<0.3
                           && dtc != Brush::Air
                           && dot(x+2,y) == Brush::Air
                           && dot(x+2,y+1)!=d) {
                    moveDot(x,y,x+2,y,Brush::Air, Brush::Steam);
                } else if (random()<0.3
                           && dtc != Brush::Air
                           && dot(x-2,y) == Brush::Air
                           && dot(x-2,y+1)!=d) {
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
                } else if (dot(x+1,y) == Brush::Fire) {
                    addDot(x,y,Brush::Steam);
                    killDot(x+1,y);
                } else if (dot(x-1,y) == Brush::Fire) {
                    addDot(x, y, Brush::Steam);
                    killDot(x-1, y);
                } else if (dbc==Brush::Oil && random()<0.3) {
                    moveDot(x,y,x,y+1,Brush::Oil,d);
                } else if (dbc==Brush::Acid && random()<0.01) {
                    killDot(x,y+1);
                } else if (dot(x+1,y)==Brush::Oil && random()<0.1) {
                    moveDot(x+1,y,x,y,d,Brush::Oil);
                } else if (dot(x-1,y)==Brush::Oil && random()<0.1) {
                    moveDot(x-1,y,x,y,d,Brush::Oil);

                    // } else if (dot(x+1,y)==Brush::Acid && random()<0.4) {
                    //     moveDot(x+1,y,x,y,d,Brush::Acid);
                    // } else if (dot(x-1,y)==Brush::Acid && random()<0.4) {
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
void GameWidget::boom(const int x, const int y, const Brush brush)
{
    const int width = 3;
    const int height = 20;
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            int index = (y-j) * m_gameAreaSizeWidth + (x+i);
            if (index >= 0 && index < m_gameAreaSizeWidth * m_gameAreaSizeHeight) {
                m_world[index] = (char)brush;
            }
        }
    }
}

void GameWidget::liquid(const int x, const int y, const Brush brush)
{
    const Brush r1 = dot(x+1,y);
    const Brush r2 = dot(x+2,y);
    const Brush r3 = dot(x+3,y);
    const Brush l1 = dot(x-1,y);
    const Brush l2 = dot(x-2,y);
    const Brush l3 = dot(x-3,y);

    const int w = ((r1==brush) ? 1 : 0 )
            + ( (r2==brush) ? 1 : 0 )
            + ( (r3==brush) ? 1 : 0 )
            - ( (l1==brush) ? 1 : 0 )
            - ( (l2==brush) ? 1 : 0 )
            - ( (l3==brush) ? 1 : 0 );

    if (w<=0 && random()<0.5) {
        if      (r1==Brush::Air && dot(x+1,y-1)!=brush) moveDot(x,y,x+1,y,Brush::Air,brush);
        else if (r2==Brush::Air && dot(x+2,y-1)!=brush) moveDot(x,y,x+2,y,Brush::Air,brush);
        else if (r3==Brush::Air && dot(x+3,y-1)!=brush) moveDot(x,y,x+3,y,Brush::Air,brush);
    } else if (w>=0 && random()<0.5) {
        if      (l1==Brush::Air && dot(x-1,y-1)!=brush) moveDot(x,y,x-1,y,Brush::Air,brush);
        else if (l2==Brush::Air && dot(x-2,y-1)!=brush) moveDot(x,y,x-2,y,Brush::Air,brush);
        else if (l3==Brush::Air && dot(x-3,y-1)!=brush) moveDot(x,y,x-3,y,Brush::Air,brush);
    }
}

/***********************************************************************************
 ***********************************************************************************/
void GameWidget::addDot(const int x, const int y, const Brush brush)
{
    const int index = y * m_gameAreaSizeWidth + x;
    if (index >= 0 && index < m_gameAreaSizeWidth * m_gameAreaSizeHeight) {
        m_world[index] = (char)brush;
        m_world_lock[index] = true;
    }
}

void GameWidget::moveDot(const int x, const int y,
                         const int nx, const int ny,
                         const Brush brush, const Brush nbrush)
{    
    const int index = y * m_gameAreaSizeWidth + x;
    const int index_n = ny * m_gameAreaSizeWidth + nx;
    if (index >= 0 && index_n >= 0
            && index < m_gameAreaSizeWidth * m_gameAreaSizeHeight
            && index_n < m_gameAreaSizeWidth * m_gameAreaSizeHeight) {
        m_world[ index ] = (char)brush;
        m_world_lock[ index ] = (brush!=Brush::Air);
        m_world[ index_n ] = (char)nbrush;
        m_world_lock[ index_n ] = true;
    }
}

void GameWidget::killDot(const int x, const int y)
{
    const int index = y * m_gameAreaSizeWidth + x;
    if (index >= 0 && index < m_gameAreaSizeWidth * m_gameAreaSizeHeight) {
        m_world[index] = (char)Brush::Air;
        m_world_lock[index] = false;
    }
}

/***********************************************************************************
 ***********************************************************************************/
void GameWidget::spawnDot(const int x, const int y, const Brush brush)
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
    for (int i = 0; i < m_spawn.count(); i++) {
        spawnDot(m_spawn.at(i).x + 2, m_spawn.at(i).y + 4, m_spawn.at(i).type);
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

    double cellWidth = (double)width()/m_gameAreaSizeWidth;
    double cellHeight = (double)height()/m_gameAreaSizeHeight;
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
    double cellWidth = (double)width()/m_gameAreaSizeWidth;
    double cellHeight = (double)height()/m_gameAreaSizeHeight;

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

GameWidget::Brush GameWidget::dot(const int x, const int y) const
{
    return (Brush)m_world[ y * m_gameAreaSizeWidth + x ];
}


void GameWidget::paintGrid(QPainter &p)
{
    QRect borders(0, 0, width()-1, height()-1);
    QColor gridColor = m_gridColor;
    gridColor.setAlpha(10);
    p.setPen(gridColor);
    double cellWidth = (double)width()/m_gameAreaSizeWidth;
    for(double k = cellWidth; k <= width(); k += cellWidth) {
        p.drawLine(k, 0, k, height());
    }
    double cellHeight = (double)height()/m_gameAreaSizeHeight;
    for(double k = cellHeight; k <= height(); k += cellHeight) {
        p.drawLine(0, k, width(), k);
    }
    p.drawRect(borders);
}

void GameWidget::paintUniverse(QPainter &p)
{
    double cellWidth = (double)width()/m_gameAreaSizeWidth;
    double cellHeight = (double)height()/m_gameAreaSizeHeight;

    for (int y = m_gameAreaSizeHeight - 1; y >= 0; y--) {
        for (int x = 0; x < m_gameAreaSizeWidth; x++) {

            Brush brush = dot(x,y);

            /// \todo Draw only new dots

            Q_ASSERT(brush >= Brush::Acid && brush <= Brush::Water);

            if (brush != Brush::Air) {
                qreal left = (qreal)(cellWidth*x);
                qreal top  = (qreal)(cellHeight*y);
                QRectF r(left, top, (qreal)cellWidth, (qreal)cellHeight);

                QColor color1 = color(brush);
                p.fillRect(r, QBrush(color1));
            }

            m_world_old[ y * m_gameAreaSizeWidth + x] = (char)brush;
            m_world_lock[ y * m_gameAreaSizeWidth + x] = false;

        }
    }
}


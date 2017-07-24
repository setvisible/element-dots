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

#include "gameengine.h"
#include "gameworld.h"
#include "utils.h" // random()

#include <QtCore/QDebug>
#include <QtCore/QList>
#include <QtCore/QRectF>
#include <QtCore/QTimer>
#include <QtCore/qmath.h>

#define C_WORLD_WIDTH_IN_DOTS   150
#define C_WORLD_HEIGHT_IN_DOTS  150

#define C_EXPLOSION_BLAST_WIDTH_IN_DOTS     3
#define C_EXPLOSION_BLAST_HEIGHT_IN_DOTS   20

#define C_INTERVAL_UPDATE_IN_MILLISECOND    30 // 30ms -> ~33Hz
#define C_INTERVAL_FOUNTAIN_IN_MILLISECOND 100 // 200ms -> 5Hz


/*! \class GameEngine
 * \brief The class GameEngine contains the game scene and the game physics.
 *
 * The GameEngine is in charge to update the game scene at some time interval.
 * A scene at a specific time is called a frame.
 * To update the scene, use the frame() method.
 *
 * Internally, the GameEngine delegates the frame calculation to a WorkerThread.
 * The WorkerThread uses a shared memory space to access the current scene's frame,
 * instead of a copy of the scene.
 *
 * The game logics requires the current frame to calculate the next frame,
 * but the next frame can be directly overwritten the current frame.
 *
 * \remark The GameWorld contains methods to access the game scene.
 *
 * \sa GameWidget, WorkerThread
 */


static bool isSolid(const Material material)
{
    return (material==Material::Earth || material==Material::Plasma || material==Material::Rock);
}

GameEngine::GameEngine(QObject *parent) : QObject(parent)
  , m_world(new GameWorld(this))
  , m_updateTimer(new QTimer(this))
  , m_fountainTimer(new QTimer(this))
  , m_isMousePressed(false)
  , m_mousePosX(0)
  , m_mousePosY(0)
  , m_currentMaterial(Material::Water)
{
    /* initialize the game */
    m_world->setSize(C_WORLD_WIDTH_IN_DOTS, C_WORLD_HEIGHT_IN_DOTS);

    m_fountains << Fountain{(int)(0.6 * m_world->width()/2), (int)(m_world->width()/5), Material::Water};
    m_fountains << Fountain{(int)(1.0 * m_world->width()/2), (int)(m_world->width()/5), Material::Sand};
    m_fountains << Fountain{(int)(1.4 * m_world->width()/2), (int)(m_world->width()/5), Material::Oil};

    /* initialize the timers */
    m_updateTimer->setInterval(C_INTERVAL_UPDATE_IN_MILLISECOND);
    connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(updateGame()));
    m_updateTimer->start();

    m_fountainTimer->setInterval(C_INTERVAL_FOUNTAIN_IN_MILLISECOND);
    connect(m_fountainTimer, SIGNAL(timeout()), this, SLOT(spawnFountain()));
    m_fountainTimer->start();

}

GameEngine::~GameEngine()
{
}

void GameEngine::clear()
{
    // stop the worker

    m_world->clear();
    emit changed();
}

/***********************************************************************************
 ***********************************************************************************/
Material GameEngine::currentMaterial() const
{
    return m_currentMaterial;
}

void GameEngine::setCurrentMaterial(const Material material)
{
    m_currentMaterial = material;
}

GameWorld* GameEngine::world() const
{
    return m_world;
}

/***********************************************************************************
 ***********************************************************************************/
void GameEngine::setMousePressed(const bool pressed)
{
    m_isMousePressed = pressed;
    if (isSolid(m_currentMaterial)) {
        spawnMouse();
    }
}

void GameEngine::moveMouseTo(const int posX, const int posY)
{
    if (m_isMousePressed && isSolid(m_currentMaterial)) {
        const double dx = posX - m_mousePosX;
        const double dy = posY - m_mousePosY;
        const double length = std::sqrt(std::pow(dx, 2) + std::pow(dy, 2));

        for (int i = 0; i < qCeil(length); ++i) {
            const double pc = (double)i/length;
            const int xx = m_mousePosX + dx*pc;
            const int yy = m_mousePosY + dy*pc;
            spawnDot(xx, yy, m_currentMaterial);
        }
    }
    m_mousePosX = posX;
    m_mousePosY = posY;
}


/***********************************************************************************
 ***********************************************************************************/
void GameEngine::updateGame()
{
    for (int y = m_world->height()-1; y >= 0; y--) {
        for (int x = 0; x < m_world->width(); x++) {

            /// \todo if (m_worldLock[y * gameAreaSizeWidth + x]==true) continue;

            if ( y >= m_world->height() ) {
                killDot(x,y);
            }

            const Material d = m_world->dot(x, y);
            const Material dbc = m_world->dot(x, y+1);
            const Material dtc = m_world->dot(x, y-1);

            switch (d) {
            case Material::Earth:
            case Material::Air:
            case Material::Rock:
                continue;
                break;

            case Material::Acid:
            {

                if (dbc == Material::Air) {
                    if (random()<0.9)
                        moveDot(x,y,x,y+1,Material::Air,d);
                } else if (dbc == Material::Fire) {
                    moveDot(x,y,x,y+1,Material::Plasma, Material::Acid);
                } else if (dbc == Material::Water) {
                    if (random()<0.7)
                        moveDot(x,y,x,y+1,Material::Water,d);
                } else if (dbc == Material::Sand) {
                    if (random()<0.05)
                        killDot(x,y);
                } else if (dbc == Material::Rock
                           || m_world->dot(x-1,y) == Material::Rock
                           || m_world->dot(x+1,y) == Material::Rock) {
                    liquid(x,y,d);
                } else if (dbc != Material::Air && dbc!=d && random()<0.04) {
                    moveDot(x,y,x,y+1,Material::Air,d);
                } else if (random()<0.05 && m_world->dot(x+1,y)!=d) {
                    moveDot(x,y,x+1,y,Material::Air,d);
                } else if (random()<0.05 && m_world->dot(x-1,y)!=d) {
                    moveDot(x,y,x-1,y,Material::Air,d);
                } else if (dbc == Material::Oil) {
                    if (random()<0.005)
                        boom(x,y,Material::Fire);
                } else if (dbc != Material::Air)  {
                    liquid(x,y,d);
                }

            }
                break;
            case Material::Fire:
            {
                if (dbc == Material::Air && random()<0.7) {
                    moveDot(x,y,x,y+1,Material::Air,d);
                } else if (dtc == Material::Rock) {
                    killDot(x,y);
                } else if ((dbc == Material::Oil || dbc == Material::Acid) && random()<0.5) {
                    addDot(x+1,y-1,d);
                } else if ((dbc == Material::Oil || dbc == Material::Acid) && random()<0.5) {
                    addDot(x-1,y-1,d);
                } else if (dbc == Material::Oil) {
                    if (random()<0.002)
                        killDot(x,y+1);
                    addDot(x,y-10-(20*random()),d);
                    addDot(x,y-1-(10*random()),d);
                } else if (dbc == Material::Acid) {
                    if (random()<0.1)
                        boom(x,y+1,Material::Fire);
                } else if (dbc == Material::Rock && random()<0.03) {
                    killDot(x,y);
                } else if ((dbc == Material::Air || dbc == Material::Earth) && random()<0.02) {
                    addDot(x+1,y-1,d);
                } else if ((dbc == Material::Air || dbc == Material::Earth) && random()<0.02) {
                    addDot(x-1,y-1,d);
                } else if (dbc == Material::Earth && random()<0.004) {
                    killDot(x,y+1);
                } else if (dbc==d && random()<0.4) {
                    moveDot(x,y,x,y-2,Material::Air,d);
                } else if (dtc==d && m_world->dot(x,y-2)==d && m_world->dot(x,y-3)==d) {
                    killDot(x,y);
                }
            }
                break;
            case Material::Oil:
            {
                if (dbc == Material::Fire && random()<0.2) {
                    moveDot(x,y,x,y+1,Material::Fire,Material::Oil);
                } else if (dbc == Material::Air) {
                    if (random()<0.7)
                        moveDot(x,y,x,y+1,Material::Air,d);
                } else if (dbc == Material::Fire && random()<0.1) {
                    addDot(x,y,Material::Fire);
                } else if (dbc == Material::Air && random()<0.05) {
                    addDot(x,y+1,d);
                } else if (dbc != Material::Air) {
                    liquid(x,y,d);
                }
            }
                break;
            case Material::Plasma:
            {
                if (random()<0.1)
                    killDot(x,y);
            }
                break;
            case Material::Sand:
            {
                if (dbc == Material::Air) {
                    if (random()<0.9)
                        moveDot(x,y,x,y+1,Material::Air,d);
                } else if (dbc == Material::Water) {
                    if (random()<0.6)
                        moveDot(x,y,x,y+1,Material::Water,d);
                } else if (dbc == Material::Acid) {
                    if (random()<0.1)
                        moveDot(x,y,x,y+1,Material::Acid,d);
                } else if (dbc == Material::Oil) {
                    if (random()<0.3)
                        moveDot(x,y,x,y+1,Material::Oil,d);
                } else if (dbc == Material::Fire) {
                    killDot(x,y+1);

                } else if (m_world->dot(x-1,y) == Material::Air && random()<0.01) {
                    moveDot(x,y,x-1,y,Material::Air,d);
                } else if (m_world->dot(x+1,y) == Material::Air && random()<0.01) {
                    moveDot(x,y,x+1,y,Material::Air,d);

                } else if (dbc != Material::Air
                           && m_world->dot(x+1,y+1) == Material::Air
                           && m_world->dot(x+1,y) == Material::Air
                           && random()<0.3) {
                    moveDot(x,y,x+1,y,Material::Air,d);

                } else if (dbc != Material::Air
                           && m_world->dot(x+1,y) == Material::Water
                           && random()<0.3) {
                    moveDot(x,y,x+1,y,Material::Water,d);

                } else if (dbc != Material::Air
                           && m_world->dot(x-1,y) == Material::Water
                           && random()<0.3) {
                    moveDot(x,y,x-1,y,Material::Water,d);

                } else if (dbc != Material::Air
                           && m_world->dot(x+1,y) == Material::Oil
                           && random()<0.3) {
                    moveDot(x,y,x+1,y,Material::Oil,d);

                } else if (dbc != Material::Air
                           && m_world->dot(x-1,y) == Material::Oil
                           && random()<0.3) {
                    moveDot(x,y,x-1,y,Material::Oil,d);

                } else if (dbc != Material::Air
                           && m_world->dot(x-1,y) == Material::Air
                           && m_world->dot(x-1,y+1) == Material::Air
                           && random()<0.3) {
                    moveDot(x,y,x-1,y,Material::Air,d);
                }
            }

                break;
            case Material::Steam:
            {
                if ( dtc != Material::Earth
                     && dtc != Material::Rock
                     && dtc != d && random()<0.5) {
                    moveDot(x,y,x,y-1,dtc,d);
                } else if (random()<0.3
                           && dtc != Material::Air
                           && m_world->dot(x-1,y) == Material::Air
                           && m_world->dot(x-1,y+1)!=d) {
                    moveDot(x,y,x-1,y,Material::Air, Material::Steam);
                } else if (random()<0.3
                           && dtc != Material::Air
                           && m_world->dot(x+1,y) == Material::Air
                           && m_world->dot(x+1,y+1)!=d) {
                    moveDot(x,y,x+1,y,Material::Air, Material::Steam);
                } else if (random()<0.3
                           && dtc != Material::Air
                           && m_world->dot(x+2,y) == Material::Air
                           && m_world->dot(x+2,y+1)!=d) {
                    moveDot(x,y,x+2,y,Material::Air, Material::Steam);
                } else if (random()<0.3
                           && dtc != Material::Air
                           && m_world->dot(x-2,y) == Material::Air
                           && m_world->dot(x-2,y+1)!=d) {
                    moveDot(x,y,x-2,y,Material::Air, Material::Steam);
                }
                if (random()<0.03 || y<1) {
                    killDot(x,y);
                }
            }
                break;
            case Material::Water:
            {
                if (dbc == Material::Air) {
                    if (random()<0.95)
                        moveDot(x,y,x,y+1,Material::Air,d);
                } else if (dbc == Material::Fire) {
                    moveDot(x,y, x, y+1, Material::Steam, d);
                } else if (m_world->dot(x+1,y) == Material::Fire) {
                    addDot(x,y,Material::Steam);
                    killDot(x+1,y);
                } else if (m_world->dot(x-1,y) == Material::Fire) {
                    addDot(x, y, Material::Steam);
                    killDot(x-1, y);
                } else if (dbc==Material::Oil && random()<0.3) {
                    moveDot(x,y,x,y+1,Material::Oil,d);
                } else if (dbc==Material::Acid && random()<0.01) {
                    killDot(x,y+1);
                } else if (m_world->dot(x+1,y)==Material::Oil && random()<0.1) {
                    moveDot(x+1,y,x,y,d,Material::Oil);
                } else if (m_world->dot(x-1,y)==Material::Oil && random()<0.1) {
                    moveDot(x-1,y,x,y,d,Material::Oil);

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
    emit changed();
}

/***********************************************************************************
 ***********************************************************************************/
inline void GameEngine::boom(const int x, const int y, const Material mat)
{
    for (int i = 0; i < C_EXPLOSION_BLAST_WIDTH_IN_DOTS; ++i) {
        for (int j = 0; j < C_EXPLOSION_BLAST_HEIGHT_IN_DOTS; ++j) {
            addDot(x+i, y-j, mat);
        }
    }
}

inline void GameEngine::liquid(const int x, const int y, const Material mat)
{
    const Material r1 = m_world->dot(x+1,y);
    const Material r2 = m_world->dot(x+2,y);
    const Material r3 = m_world->dot(x+3,y);
    const Material l1 = m_world->dot(x-1,y);
    const Material l2 = m_world->dot(x-2,y);
    const Material l3 = m_world->dot(x-3,y);

    const int w = ((r1==mat) ? 1 : 0 )
            + ( (r2==mat) ? 1 : 0 )
            + ( (r3==mat) ? 1 : 0 )
            - ( (l1==mat) ? 1 : 0 )
            - ( (l2==mat) ? 1 : 0 )
            - ( (l3==mat) ? 1 : 0 );

    if (w<=0 && random()<0.5) {
        if      (r1==Material::Air && m_world->dot(x+1,y-1)!=mat) moveDot(x,y,x+1,y,Material::Air,mat);
        else if (r2==Material::Air && m_world->dot(x+2,y-1)!=mat) moveDot(x,y,x+2,y,Material::Air,mat);
        else if (r3==Material::Air && m_world->dot(x+3,y-1)!=mat) moveDot(x,y,x+3,y,Material::Air,mat);
    } else if (w>=0 && random()<0.5) {
        if      (l1==Material::Air && m_world->dot(x-1,y-1)!=mat) moveDot(x,y,x-1,y,Material::Air,mat);
        else if (l2==Material::Air && m_world->dot(x-2,y-1)!=mat) moveDot(x,y,x-2,y,Material::Air,mat);
        else if (l3==Material::Air && m_world->dot(x-3,y-1)!=mat) moveDot(x,y,x-3,y,Material::Air,mat);
    }
}

/***********************************************************************************
 ***********************************************************************************/
inline void GameEngine::addDot(const int x, const int y, const Material mat)
{
    m_world->setDot(x,y,mat);

    ColorVariation c = (random() < materialRandomBreakValue(mat) )
            ? ColorVariation::Color0
            : ColorVariation::Color1;

    m_world->setColorVariation(x,y,c);

}

inline void GameEngine::moveDot(const int x, const int y,
                                const int nx, const int ny,
                                const Material mat, const Material nMat)
{
    m_world->setDot(x,y,mat);
    m_world->setDot(nx,ny,nMat);
}

inline void GameEngine::killDot(const int x, const int y)
{
    m_world->setDot(x,y,Material::Air);
}

/***********************************************************************************
 ***********************************************************************************/
void GameEngine::spawnFountain()
{
    for (int i = 0; i < m_fountains.count(); i++) {
        spawnDot(m_fountains.at(i).x, m_fountains.at(i).y, m_fountains.at(i).type);
    }
    spawnMouse();
    emit changed();
}

inline void GameEngine::spawnDot(const int x, const int y, const Material mat)
{
    if (isSolid(mat)) {
        const int total = 4;
        for (int i = 0; i <= total; ++i) {
            for (int j = 0; j <= total; ++j) {
                if ((i==0 || i==total) && (j==0 || j==total))
                    continue;
                if (m_world->dot(x-(total/2)+i, y-(total/2)+j) != mat) {
                    addDot(x-(total/2)+i, y-(total/2)+j, mat);
                }
            }
        }
    } else {
        addDot(x,y,mat);
    }
}

inline void GameEngine::spawnMouse()
{
    if (m_isMousePressed) {
        spawnDot(m_mousePosX, m_mousePosY, m_currentMaterial);
    }
}



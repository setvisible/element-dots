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

#include "gameworld.h"

#include <QtCore/QDebug>

/*! \class GameWorld
 *  \brief The class GameWorld holds the scene of the game.
 *
 * It hides the scene's buffers.
 * It hides also some internal variables that speed up the memory accesses.
 *
 * The class GameWorld is reentrant but not thread-safe.
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

GameWorld::GameWorld(QObject *parent) : QObject(parent)
  , m_world(Q_NULLPTR)
  , m_worldColor(Q_NULLPTR)
  , m_width(16)
  , m_height(16)
{
    clear();
}

GameWorld::~GameWorld()
{
    if (m_world) delete [] m_world;
    if (m_worldColor) delete [] m_worldColor;
}

void GameWorld::clear()
{
    if (m_world) delete [] m_world;
    if (m_worldColor) delete [] m_worldColor;

    m_world = new char[m_height * m_width];
    m_worldColor = new bool[m_height * m_width];

    memset(m_world, (char)Material::Air, sizeof(char) * m_height * m_width);
    memset(m_worldColor, false, sizeof(bool) * m_height * m_width);
}

/***********************************************************************************
 ***********************************************************************************/
int GameWorld::width() const
{
    return m_width;
}

int GameWorld::height() const
{
    return m_height;
}

void GameWorld::setSize(const int width, const int height)
{
    m_width  = (width>0) ? width  : 16 ;
    m_height = (height>0)? height : 16 ;
    clear();
}

/***********************************************************************************
 ***********************************************************************************/
void GameWorld::setDot(const int x, const int y, const Material material)
{
    if (x >= 0 && y >= 0 && x < m_width && y < m_height) {
        m_world[ y * m_width + x ] = (char)material;
    }
}

Material GameWorld::dot(const int x, const int y) const
{
    if (x >= 0 && y >= 0 && x < m_width && y < m_height) {
        return (Material)m_world[ y * m_width + x ];
    }
    return Material::Air;
}

/***********************************************************************************
 ***********************************************************************************/
void GameWorld::setColorVariation(const int x, const int y, const ColorVariation color)
{
    if (x >= 0 && y >= 0 && x < m_width && y < m_height) {
        m_worldColor[ y * m_width + x ] = (bool)color;
    }
}

ColorVariation GameWorld::colorVariation(const int x, const int y) const
{
    if (x >= 0 && y >= 0 && x < m_width && y < m_height) {
        return (ColorVariation)m_worldColor[ y * m_width + x ];
    }
    return (ColorVariation)0;
}


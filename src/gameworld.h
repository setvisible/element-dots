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

#ifndef GAME_WORLD_H
#define GAME_WORLD_H

#include "gamematerial.h"

#include <QtCore/QObject>

class GameWorld : public QObject
{
    Q_OBJECT
public:
    explicit GameWorld(QObject *parent = 0);
    ~GameWorld();

    void clear();

public Q_SLOTS:
    int width() const;
    int height() const;
    void setSize(const int width, const int height);

    Material dot(const int x, const int y) const;
    void setDot(const int x, const int y, const Material material);

    ColorVariation colorVariation(const int x, const int y) const;
    void setColorVariation(const int x, const int y, const ColorVariation color);

private:
    char* m_world;       /* Material has 10 values -> stored as char */
    bool* m_worldColor;  /* ColorVariation has 2 variants -> stored as boolean */
    int m_width;
    int m_height;

};

#endif // GAME_WORLD_H

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

#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <QtCore/QObject>

#include "gamematerial.h"

class QTimer;
class GameWorld;
class GameEngine : public QObject
{
    Q_OBJECT

    struct Fountain {
        int x;
        int y;
        Brush type;
    };

public:
    explicit GameEngine(QObject *parent = 0);
    ~GameEngine();

Q_SIGNALS:
    void changed();

public Q_SLOTS:
    void clear();

    Brush currentBrush() const;
    void setCurrentBrush(const Brush brush);

    GameWorld* world() const;

    void setMousePressed(const bool pressed);
    void moveMouseTo(const int posX, const int posY);

private Q_SLOTS:
    void updateGame();
    void spawnFountain();

private:
    GameWorld* m_world;
    QTimer* m_updateTimer;
    QTimer* m_fountainTimer;
    bool m_isMousePressed;
    int m_mousePosX;
    int m_mousePosY;
    Brush m_currentBrush;
    QList<Fountain> m_fountains;


    inline void boom(const int x, const int y, const Brush brush) ;
    inline void liquid(const int x, const int y, const Brush brush);

    inline void addDot(const int x, const int y, const Brush brush);
    inline void moveDot(const int x, const int y, const int nx, const int ny,
                        const Brush brush, const Brush nbrush);
    inline void killDot(const int x, const int y);

    inline void spawnDot(const int x, const int y, const Brush brush);
    inline void spawnMouse();

};

#endif // GAME_ENGINE_H

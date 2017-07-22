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

#ifndef GAME_WIDGET_H
#define GAME_WIDGET_H

#include <QtWidgets/QWidget>

#include <gamematerial.h>

class GameWorld;
class GameWidget : public QWidget
{
    Q_OBJECT

    struct Fountain {
        int x;
        int y;
        Brush type;
    };

public:
    explicit GameWidget(QWidget *parent = 0);
    ~GameWidget();

    Brush currentBrush() const;
    void setCurrentBrush(const QString &name);

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

Q_SIGNALS:

public Q_SLOTS:
    void startGame(); // to do
    void stopGame(); // to do
    void clear();


private Q_SLOTS:
    void spawnFountain();
    void updateGame();

private:
    QTimer* m_updateTimer;
    QTimer* m_fountainTimer;
    GameWorld* m_world;

    Brush m_currentBrush;
    QColor m_gridColor;

    bool m_isSpawningDots;
    int m_mousePosX;
    int m_mousePosY;

    QList<Fountain> m_fountains;

    inline void boom(const int x, const int y, const Brush brush) ;
    inline void liquid(const int x, const int y, const Brush brush);

    inline void addDot(const int x, const int y, const Brush brush);
    inline void moveDot(const int x, const int y, const int nx, const int ny,
                        const Brush brush, const Brush nbrush);
    inline void killDot(const int x, const int y);

    inline void spawnDot(const int x, const int y, const Brush brush);

    void paintGrid(QPainter &p);
    void paintUniverse(QPainter &p);

};

#endif // GAME_WIDGET_H

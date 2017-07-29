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

#include "gamematerial.h"

class GameWorld;
class GameEngine;


class GameWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GameWidget(QWidget *parent = 0);
    ~GameWidget();

    Material currentMaterial() const;
    void setCurrentMaterial(const Material mat);

    int worldWidth() const;
    int worldHeight() const;
    void setWorldSize(const int width, const int height);

    int threadsNumber() const;
    void setThreadsNumber(const int threads);

public Q_SLOTS:
    void clear();
    void fillRandomly();

Q_SIGNALS:

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);

private Q_SLOTS:
    void paint();
    void paintFrame();

private:
    GameEngine* m_engine;
    QColor m_gridColor;
    int m_threads;

    inline QPixmap generatePixmapDots();
    inline QPixmap generatePixmapGrid();

};

#endif // GAME_WIDGET_H

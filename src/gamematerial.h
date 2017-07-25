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

#ifndef GAME_MATERIAL_H
#define GAME_MATERIAL_H

#include <QtCore/QMetaType>
#include <QtCore/QString>

#ifdef QT_DEBUG
#  include <QtCore/QDebug>
#endif


enum class ColorVariation {
    Color0 = 0,
    Color1 = 1
};

enum class Material {
    Acid     = 0,
    Air      = 1,
    Earth    = 2,
    Fire     = 3,
    Oil      = 4,
    Plasma   = 5,
    Rock     = 6,
    Sand     = 7,
    Steam    = 8,
    Water    = 9
};

Q_DECLARE_METATYPE(Material)

QString toString(const Material material);
Material toMaterial(const QString &name);

QString materialColor(const Material material, const ColorVariation color);
ColorVariation computeRandomColor(const Material material);

bool isSolid(const Material material);
bool isLiquid(const Material material);

#ifdef QT_DEBUG
QDebug operator<<(QDebug dbg, const Material &material);
#endif


#endif // GAME_MATERIAL_H

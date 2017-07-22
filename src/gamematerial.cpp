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

#include "gamematerial.h"

QString toString(const Brush brush)
{
    QString str;
    switch (brush) {
    case Brush::Acid:   str = QLatin1String("Brush::Acid"); break;
    case Brush::Air:    str = QLatin1String("Brush::Air"); break;
    case Brush::Earth:  str = QLatin1String("Brush::Earth"); break;
    case Brush::Fire:   str = QLatin1String("Brush::Fire"); break;
    case Brush::Oil:    str = QLatin1String("Brush::Oil"); break;
    case Brush::Plasma: str = QLatin1String("Brush::Plasma"); break;
    case Brush::Rock:   str = QLatin1String("Brush::Rock"); break;
    case Brush::Sand:   str = QLatin1String("Brush::Sand"); break;
    case Brush::Steam:  str = QLatin1String("Brush::Steam"); break;
    case Brush::Water:  str = QLatin1String("Brush::Water"); break;
    default:
        Q_UNREACHABLE();
        break;
    }
    return str;
}

Brush toBrush(const QString &name)
{
    if (      name == QLatin1String("acid"   ) ) { return Brush::Acid   ; }
    else if ( name == QLatin1String("air"    ) ) { return Brush::Air    ; }
    else if ( name == QLatin1String("earth"  ) ) { return Brush::Earth  ; }
    else if ( name == QLatin1String("fire"   ) ) { return Brush::Fire   ; }
    else if ( name == QLatin1String("oil"    ) ) { return Brush::Oil    ; }
    else if ( name == QLatin1String("plasma" ) ) { return Brush::Plasma ; }
    else if ( name == QLatin1String("rock"   ) ) { return Brush::Rock   ; }
    else if ( name == QLatin1String("sand"   ) ) { return Brush::Sand   ; }
    else if ( name == QLatin1String("steam"  ) ) { return Brush::Steam  ; }
    else if ( name == QLatin1String("water"  ) ) { return Brush::Water  ; }
    else {
        Q_UNREACHABLE();
    }
    return Brush::Water;
}

QString brushColor(const Brush brush, const ColorVariation color)
{
    switch (brush) {
    case Brush::Acid:   return (color==ColorVariation::Color0) ? QLatin1String("#f1f") : QLatin1String("#e4e"); break;
    case Brush::Air:    return (color==ColorVariation::Color0) ? QLatin1String("#fff") : QLatin1String("#fff"); break;
    case Brush::Earth:  return (color==ColorVariation::Color0) ? QLatin1String("#0b0") : QLatin1String("#2d2"); break;
    case Brush::Fire:   return (color==ColorVariation::Color0) ? QLatin1String("#c41") : QLatin1String("#f60"); break;
    case Brush::Oil:    return (color==ColorVariation::Color0) ? QLatin1String("#212") : QLatin1String("#111"); break;
    case Brush::Plasma: return (color==ColorVariation::Color0) ? QLatin1String("#f3a") : QLatin1String("#ee0"); break;
    case Brush::Rock:   return (color==ColorVariation::Color0) ? QLatin1String("#777") : QLatin1String("#666"); break;
    case Brush::Sand:   return (color==ColorVariation::Color0) ? QLatin1String("#b73") : QLatin1String("#a82"); break;
    case Brush::Steam:  return (color==ColorVariation::Color0) ? QLatin1String("#bbd") : QLatin1String("#ccc"); break;
    case Brush::Water:  return (color==ColorVariation::Color0) ? QLatin1String("#12d") : QLatin1String("#12f"); break;
    default:
        Q_UNREACHABLE();
        break;
    }
    return QLatin1String("#f00");
}

double brushRandomBreakValue(const Brush brush)
{
    switch (brush) {
    case Brush::Acid:   return 0.9; break;
    case Brush::Air:    return 0.0; break;
    case Brush::Earth:  return 0.5; break;
    case Brush::Fire:   return 0.5; break;
    case Brush::Oil:    return 0.7; break;
    case Brush::Plasma: return 0.7; break;
    case Brush::Rock:   return 0.5; break;
    case Brush::Sand:   return 0.6; break;
    case Brush::Steam:  return 0.5; break;
    case Brush::Water:  return 0.5; break;
    default:
        Q_UNREACHABLE();
        break;
    }
    return 0.0;
}




#ifdef QT_DEBUG
QDebug operator<<(QDebug dbg, const Brush &brush)
{
    dbg.nospace() << toString(brush);
    return dbg.maybeSpace();
}
#endif




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
#include "utils.h"

QString toString(const Material material)
{
    QString str;
    switch (material) {
    case Material::Acid:   str = QLatin1String("Material::Acid"); break;
    case Material::Air:    str = QLatin1String("Material::Air"); break;
    case Material::Earth:  str = QLatin1String("Material::Earth"); break;
    case Material::Fire:   str = QLatin1String("Material::Fire"); break;
    case Material::Oil:    str = QLatin1String("Material::Oil"); break;
    case Material::Plasma: str = QLatin1String("Material::Plasma"); break;
    case Material::Rock:   str = QLatin1String("Material::Rock"); break;
    case Material::Sand:   str = QLatin1String("Material::Sand"); break;
    case Material::Steam:  str = QLatin1String("Material::Steam"); break;
    case Material::Water:  str = QLatin1String("Material::Water"); break;
    default:
        Q_UNREACHABLE();
        break;
    }
    return str;
}

Material toMaterial(const QString &name)
{
    if (      name == QLatin1String("acid"   ) ) { return Material::Acid   ; }
    else if ( name == QLatin1String("air"    ) ) { return Material::Air    ; }
    else if ( name == QLatin1String("earth"  ) ) { return Material::Earth  ; }
    else if ( name == QLatin1String("fire"   ) ) { return Material::Fire   ; }
    else if ( name == QLatin1String("oil"    ) ) { return Material::Oil    ; }
    else if ( name == QLatin1String("plasma" ) ) { return Material::Plasma ; }
    else if ( name == QLatin1String("rock"   ) ) { return Material::Rock   ; }
    else if ( name == QLatin1String("sand"   ) ) { return Material::Sand   ; }
    else if ( name == QLatin1String("steam"  ) ) { return Material::Steam  ; }
    else if ( name == QLatin1String("water"  ) ) { return Material::Water  ; }
    else {
        Q_UNREACHABLE();
    }
    return Material::Water;
}

QString materialColor(const Material material, const ColorVariation color)
{
    switch (material) {
    case Material::Acid:   return (color==ColorVariation::Color0) ? QLatin1String("#f1f") : QLatin1String("#e4e"); break;
    case Material::Air:    return (color==ColorVariation::Color0) ? QLatin1String("#fff") : QLatin1String("#fff"); break;
    case Material::Earth:  return (color==ColorVariation::Color0) ? QLatin1String("#0b0") : QLatin1String("#2d2"); break;
    case Material::Fire:   return (color==ColorVariation::Color0) ? QLatin1String("#c41") : QLatin1String("#f60"); break;
    case Material::Oil:    return (color==ColorVariation::Color0) ? QLatin1String("#212") : QLatin1String("#111"); break;
    case Material::Plasma: return (color==ColorVariation::Color0) ? QLatin1String("#f3a") : QLatin1String("#ee0"); break;
    case Material::Rock:   return (color==ColorVariation::Color0) ? QLatin1String("#777") : QLatin1String("#666"); break;
    case Material::Sand:   return (color==ColorVariation::Color0) ? QLatin1String("#b73") : QLatin1String("#a82"); break;
    case Material::Steam:  return (color==ColorVariation::Color0) ? QLatin1String("#bbd") : QLatin1String("#ccc"); break;
    case Material::Water:  return (color==ColorVariation::Color0) ? QLatin1String("#12d") : QLatin1String("#12f"); break;
    default:
        Q_UNREACHABLE();
        break;
    }
    return QLatin1String("#f00");
}

static inline double materialRandomBreakValue(const Material material)
{
    switch (material) {
    case Material::Acid:   return 0.9; break;
    case Material::Air:    return 0.0; break;
    case Material::Earth:  return 0.5; break;
    case Material::Fire:   return 0.5; break;
    case Material::Oil:    return 0.7; break;
    case Material::Plasma: return 0.7; break;
    case Material::Rock:   return 0.5; break;
    case Material::Sand:   return 0.6; break;
    case Material::Steam:  return 0.5; break;
    case Material::Water:  return 0.5; break;
    default:
        Q_UNREACHABLE();
        break;
    }
    return 0.0;
}

ColorVariation computeRandomColor(const Material material)
{
    return (random() < materialRandomBreakValue(material) )
            ? ColorVariation::Color0
            : ColorVariation::Color1;
}

bool isSolid(const Material material)
{
    return (material==Material::Earth || material==Material::Plasma || material==Material::Rock);
}

bool isLiquid(const Material material)
{
    return (material==Material::Acid || material==Material::Oil || material==Material::Water);
}


#ifdef QT_DEBUG
QDebug operator<<(QDebug dbg, const Material &material)
{
    dbg.nospace() << toString(material);
    return dbg.maybeSpace();
}
#endif


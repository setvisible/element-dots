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

#ifndef ABOUT_H
#define ABOUT_H

#include "globals.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QString>

QString buildAbout(QString paragraphDelimiter)
{
    return QString(QCoreApplication::tr(
                       "%1 - version %2 - build %3"
                       ).arg(STR_APPLICATION_NAME, STR_APPLICATION_VERSION, STR_APPLICATION_BUILD) +
                   paragraphDelimiter +
                   QCoreApplication::tr(
                       "Copyright (C) %0 %1"
                       ).arg(STR_APPLICATION_DATE).arg(STR_APPLICATION_AUTHOR) +
                   paragraphDelimiter +
                   QCoreApplication::tr("MIT License") +
                   paragraphDelimiter +
                   QCoreApplication::tr(
                       "Permission is hereby granted, free of charge, to any person obtaining a copy "
                       "of this software and associated documentation files (the \"Software\"), to deal "
                       "in the Software without restriction, including without limitation the rights "
                       "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell "
                       "copies of the Software, and to permit persons to whom the Software is "
                       "furnished to do so, subject to the following conditions: ") +
                   paragraphDelimiter +
                   QCoreApplication::tr(
                       "The above copyright notice and this permission notice shall be included in all "
                       "copies or substantial portions of the Software. ") +
                   paragraphDelimiter +
                   QCoreApplication::tr(
                       "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR "
                       "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, "
                       "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE "
                       "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER "
                       "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, "
                       "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE "
                       "SOFTWARE. "));
}

QString about()
{
    return buildAbout("\n\n");
}

QString aboutHtml()
{
    return QString("<h3>" + QCoreApplication::tr("About %0").arg(STR_APPLICATION_NAME) + "</h3>" +
                   "<p><img src=\"://icons/logo/Movies_128x128.png\" /></p>" +
                   "<p></p>" +
                   "<p>" + buildAbout("</p><p>") + "</p>");
}

#endif // ABOUT_H
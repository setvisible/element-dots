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

#ifndef UTILS_H
#define UTILS_H

#include <QtCore/QTime>
#include <QtCore/qmath.h>

static bool seeded = false;

/*!
 * \brief Return a random value between 0 and 1.
 */
static double random()
{
    if (!seeded) {
        /* initialize the pseudo-random number generator with a seed value. */
        qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
        seeded = true;
    }
    Q_ASSERT(RAND_MAX > 0);
    return (double)(qrand())/RAND_MAX;
}


/*
 * Macros for performance measurement
 */
#ifndef PERFS_MEASUREMENT
# define PERFS_MEASUREMENT

# define CREATE_PERFS_MEASUREMENT(AFUNC) \
    static QTime _s_time_##AFUNC;

# define PERFS_MEASURE_START(AFUNC){ \
    _s_time_##AFUNC.restart();  \
    }

# define PERFS_MEASURE_STOP(AFUNC){ \
    qDebug() << Q_FUNC_INFO << _s_time_##AFUNC.elapsed() << "ms"; \
    }

#endif



#endif // UTILS_H

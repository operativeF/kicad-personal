/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 1992-2019 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#ifndef WX_FILENAME_H_
#define WX_FILENAME_H_

enum EDA_UNITS_T {
    INCHES = 0,
    MILLIMETRES = 1,
    UNSCALED_UNITS = 2,
    DEGREES = 3,
    PERCENT = 4,
};

/**
 * Enum FILL_T
 * is the set of fill types used in plotting or drawing enclosed areas.
 */
enum FILL_T {
    NO_FILL,                     // Poly, Square, Circle, Arc = option No Fill
    FILLED_SHAPE,                /* Poly, Square, Circle, Arc = option Fill
                                  * with current color ("Solid shape") */
    FILLED_WITH_BG_BODYCOLOR     /* Poly, Square, Circle, Arc = option Fill
                                  * with background body color, translucent
                                  * (texts inside this shape can be seen)
                                  * not filled in B&W mode when plotting or
                                  * printing */
};


enum SEARCH_RESULT {
    SEARCH_QUIT,
    SEARCH_CONTINUE
};

#endif
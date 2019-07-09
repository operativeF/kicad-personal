/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2019 KiCad Developers, see AUTHORS.txt for contributors.
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

#ifndef TIMESTAMP_H_
#define TIMESTAMP_H_

#include <wx/wx.h>
#include <wx/datetime.h>
#include <wx/longlong.h>
#include <cstdint>

/**
 * timestamp_t is our type to represent unique IDs for all kinds of elements;
 * historically simply the timestamp when they were created.
 *
 * Long term, this type might be renamed to something like unique_id_t
 * (and then rename all the methods from {Get,Set}TimeStamp()
 * to {Get,Set}Id()) ?
 */
using timestamp_t = uint32_t;

/**
 * @return an unique time stamp that changes after each call
 */
timestamp_t GetNewTimeStamp();

/**
 * A copy of ConvertFileTimeToWx() because wxWidgets left it as a static function
 * private to src/common/filename.cpp.
 */
#if wxUSE_DATETIME && defined(__WIN32__) && !defined(__WXMICROWIN__)

// Convert between wxDateTime and FILETIME which is a 64-bit value representing
// the number of 100-nanosecond intervals since January 1, 1601 UTC.
//
// This is the offset between FILETIME epoch and the Unix/wxDateTime Epoch.
static wxInt64 EPOCH_OFFSET_IN_MSEC = wxLL(11644473600000);

static void ConvertFileTimeToWx( wxDateTime *dt, const FILETIME &ft )
{
    wxLongLong t( ft.dwHighDateTime, ft.dwLowDateTime );
    t /= 10000; // Convert hundreds of nanoseconds to milliseconds.
    t -= EPOCH_OFFSET_IN_MSEC;

    *dt = wxDateTime( t );
}

#endif // wxUSE_DATETIME && __WIN32__

#endif // TIMESTAMP_H_
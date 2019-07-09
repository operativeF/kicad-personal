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

#include <time.h>

#include <wx/datetime.h>
#include <wx/filename.h>
#include <wx/string.h>

#include <timestamp.h>


timestamp_t GetNewTimeStamp()
{
    static timestamp_t oldTimeStamp;
    timestamp_t newTimeStamp;

    newTimeStamp = time( 0 );

    if( newTimeStamp <= oldTimeStamp )
        newTimeStamp = oldTimeStamp + 1;

    oldTimeStamp = newTimeStamp;

    return newTimeStamp;
}

/**
 * TimestampDir
 *
 * This routine offers SIGNIFICANT performance benefits over using wxWidgets to gather
 * timestamps from matching files in a directory.
 * @param aDirPath the directory to search
 * @param aFilespec a (wildcarded) file spec to match against
 * @return a hash of the last-mod-dates of all matching files in the directory
 */
long long TimestampDir( const wxString& aDirPath, const wxString& aFilespec )
{
    long long timestamp = 0;

#if defined( __WIN32__ )
    // Win32 version.
    // Save time by not searching for each path twice: once in wxDir.GetNext() and once in
    // wxFileName.GetModificationTime().  Also cuts out wxWidgets' string-matching and case
    // conversion by staying on the MSW side of things.
    std::wstring filespec( aDirPath.t_str() );
    filespec += '\\';
    filespec += aFilespec.t_str();

    WIN32_FIND_DATA findData;
    wxDateTime      lastModDate;

    HANDLE fileHandle = ::FindFirstFile( filespec.data(), &findData );

    if( fileHandle != INVALID_HANDLE_VALUE )
    {
        do
        {
            ConvertFileTimeToWx( &lastModDate, findData.ftLastWriteTime );
            timestamp += lastModDate.GetValue().GetValue();
        }
        while ( FindNextFile( fileHandle, &findData ) != 0 );
    }

    FindClose( fileHandle );
#else
    // POSIX version.
    // Save time by not converting between encodings -- do everything on the file-system side.
    std::string filespec( aFilespec.fn_str() );
    std::string dir_path( aDirPath.fn_str() );

    DIR* dir = opendir( dir_path.c_str() );

    if( dir )
    {
        for( dirent* dir_entry = readdir( dir ); dir_entry; dir_entry = readdir( dir ) )
        {
            if( !matchWild( filespec.c_str(), dir_entry->d_name, true ) )
                continue;

            std::string entry_path = dir_path + '/' + dir_entry->d_name;
            struct stat entry_stat;

            wxCRT_Lstat( entry_path.c_str(), &entry_stat );

            // Timestamp the source file, not the symlink
            if( S_ISLNK( entry_stat.st_mode ) )    // wxFILE_EXISTS_SYMLINK
            {
                char buffer[ PATH_MAX + 1 ];
                ssize_t pathLen = readlink( entry_path.c_str(), buffer, PATH_MAX );

                if( pathLen > 0 )
                {
                    buffer[ pathLen ] = '\0';
                    entry_path = dir_path + buffer;

                    wxCRT_Lstat( entry_path.c_str(), &entry_stat );
                }
            }

            if( S_ISREG( entry_stat.st_mode ) )    // wxFileExists()
                timestamp += entry_stat.st_mtime * 1000;
        }

        closedir( dir );
    }
#endif

    return timestamp;
}
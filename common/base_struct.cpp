/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2015 Jean-Pierre Charras, jaen-pierre.charras@gipsa-lab.inpg.com
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

/**
 * @brief Implementation of EDA_ITEM base class for KiCad.
 */

#include <deque>

#include <base_struct.h>
#include <trace_helpers.h>

#include "../eeschema/dialogs/dialog_schematic_find.h"


static const unsigned char dummy_png[] = {
 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52,
 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x08, 0x06, 0x00, 0x00, 0x00, 0x1f, 0xf3, 0xff,
 0x61, 0x00, 0x00, 0x00, 0x5f, 0x49, 0x44, 0x41, 0x54, 0x38, 0xcb, 0x63, 0xf8, 0xff, 0xff, 0x3f,
 0x03, 0x25, 0x98, 0x61, 0x68, 0x1a, 0x00, 0x04, 0x46, 0x40, 0xfc, 0x02, 0x88, 0x45, 0x41, 0x1c,
 0x76, 0x20, 0xfe, 0x01, 0xc4, 0xbe, 0x24, 0x18, 0x60, 0x01, 0xc4, 0x20, 0x86, 0x04, 0x88, 0xc3,
 0x01, 0xe5, 0x04, 0x0c, 0xb8, 0x01, 0x37, 0x81, 0xf8, 0x04, 0x91, 0xf8, 0x0a, 0x54, 0x8f, 0x06,
 0xb2, 0x01, 0x9b, 0x81, 0x78, 0x02, 0x91, 0x78, 0x05, 0x54, 0x8f, 0xca, 0xe0, 0x08, 0x03, 0x36,
 0xa8, 0xbf, 0xec, 0xc8, 0x32, 0x80, 0xcc, 0x84, 0x04, 0x0a, 0xbc, 0x1d, 0x40, 0x2c, 0xc8, 0x30,
 0xf4, 0x33, 0x13, 0x00, 0x6b, 0x1a, 0x46, 0x7b, 0x68, 0xe7, 0x0f, 0x0b, 0x00, 0x00, 0x00, 0x00,
 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82,
};

static const BITMAP_OPAQUE dummy_xpm[1] = {{ dummy_png, sizeof( dummy_png ), "dummy_xpm" }};


enum textbox {
    ID_TEXTBOX_LIST = 8010
};


EDA_ITEM::EDA_ITEM( EDA_ITEM* parent, KICAD_T idType )
{
    initVars();
    m_StructType = idType;
    m_Parent     = parent;
}


EDA_ITEM::EDA_ITEM( KICAD_T idType )
{
    initVars();
    m_StructType = idType;
}


EDA_ITEM::EDA_ITEM( const EDA_ITEM& base )
{
    initVars();
    *this = base;
}


void EDA_ITEM::initVars()
{
    m_StructType = TYPE_NOT_INIT;
    Pnext       = NULL;     // Linked list: Link (next struct)
    Pback       = NULL;     // Linked list: Link (previous struct)
    m_Parent    = NULL;     // Linked list: Link (parent struct)
    m_List      = NULL;     // I am not on any list yet
    m_Flags     = 0;        // flags for editions and other
    SetTimeStamp( 0 );      // Time stamp used for logical links
    m_Status    = 0;
    m_forceVisible = false; // true to override the visibility setting of the item.
}


void EDA_ITEM::SetModified()
{
    SetFlags( IS_CHANGED );

    // If this a child object, then the parent modification state also needs to be set.
    if( m_Parent )
        m_Parent->SetModified();
}


EDA_RECT EDA_ITEM::GetBoundingBox() const
{
#if defined(DEBUG)
    printf( "Missing GetBoundingBox()\n" );
    Show( 0, std::cout ); // tell me which classes still need GetBoundingBox support
#endif

    // return a zero-sized box per default. derived classes should override
    // this
    return EDA_RECT( wxPoint( 0, 0 ), wxSize( 0, 0 ) );
}


EDA_ITEM* EDA_ITEM::Clone() const
{
    wxCHECK_MSG( false, NULL, wxT( "Clone not implemented in derived class " ) + GetClass() +
                 wxT( ".  Bad programmer!" ) );
}


// see base_struct.h
// many classes inherit this method, be careful:
//TODO(snh): Fix this to use std::set instead of C-style vector
SEARCH_RESULT EDA_ITEM::Visit( INSPECTOR inspector, void* testData, const KICAD_T scanTypes[] )
{
#if 0 && defined(DEBUG)
    std::cout << GetClass().mb_str() << ' ';
#endif

    if( IsType( scanTypes ) )
    {
        if( SEARCH_QUIT == inspector( this, testData ) )
            return SEARCH_QUIT;
    }

    return SEARCH_CONTINUE;
}


wxString EDA_ITEM::GetSelectMenuText( EDA_UNITS_T aUnits ) const
{
    wxFAIL_MSG( wxT( "GetSelectMenuText() was not overridden for schematic item type " ) +
                GetClass() );

    return wxString( wxT( "Undefined menu text for " ) + GetClass() );
}


bool EDA_ITEM::Matches( const wxString& aText, wxFindReplaceData& aSearchData )
{
    wxString text = aText;
    wxString searchText = aSearchData.GetFindString();

    // Don't match if searching for replaceable item and the item doesn't support text replace.
    if( (aSearchData.GetFlags() & FR_SEARCH_REPLACE) && !IsReplaceable() )
        return false;

    if( aSearchData.GetFlags() & wxFR_WHOLEWORD )
        return aText.IsSameAs( searchText, aSearchData.GetFlags() & wxFR_MATCHCASE );

    if( aSearchData.GetFlags() & FR_MATCH_WILDCARD )
    {
        if( aSearchData.GetFlags() & wxFR_MATCHCASE )
            return text.Matches( searchText );

        return text.MakeUpper().Matches( searchText.MakeUpper() );
    }

    if( aSearchData.GetFlags() & wxFR_MATCHCASE )
        return aText.Find( searchText ) != wxNOT_FOUND;

    return text.MakeUpper().Find( searchText.MakeUpper() ) != wxNOT_FOUND;
}


bool EDA_ITEM::Replace( wxFindReplaceData& aSearchData, wxString& aText )
{
    wxCHECK_MSG( IsReplaceable(), false,
                 wxT( "Attempt to replace text in <" ) + GetClass() + wxT( "> item." ) );

    wxString searchString = (aSearchData.GetFlags() & wxFR_MATCHCASE) ? aText : aText.Upper();

    int result = searchString.Find( (aSearchData.GetFlags() & wxFR_MATCHCASE) ?
                                    aSearchData.GetFindString() :
                                    aSearchData.GetFindString().Upper() );

    if( result == wxNOT_FOUND )
        return false;

    wxString prefix = aText.Left( result );
    wxString suffix;

    if( aSearchData.GetFindString().length() + result < aText.length() )
        suffix = aText.Right( aText.length() - ( aSearchData.GetFindString().length() + result ) );

    wxLogTrace( traceFindReplace, wxT( "Replacing '%s', prefix '%s', replace '%s', suffix '%s'." ),
                GetChars( aText ), GetChars( prefix ), GetChars( aSearchData.GetReplaceString() ),
                GetChars( suffix ) );

    aText = prefix + aSearchData.GetReplaceString() + suffix;

    return true;
}


bool EDA_ITEM::operator<( const EDA_ITEM& aItem ) const
{
    wxFAIL_MSG( wxString::Format( wxT( "Less than operator not defined for item type %s." ),
                                  GetChars( GetClass() ) ) );

    return false;
}

EDA_ITEM& EDA_ITEM::operator=( const EDA_ITEM& aItem )
{
    // do not call initVars()

    m_StructType = aItem.m_StructType;
    m_Flags      = aItem.m_Flags;
    m_Status     = aItem.m_Status;
    m_Parent     = aItem.m_Parent;
    m_forceVisible = aItem.m_forceVisible;

    // A copy of an item cannot have the same time stamp as the original item.
    SetTimeStamp( GetNewTimeStamp() );

    // do not copy list related fields (Pnext, Pback, m_List)

    return *this;
}

BOX2I EDA_ITEM::ViewBBox() const
{
    // Basic fallback
    return BOX2I( VECTOR2I( GetBoundingBox().GetOrigin() ),
                  VECTOR2I( GetBoundingBox().GetSize() ) );
}


void EDA_ITEM::ViewGetLayers( int aLayers[], int& aCount ) const
{
    // Basic fallback
    aCount      = 1;
    aLayers[0]  = 0;
}

BITMAP_DEF EDA_ITEM::GetMenuImage() const
{
    return dummy_xpm;
}

#if defined(DEBUG)

void EDA_ITEM::ShowDummy( std::ostream& os ) const
{
    // XML output:
    wxString s = GetClass();

    os << '<' << s.Lower().mb_str() << ">"
       << " Need ::Show() override for this class "
       << "</" << s.Lower().mb_str() << ">\n";
}


std::ostream& EDA_ITEM::NestedSpace( int nestLevel, std::ostream& os )
{
    for( int i = 0; i<nestLevel; ++i )
        os << "  ";

    // number of spaces here controls indent per nest level

    return os;
}

#endif
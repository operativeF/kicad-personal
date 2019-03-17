/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2004 Jean-Pierre Charras, jaen-pierre.charras@gipsa-lab.inpg.com
 * Copyright (C) 2004-2019 KiCad Developers, see change_log.txt for contributors.
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

#ifndef LIB_CIRCLE_H
#define LIB_CIRCLE_H

#include <lib_draw_item.h>


class LIB_CIRCLE : public LIB_ITEM
{
    wxPoint m_Pos;            // Position or centre (Arc and Circle) or start point (segments).
    wxPoint m_EndPos;         // A point on the circumference of the circle.
    int     m_Width;          // Line width.

    void print( wxDC* aDC, const wxPoint& aOffset, void* aData,
                const TRANSFORM& aTransform ) override;

public:
    LIB_CIRCLE( LIB_PART * aParent );

    wxString GetClass() const override
    {
        return wxT( "LIB_CIRCLE" );
    }

    wxString GetTypeName() override
    {
        return _( "Circle" );
    }

    bool HitTest( const wxPoint& aPosition, int aAccuracy = 0 ) const override;
    bool HitTest( const EDA_RECT& aRect, bool aContained, int aAccuracy = 0 ) const override;

    int GetPenSize( ) const override;

    const EDA_RECT GetBoundingBox() const override;

    void GetMsgPanelInfo( EDA_UNITS_T aUnits, std::vector< MSG_PANEL_ITEM >& aList ) override;

    void BeginEdit( const wxPoint aStartPoint ) override;
    void CalcEdit( const wxPoint& aPosition ) override;

    void Offset( const wxPoint& aOffset ) override;

    bool Inside( EDA_RECT& aRect ) const override;

    void MoveTo( const wxPoint& aPosition ) override;

    wxPoint GetPosition() const override { return m_Pos; }

    void SetEnd( const wxPoint& aPosition ) { m_EndPos = aPosition; }
    wxPoint GetEnd() const { return m_EndPos; }

    void MirrorHorizontal( const wxPoint& aCenter ) override;
    void MirrorVertical( const wxPoint& aCenter ) override;
    void Rotate( const wxPoint& aCenter, bool aRotateCCW = true ) override;

    void Plot( PLOTTER* aPlotter, const wxPoint& aOffset, bool aFill,
               const TRANSFORM& aTransform ) override;

    int GetWidth() const override { return m_Width; }
    void SetWidth( int aWidth ) override { m_Width = aWidth; }

    void SetRadius( int aRadius ) { m_EndPos = wxPoint( m_Pos.x + aRadius, m_Pos.y ); }
    int GetRadius() const { return KiROUND( GetLineLength( m_EndPos, m_Pos ) ); }

    wxString GetSelectMenuText( EDA_UNITS_T aUnits ) const override;

    BITMAP_DEF GetMenuImage() const override;

    EDA_ITEM* Clone() const override;

private:

    /**
     * @copydoc LIB_ITEM::compare()
     *
     * The circle specific sort order is as follows:
     *      - Circle horizontal (X) position.
     *      - Circle vertical (Y) position.
     *      - Circle radius.
     */
    int compare( const LIB_ITEM& aOther ) const override;
};


#endif    // LIB_CIRCLE_H

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

#ifndef LIB_ARC_H
#define LIB_ARC_H

#include <lib_draw_item.h>


class TRANSFORM;


class LIB_ARC : public LIB_ITEM
{
    enum SELECT_T               // When creating an arc: status of arc
    {
        ARC_STATUS_START,
        ARC_STATUS_END,
        ARC_STATUS_OUTLINE,
    };

    int      m_Radius;
    int      m_t1;              // First radius angle of the arc in 0.1 degrees.
    int      m_t2;              /* Second radius angle of the arc in 0.1 degrees. */
    wxPoint  m_ArcStart;
    wxPoint  m_ArcEnd;          /* Arc end position. */
    wxPoint  m_Pos;             /* Radius center point. */
    int      m_Width;           /* Line width */
    int      m_editState;

    void print( wxDC* aDC, const wxPoint& aOffset, void* aData,
                const TRANSFORM& aTransform ) override;

public:
    LIB_ARC( LIB_PART * aParent );

    wxString GetClass() const override
    {
        return wxT( "LIB_ARC" );
    }

    wxString GetTypeName() override
    {
        return _( "Arc" );
    }

    bool HitTest( const wxPoint& aPosition, int aAccuracy = 0 ) const override;
    bool HitTest( const EDA_RECT& aRect, bool aContained, int aAccuracy = 0 ) const override;

    EDA_RECT GetBoundingBox() const override;

    void GetMsgPanelInfo( EDA_UNITS_T aUnits, std::vector< MSG_PANEL_ITEM >& aList ) override;

    int GetPenSize() const override;

    void BeginEdit( const wxPoint aStartPoint ) override;
    void CalcEdit( const wxPoint& aPosition ) override;
    void SetEditState( int aState ) { m_editState = aState; }

    void Offset( const wxPoint& aOffset ) override;

    bool Inside( EDA_RECT& aRect ) const override;

    void MoveTo( const wxPoint& aPosition ) override;

    wxPoint GetPosition() const override { return m_Pos; }

    void MirrorHorizontal( const wxPoint& aCenter ) override;
    void MirrorVertical( const wxPoint& aCenter ) override;
    void Rotate( const wxPoint& aCenter, bool aRotateCCW = true ) override;

    void Plot( PLOTTER* aPlotter, const wxPoint& aOffset, bool aFill,
               const TRANSFORM& aTransform ) override;

    int GetWidth() const override { return m_Width; }
    void SetWidth( int aWidth ) override { m_Width = aWidth; }

    void SetRadius( int aRadius ) { m_Radius = aRadius; }
    int GetRadius() const { return m_Radius; }

    void SetFirstRadiusAngle( int aAngle ) { m_t1 = aAngle; }
    int GetFirstRadiusAngle() const { return m_t1; }

    void SetSecondRadiusAngle( int aAngle ) { m_t2 = aAngle; }
    int GetSecondRadiusAngle() const { return m_t2; }

    wxPoint GetStart() const { return m_ArcStart; }
    void SetStart( const wxPoint& aPoint ) { m_ArcStart = aPoint; }

    wxPoint GetEnd() const { return m_ArcEnd; }
    void SetEnd( const wxPoint& aPoint ) { m_ArcEnd = aPoint; }

    /**
     * Calculate the radius and angle of an arc using the start, end, and center points.
     */
    void CalcRadiusAngles();


    wxString GetSelectMenuText( EDA_UNITS_T aUnits ) const override;

    BITMAP_DEF GetMenuImage() const override;

    EDA_ITEM* Clone() const override;

private:

    /**
     * @copydoc LIB_ITEM::compare()
     *
     * The arc specific sort order is as follows:
     *      - Arc horizontal (X) position.
     *      - Arc vertical (Y) position.
     *      - Arc start angle.
     *      - Arc end angle.
     */
    int compare( const LIB_ITEM& aOther ) const override;
};


#endif    // LIB_ARC_H

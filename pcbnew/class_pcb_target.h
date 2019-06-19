/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2004 Jean-Pierre Charras, jaen-pierre.charras@gipsa-lab.inpg.com
 * Copyright (C) 1992-2011 KiCad Developers, see AUTHORS.txt for contributors.
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
 * @file class_pcb_target.h
 * @brief PCB_TARGET class definition.  (targets for photo plots).
 */

#ifndef PCB_TARGET_H_
#define PCB_TARGET_H_

#include <class_board_item.h>


class EDA_RECT;
class LINE_READER;


class PCB_TARGET : public BOARD_ITEM
{
    int     m_Shape;            // bit 0 : 0 = draw +, 1 = draw X
    int     m_Size;
    int     m_Width;
    wxPoint m_Pos;

public:
    PCB_TARGET( BOARD_ITEM* aParent );

    PCB_TARGET( BOARD_ITEM* aParent, int aShape, PCB_LAYER_ID aLayer,
                const wxPoint& aPos, int aSize, int aWidth );

    static inline bool ClassOf( const EDA_ITEM* aItem )
    {
        return aItem && PCB_TARGET_T == aItem->Type();
    }

    void SetPosition( const wxPoint& aPos ) override { m_Pos = aPos; }
    wxPoint GetPosition() const override { return m_Pos; }

    void SetShape( int aShape )     { m_Shape = aShape; }
    int GetShape() const            { return m_Shape; }

    void SetSize( int aSize )       { m_Size = aSize; }
    int GetSize() const             { return m_Size; }

    void SetWidth( int aWidth )     { m_Width = aWidth; }
    int GetWidth() const            { return m_Width; }

    void Move( const wxPoint& aMoveVector ) override
    {
        m_Pos += aMoveVector;
    }

    void Rotate( const wxPoint& aRotCentre, double aAngle ) override;

    void Flip( const wxPoint& aCentre ) override;

    void Print( PCB_BASE_FRAME* aFrame, wxDC* DC, const wxPoint& offset = ZeroOffset ) override;

    wxString GetClass() const override
    {
        return wxT( "PCB_TARGET" );
    }

    bool HitTest( const wxPoint& aPosition, int aAccuracy = 0 ) const override;
    bool HitTest( const EDA_RECT& aRect, bool aContained, int aAccuracy = 0 ) const override;

    // Virtual function
    EDA_RECT GetBoundingBox() const override;

    wxString GetSelectMenuText( EDA_UNITS_T aUnits ) const override;

    BITMAP_DEF GetMenuImage() const override;

    EDA_ITEM* Clone() const override;

    virtual void SwapData( BOARD_ITEM* aImage ) override;

#if defined(DEBUG)
    void Show( int nestLevel, std::ostream& os ) const override { ShowDummy( os ); }
#endif
};


#endif

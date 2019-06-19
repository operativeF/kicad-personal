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

/**
 * @file sch_bus_entry.h
 *
 */

#ifndef _SCH_BUS_ENTRY_H_
#define _SCH_BUS_ENTRY_H_

#include <sch_item.h>

#define TARGET_BUSENTRY_RADIUS 12   // Circle diameter drawn at the ends


/**
 * Base class for a bus or wire entry.
 */
class SCH_BUS_ENTRY_BASE : public SCH_ITEM
{
protected:
    wxPoint m_pos;
    wxSize  m_size;
    bool m_isDanglingStart, m_isDanglingEnd;

public:
    SCH_BUS_ENTRY_BASE( KICAD_T aType, const wxPoint& pos = wxPoint( 0, 0 ), char shape = '\\' );

    bool IsDanglingStart() const { return m_isDanglingStart; }
    bool IsDanglingEnd() const { return m_isDanglingEnd; }

    /**
     * Virtual function IsMovableFromAnchorPoint
     * Return true for items which are moved with the anchor point at mouse cursor
     *  and false for items moved with no reference to anchor
     * @return false for a bus entry
     */
    bool IsMovableFromAnchorPoint() override { return false; }

    wxPoint m_End() const;

    /**
     * function GetBusEntryShape
     * @return the shape of the bus entry, as an ascii code '/' or '\'
     */
    char GetBusEntryShape() const;

    /**
     * function SetBusEntryShape
     * @param aShape = the shape of the bus entry, as an ascii code '/' or '\'
     */
    void SetBusEntryShape( char aShape );

    wxSize GetSize() const { return m_size; }

    void SetSize( const wxSize& aSize ) { m_size = aSize; }

    void SwapData( SCH_ITEM* aItem ) override;

    void ViewGetLayers( int aLayers[], int& aCount ) const override;

    void Print( wxDC* aDC, const wxPoint& aOffset ) override;

    EDA_RECT GetBoundingBox() const override;

    void Move( const wxPoint& aMoveVector ) override
    {
        m_pos += aMoveVector;
    }

    void MirrorY( int aYaxis_position ) override;
    void MirrorX( int aXaxis_position ) override;
    void Rotate( wxPoint aPosition ) override;

    bool IsDangling() const override;

    bool IsConnectable() const override { return true; }

    void GetConnectionPoints( std::vector< wxPoint >& aPoints ) const override;

    wxPoint GetPosition() const override { return m_pos; }
    void SetPosition( const wxPoint& aPosition ) override { m_pos = aPosition; }

    bool HitTest( const wxPoint& aPosition, int aAccuracy = 0 ) const override;
    bool HitTest( const EDA_RECT& aRect, bool aContained, int aAccuracy = 0 ) const override;

    void Plot( PLOTTER* aPlotter ) override;

    void GetMsgPanelInfo( EDA_UNITS_T aUnits, std::vector< MSG_PANEL_ITEM >& aList ) override;

#if defined(DEBUG)
    void Show( int nestLevel, std::ostream& os ) const override { ShowDummy( os ); }
#endif

private:
    bool doIsConnected( const wxPoint& aPosition ) const override;
};

/**
 * Class for a wire to bus entry.
 */
class SCH_BUS_WIRE_ENTRY : public SCH_BUS_ENTRY_BASE
{
public:
    SCH_BUS_WIRE_ENTRY( const wxPoint& pos = wxPoint( 0, 0 ), char shape = '\\' );

    ~SCH_BUS_WIRE_ENTRY() { }

    wxString GetClass() const override
    {
        return wxT( "SCH_BUS_WIRE_ENTRY" );
    }

    int GetPenSize() const override;

    void GetEndPoints( std::vector< DANGLING_END_ITEM >& aItemList ) override;

    bool CanConnect( const SCH_ITEM* aItem ) const override
    {
        return aItem->Type() == SCH_LINE_T &&
                ( aItem->GetLayer() == LAYER_WIRE || aItem->GetLayer() == LAYER_BUS );
    }

    wxString GetSelectMenuText( EDA_UNITS_T aUnits ) const override;

    EDA_ITEM* Clone() const override;

    virtual bool ConnectionPropagatesTo( const EDA_ITEM* aItem ) const override;

    BITMAP_DEF GetMenuImage() const override;

    bool UpdateDanglingState( std::vector<DANGLING_END_ITEM>& aItemList ) override;

    /**
     * Pointer to the bus item (usually a bus wire) connected to this bus-wire
     * entry, if it is connected to one.
     */
    SCH_ITEM* m_connected_bus_item;
};

/**
 * Class for a bus to bus entry.
 */
class SCH_BUS_BUS_ENTRY : public SCH_BUS_ENTRY_BASE
{
public:
    SCH_BUS_BUS_ENTRY( const wxPoint& pos = wxPoint( 0, 0 ), char shape = '\\' );

    ~SCH_BUS_BUS_ENTRY() { }

    wxString GetClass() const override
    {
        return wxT( "SCH_BUS_BUS_ENTRY" );
    }

    int GetPenSize() const override;

    void GetEndPoints( std::vector< DANGLING_END_ITEM >& aItemList ) override;

    bool CanConnect( const SCH_ITEM* aItem ) const override
    {
        return aItem->Type() == SCH_LINE_T && aItem->GetLayer() == LAYER_BUS;
    }

    wxString GetSelectMenuText( EDA_UNITS_T aUnits ) const override;

    EDA_ITEM* Clone() const override;

    BITMAP_DEF GetMenuImage() const override;

    bool UpdateDanglingState( std::vector<DANGLING_END_ITEM>& aItemList ) override;

    /**
     * Pointer to the bus items (usually bus wires) connected to this bus-bus
     * entry (either or both may be nullptr)
     */
    SCH_ITEM* m_connected_bus_items[2];
};

#endif    // _SCH_BUS_ENTRY_H_

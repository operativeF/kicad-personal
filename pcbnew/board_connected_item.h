/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2012 Jean-Pierre Charras, jean-pierre.charras@ujf-grenoble.fr
 * Copyright (C) 2012 SoftPLC Corporation, Dick Hollenbeck <dick@softplc.com>
 * Copyright (C) 1992-2012 KiCad Developers, see AUTHORS.txt for contributors.
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

#ifndef BOARD_CONNECTED_ITEM_H
#define BOARD_CONNECTED_ITEM_H

#include <class_board_item.h>
#include <netinfo.h>
#include <kicad_string.h>

class NETCLASS;
class TRACK;
class D_PAD;

/**
 * Class BOARD_CONNECTED_ITEM
 * is a base class derived from BOARD_ITEM for items that can be connected
 * and have a net, a netname, a clearance ...
 * mainly: tracks, pads and zones
 * Handle connection info
 */
class BOARD_CONNECTED_ITEM : public BOARD_ITEM
{
public:
    BOARD_CONNECTED_ITEM( BOARD_ITEM* aParent, KICAD_T idtype );

    static inline bool ClassOf( const EDA_ITEM* aItem )
    {
        if( aItem == NULL )
            return false;

        switch( aItem->Type() )
        {
        case PCB_PAD_T:
        case PCB_TRACE_T:
        case PCB_VIA_T:
        case PCB_ZONE_AREA_T:
            return true;

        default:
            return false;
        }
    }

    ///> @copydoc BOARD_ITEM::IsConnected()
    bool IsConnected() const override
    {
        return true;
    }

    /**
     * Function GetNet
     * Returns NET_INFO object for a given item.
     */
    NETINFO_ITEM* GetNet() const
    {
        return m_netinfo;
    }

    /**
     * Function SetNet
     * Sets a NET_INFO object for the item.
     */
    void SetNet( NETINFO_ITEM* aNetInfo )
    {
        assert( aNetInfo->GetBoard() == GetBoard() );
        m_netinfo = aNetInfo;
    }

    /**
     * Function GetNetCode
     * @return int - the net code.
     */
    int GetNetCode() const
    {
        return m_netinfo ? m_netinfo->GetNet() : -1;
    }

    /**
     * Sets net using a net code.
     * @param aNetCode is a net code for the new net. It has to exist in NETINFO_LIST held by BOARD.
     * @param aNoAssert if true, do not assert that the net exists.
     * Otherwise, item is assigned to the unconnected net.
     * @return true on success, false if the net did not exist
     * Note also items (in fact pads) not on copper layers will have
     * their net code always set to 0 (not connected)
     */
    bool SetNetCode( int aNetCode, bool aNoAssert=false );

    /**
     * Function GetNetname
     * @return wxString - the full netname
     */
    const wxString& GetNetname() const
    {
        return m_netinfo->GetNetname();
    }

    /**
     * Function GetNetnameMsg
     * @return wxString - the full netname or "<no net>" in square braces, followed by
     * "(Not Found)" if the netcode is undefined.
     */
    wxString GetNetnameMsg() const
    {
        if( !GetBoard() )
            return wxT( "[** NO BOARD DEFINED **]" );

        wxString netname = GetNetname();

        if( !netname.length() )
            return wxT( "[<no net>]" );
        else if( GetNetCode() < 0 )
            return wxT( "[" + UnescapeString( netname ) + "](" + _( "Not Found" ) + ")" );
        else
            return wxT( "[" + UnescapeString( netname ) + "]" );
    }

    /**
     * Function GetShortNetname
     * @return wxString - the short netname
     */
    const wxString& GetShortNetname() const
    {
        return m_netinfo->GetShortNetname();
    }

    /**
     * Function GetClearance
     * returns the clearance in internal units.  If \a aItem is not NULL then the
     * returned clearance is the greater of this object's NETCLASS clearance and
     * aItem's NETCLASS clearance.  If \a aItem is NULL, then this objects clearance
     * is returned.
     * @param aItem is another BOARD_CONNECTED_ITEM or NULL
     * @return int - the clearance in internal units.
     */
    virtual int GetClearance( BOARD_CONNECTED_ITEM* aItem = NULL ) const;

     /**
      * Function GetNetClass
      * returns the NETCLASS for this item.
      */
    std::shared_ptr<NETCLASS> GetNetClass() const;

    /**
     * Function GetNetClassName
     * returns a pointer to the netclass of the zone.
     * If the net is not found (can happen when a netlist is reread,
     * and the net name does not exist, return the default net class
     * (should not return a null pointer).
     * @return the Net Class name of this item
     */
    wxString GetNetClassName() const;

    void SetLocalRatsnestVisible( bool aVisible )
    {
        m_localRatsnestVisible = aVisible;
    }

    bool GetLocalRatsnestVisible() const
    {
        return m_localRatsnestVisible;
    }

protected:
    /// Stores all informations about the net that item belongs to
    NETINFO_ITEM* m_netinfo;

private:

    bool m_localRatsnestVisible;
};


#if 0   // template for future
/**
 * Class BOARD_ITEM_LIST
 * is a container for a list of BOARD_ITEMs.
 */
class BOARD_ITEM_LIST : public BOARD_ITEM
{
    typedef boost::ptr_vector<BOARD_ITEM>   ITEM_ARRAY;

    ITEM_ARRAY   myItems;

    BOARD_ITEM_LIST( const BOARD_ITEM_LIST& other ) :
        BOARD_ITEM( NULL, PCB_ITEM_LIST_T )
    {
        // copy constructor is not supported, is private to cause compiler error
    }

public:

    BOARD_ITEM_LIST( BOARD_ITEM* aParent = NULL ) :
        BOARD_ITEM( aParent, PCB_ITEM_LIST_T )
    {}

    //-----< satisfy some virtual functions >------------------------------
    const wxPoint GetPosition()
    {
        return wxPoint(0, 0);   // dummy
    }

    void Print( PCB_BASE_FRAME* aFrame, wxDC* DC, const wxPoint& aOffset = ZeroOffset )
    {
    }

    void UnLink()
    {
        /* if it were needed:
        DHEAD* list = GetList();

        wxASSERT( list );

        list->remove( this );
        */
    }

    //-----</ satisfy some virtual functions >-----------------------------

    /**
     * Function GetCount
     * returns the number of BOARD_ITEMs.
     */
    int GetCount() const
    {
        return myItems.size();
    }

    void Append( BOARD_ITEM* aItem )
    {
        myItems.push_back( aItem );
    }

    BOARD_ITEM* Replace( int aIndex, BOARD_ITEM* aItem )
    {
        ITEM_ARRAY::auto_type ret = myItems.replace( aIndex, aItem );
        return ret.release();
    }

    BOARD_ITEM* Remove( int aIndex )
    {
        ITEM_ARRAY::auto_type ret = myItems.release( myItems.begin()+aIndex );
        return ret.release();
    }

    void Insert( int aIndex, BOARD_ITEM* aItem )
    {
        myItems.insert( myItems.begin()+aIndex, aItem );
    }

    BOARD_ITEM* At( int aIndex ) const
    {
        // we have varying sized objects and are using polymorphism, so we
        // must return a pointer not a reference.
        return (BOARD_ITEM*) &myItems[aIndex];
    }

    BOARD_ITEM* operator[]( int aIndex ) const override
    {
        return At( aIndex );
    }

    void Delete( int aIndex )
    {
        myItems.erase( myItems.begin()+aIndex );
    }

    void PushBack( BOARD_ITEM* aItem )
    {
        Append( aItem );
    }

    BOARD_ITEM* PopBack()
    {
        if( GetCount() )
            return Remove( GetCount()-1 );

        return NULL;
    }
};
#endif  // future

#endif  // BOARD_CONNECTED_ITEM_H

/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2018 Jean-Pierre Charras, jp.charras at wandadoo.fr
 * Copyright (C) 1992-2016 KiCad Developers, see AUTHORS.txt for contributors.
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
 * @file  class_board_item.h
 * @brief Classes BOARD_ITEM and BOARD_CONNECTED_ITEM.
 */

#ifndef BOARD_ITEM_STRUCT_H
#define BOARD_ITEM_STRUCT_H


#include <base_struct.h>
#include <convert_to_biu.h>
#include <gr_basic.h>
#include <layers_id_colors_and_visibility.h>


class BOARD;
class BOARD_ITEM_CONTAINER;
class SHAPE_POLY_SET;
class PCB_BASE_FRAME;


/**
 * Enum STROKE_T
 * is the set of shapes for segments (graphic segments and tracks) which are often
 * in the .m_Shape member
 */
enum STROKE_T
{
    S_SEGMENT = 0,  ///< usual segment : line with rounded ends
    S_RECT,         ///< segment with non rounded ends
    S_ARC,          ///< Arcs (with rounded ends)
    S_CIRCLE,       ///< ring
    S_POLYGON,      ///< polygon (not yet used for tracks, but could be in microwave apps)
    S_CURVE,        ///< Bezier Curve
    S_LAST          ///< last value for this list
};


/**
 * Class BOARD_ITEM
 * is a base class for any item which can be embedded within the BOARD
 * container class, and therefore instances of derived classes should only be
 * found in Pcbnew or other programs that use class BOARD and its contents.
 * The corresponding class in Eeschema is SCH_ITEM.
 */
class BOARD_ITEM : public EDA_ITEM
{

protected:
    PCB_LAYER_ID    m_Layer;

    static int getNextNumberInSequence( const std::set<int>& aSeq, bool aFillSequenceGaps );

public:

    BOARD_ITEM( BOARD_ITEM* aParent, KICAD_T idtype ) :
        EDA_ITEM( aParent, idtype ), m_Layer( F_Cu )
    {
    }

    virtual wxPoint GetPosition() const = 0;

    /**
     * Function GetCenter()
     *
     * This defaults to the same point as returned by GetPosition(), unless
     * overridden
     *
     * @return centre point of the item
     */
    virtual wxPoint GetCenter() const { return GetPosition(); }

    virtual void SetPosition( const wxPoint& aPos ) = 0;

    /**
     * Function IsConnected()
     * Returns information if the object is derived from BOARD_CONNECTED_ITEM.
     * @return True if the object is of BOARD_CONNECTED_ITEM type, false otherwise.
     */
    virtual bool IsConnected() const
    {
        return false;
    }

    /**
     * A value of wxPoint(0,0) which can be passed to the Draw() functions.
     */
    static wxPoint ZeroOffset;

    BOARD_ITEM_CONTAINER* GetParent() const { return (BOARD_ITEM_CONTAINER*) m_Parent; }

    /**
     * Function GetLayer
     * returns the primary layer this item is on.
     */
    virtual PCB_LAYER_ID GetLayer() const { return m_Layer; }

    /**
     * Function GetLayerSet
     * returns a "layer mask", which is a bitmap of all layers on which the
     * TRACK segment or VIA physically resides.
     * @return int - a layer mask, see layers_id_colors_visibility.h.
     */
    virtual LSET GetLayerSet() const { return LSET( m_Layer ); }

    /**
     * Function SetLayer
     * sets the layer this item is on.
     * @param aLayer The layer number.
     * is virtual because some items (in fact: class DIMENSION)
     * have a slightly different initialization
     */
    virtual void SetLayer( PCB_LAYER_ID aLayer )
    {
        // trap any invalid layers, then go find the caller and fix it.
        // wxASSERT( unsigned( aLayer ) < unsigned( NB_PCB_LAYERS ) );
        m_Layer = aLayer;
    }

    /**
     * Function Print
     * BOARD_ITEMs have their own color information.
     */
    virtual void Print( PCB_BASE_FRAME* aFrame, wxDC* DC, const wxPoint& offset = ZeroOffset ) = 0;

    /**
     * Swap data between aItem and aImage.
     * aItem and aImage should have the same type
     * Used in undo redo command to swap values between an item and its copy
     * Only values like layer, size .. which are modified by editing are swapped,
     * not the pointers like
     * Pnext and Pback because aItem is not changed in the linked list
     * @param aImage = the item image which contains data to swap
     */
    virtual void SwapData( BOARD_ITEM* aImage );

    /**
     * Function IsOnLayer
     * tests to see if this object is on the given layer.  Is virtual so
     * objects like D_PAD, which reside on multiple layers can do their own
     * form of testing.
     * @param aLayer The layer to test for.
     * @return bool - true if on given layer, else false.
     */
    virtual bool IsOnLayer( PCB_LAYER_ID aLayer ) const
    {
        return m_Layer == aLayer;
    }

    /**
     * Function IsTrack
     * tests to see if this object is a track or via (or microvia).
     * form of testing.
     * @return bool - true if a track or via, else false.
     */
    bool IsTrack() const
    {
        return ( Type() == PCB_TRACE_T ) || ( Type() == PCB_VIA_T );
    }

    /**
     * Function IsLocked
     * @return bool - true if the object is locked, else false
     */
    virtual bool IsLocked() const
    {
        // only MODULEs & TRACKs can be locked at this time.
        return false;
    }

    /**
     * Function SetLocked
     * modifies 'lock' status for of the item.
     */
    virtual void SetLocked( bool aLocked )
    {
        // only MODULEs & TRACKs can be locked at this time.
    }

    /**
     * Function UnLink
     * detaches this object from its owner.  This base class implementation
     * should work for all derived classes which are held in a DLIST<>.
     */
    virtual void UnLink();

    /**
     * Function DeleteStructure
     * deletes this object after UnLink()ing it from its owner if it has one.
     */
    void DeleteStructure();

    /**
     * Function ShowShape
     * converts the enum STROKE_T integer value to a wxString.
     */
    static wxString ShowShape( STROKE_T aShape );

    // Some geometric transforms, that must be rewritten for derived classes
    /**
     * Function Move
     * move this object.
     * @param aMoveVector - the move vector for this object.
     */
    virtual void Move( const wxPoint& aMoveVector )
    {
        wxFAIL_MSG( wxString::Format( wxT( "virtual BOARD_ITEM::Move called for %s" ),
                                      GetClass() ) );
    }

    void Move( const VECTOR2I& aMoveVector )
    {
        Move( wxPoint( aMoveVector.x, aMoveVector.y ) );
    }

    /**
     * Function Rotate
     * Rotate this object.
     * @param aRotCentre - the rotation point.
     * @param aAngle - the rotation angle in 0.1 degree.
     */
    virtual void Rotate( const wxPoint& aRotCentre, double aAngle )
    {
        wxMessageBox( wxT( "virtual BOARD_ITEM::Rotate used, should not occur" ), GetClass() );
    }

    void Rotate( const VECTOR2I& aRotCentre, double aAngle )
    {
        Rotate( wxPoint( aRotCentre.x, aRotCentre.y ), aAngle );
    }

    /**
     * Function Flip
     * Flip this object, i.e. change the board side for this object
     * @param aCentre - the rotation point.
     */
    virtual void Flip( const wxPoint& aCentre )
    {
        wxMessageBox( wxT( "virtual BOARD_ITEM::Flip used, should not occur" ), GetClass() );
    }

    void Flip( const VECTOR2I& aCentre )
    {
        Flip( wxPoint( aCentre.x, aCentre.y ) );
    }

    /**
     * Function GetBoard
     * returns the BOARD in which this BOARD_ITEM resides, or NULL if none.
     */
    virtual BOARD* GetBoard() const;

    /**
     * Function GetLayerName
     * returns the name of the PCB layer on which the item resides.
     *
     * @return wxString containing the layer name associated with this item.
     */
    wxString GetLayerName() const;

    virtual void ViewGetLayers( int aLayers[], int& aCount ) const override;

    /**
     * Function TransformShapeWithClearanceToPolygon
     * Convert the item shape to a closed polygon
     * Used in filling zones calculations
     * Circles and arcs are approximated by segments
     * @param aCornerBuffer = a buffer to store the polygon
     * @param aClearanceValue = the clearance around the pad
     * @param aError = the maximum deviation from true circle
     * @param ignoreLineWidth = used for edge cut items where the line width is only
     * for visualization
     */
    virtual void TransformShapeWithClearanceToPolygon( SHAPE_POLY_SET& aCornerBuffer,
            int aClearanceValue, int aError = ARC_LOW_DEF, bool ignoreLineWidth = false ) const;
};

#endif /* BOARD_ITEM_STRUCT_H */

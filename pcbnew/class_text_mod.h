/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2004 Jean-Pierre Charras, jaen-pierre.charras@gipsa-lab.inpg.com
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
 * @file class_text_mod.h
 * @brief Footprint text class description.
 */


#ifndef TEXT_MODULE_H_
#define TEXT_MODULE_H_


#include <eda_text.h>
#include <class_board_item.h>


class LINE_READER;
class EDA_RECT;
class MODULE;
class MSG_PANEL_ITEM;
class PCB_BASE_FRAME;


#define UMBILICAL_COLOR   LIGHTBLUE


class TEXTE_MODULE : public BOARD_ITEM, public EDA_TEXT
{
public:
    /** Text module type: there must be only one (and only one) for each
     * of the reference and value texts in one module; others could be
     * added for the user (DIVERS is French for 'others'). Reference and
     * value always live on silkscreen (on the module side); other texts
     * are planned to go on whatever layer the user wants (except
     * copper, probably) */
    enum TEXT_TYPE
    {
        TEXT_is_REFERENCE = 0,
        TEXT_is_VALUE = 1,
        TEXT_is_DIVERS = 2
    };

    TEXTE_MODULE( MODULE* parent, TEXT_TYPE text_type = TEXT_is_DIVERS );

    static inline bool ClassOf( const EDA_ITEM* aItem )
    {
        return aItem && PCB_MODULE_TEXT_T == aItem->Type();
    }

    virtual const wxPoint GetPosition() const override
    {
        return EDA_TEXT::GetTextPos();
    }

    virtual void SetPosition( const wxPoint& aPos ) override
    {
        EDA_TEXT::SetTextPos( aPos );
        SetLocalCoord();
    }

    void SetTextAngle( double aAngle );

    /**
     * Called when rotating the parent footprint.
     */
    void KeepUpright( double aOldOrientation, double aNewOrientation );

    /**
     * @return force the text rotation to be always between -90 .. 90 deg. Otherwise the text is not easy to read
     * if false, the text rotation is free.
     */
    bool IsKeepUpright()
    {
        return m_keepUpright;
    }

    void SetKeepUpright( bool aKeepUpright )
    {
        m_keepUpright = aKeepUpright;
    }

    /// Rotate text, in footprint editor
    /// (for instance in footprint rotation transform)
    void Rotate( const wxPoint& aOffset, double aAngle ) override;

    /// Flip entity during module flip
    void Flip( const wxPoint& aCentre, bool aFlipLeftRight ) override;

    bool IsParentFlipped() const;

    /// Mirror text position in footprint editing
    /// the text itself is not mirrored, and the layer not modified,
    /// only position is mirrored.
    /// (use Flip to change layer to its paired and mirror the text in fp editor).
    void Mirror( const wxPoint& aCentre, bool aMirrorAroundXAxis );

    /// move text in move transform, in footprint editor
    void Move( const wxPoint& aMoveVector ) override;

    /// @deprecated it seems (but the type is used to 'protect'
    //  reference and value from deletion, and for identification)
    void SetType( TEXT_TYPE aType )     { m_Type = aType; }
    TEXT_TYPE GetType() const           { return m_Type; }

    /**
     * Function SetEffects
     * sets the text effects from another instance.
     */
    void SetEffects( const TEXTE_MODULE& aSrc )
    {
        EDA_TEXT::SetEffects( aSrc );
        SetLocalCoord();
        // SetType( aSrc.GetType() );
    }

    /**
     * Function SwapEffects
     * swaps the text effects of the two involved instances.
     */
    void SwapEffects( TEXTE_MODULE& aTradingPartner )
    {
        EDA_TEXT::SwapEffects( aTradingPartner );
        SetLocalCoord();
        aTradingPartner.SetLocalCoord();
        // std::swap( m_Type, aTradingPartner.m_Type );
    }

    // The Pos0 accessors are for module-relative coordinates
    void SetPos0( const wxPoint& aPos ) { m_Pos0 = aPos; SetDrawCoord(); }
    const wxPoint& GetPos0() const      { return m_Pos0; }

    int GetLength() const;        // text length

    /**
     * @return the text rotation for drawings and plotting
     * the footprint rotation is taken in account
     */
    double GetDrawRotation() const;
    double GetDrawRotationRadians() const { return GetDrawRotation() * M_PI/1800; }

    // Virtual function
    const EDA_RECT GetBoundingBox() const override;

    ///> Set absolute coordinates.
    void SetDrawCoord();

    ///> Set relative coordinates.
    void SetLocalCoord();

    /* drawing functions */

    /**
     * Function Print
     * Print the text according to the footprint pos and orient
     * @param aFrame = the current Frame
     * @param aDC = Current Device Context
     * @param aOffset = draw offset (usually wxPoint(0,0)
     */
    void Print( PCB_BASE_FRAME* aFrame, wxDC* aDC, const wxPoint&  aOffset = ZeroOffset ) override;

    void GetMsgPanelInfo( EDA_UNITS_T aUnits, std::vector< MSG_PANEL_ITEM >& aList ) override;

    bool TextHitTest( const wxPoint& aPoint, int aAccuracy = 0 ) const override;
    bool TextHitTest( const EDA_RECT& aRect, bool aContains, int aAccuracy = 0 ) const override;

    bool HitTest( const wxPoint& aPosition, int aAccuracy ) const override
    {
        return TextHitTest( aPosition, aAccuracy );
    }

    bool HitTest( const EDA_RECT& aRect, bool aContained, int aAccuracy = 0 ) const override
    {
        return TextHitTest( aRect, aContained, aAccuracy );
    }

    wxString GetClass() const override
    {
        return wxT( "MTEXT" );
    }

    wxString GetSelectMenuText( EDA_UNITS_T aUnits ) const override;

    BITMAP_DEF GetMenuImage() const override;

    EDA_ITEM* Clone() const override;

    virtual wxString GetShownText() const override;

    virtual const BOX2I ViewBBox() const override;

    virtual void ViewGetLayers( int aLayers[], int& aCount ) const override;

    virtual unsigned int ViewGetLOD( int aLayer, KIGFX::VIEW* aView ) const override;

#if defined(DEBUG)
    virtual void Show( int nestLevel, std::ostream& os ) const override { ShowDummy( os ); }
#endif

private:
    /* Note: orientation in 1/10 deg relative to the footprint
     * Physical orient is m_Orient + m_Parent->m_Orient
     */

    TEXT_TYPE m_Type;       ///< 0=ref, 1=val, etc.

    wxPoint   m_Pos0;       ///< text coordinates relative to the footprint anchor, orient 0.
                            ///< text coordinate ref point is the text center

    bool      m_keepUpright;    ///< if true, keep rotation angle between -90 .. 90 deg.
                                ///< to keep the text more easy to read

};

#endif // TEXT_MODULE_H_

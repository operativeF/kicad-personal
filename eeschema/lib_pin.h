/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2015 Jean-Pierre Charras, jaen-pierre.charras at wanadoo.fr
 * Copyright (C) 2015 Wayne Stambaugh <stambaughw@gmail.com>
 * Copyright (C) 2004-2017 KiCad Developers, see change_log.txt for contributors.
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
 * @file class_libentry.h
 * @brief Class LIB_PIN definition.
 */
#ifndef CLASS_PIN_H
#define CLASS_PIN_H

class SCH_COMPONENT;

#include <eda_rect.h>
#include <lib_draw_item.h>

#include "pin_shape.h"
#include "pin_type.h"
#include "class_libentry.h"

// Circle diameter drawn at the active end of pins:
#define TARGET_PIN_RADIUS   15

// Pin visibility flag bit:
#define PIN_INVISIBLE 1    // Set makes pin invisible


/**
 *  The component library pin object orientations.
 */
enum DrawPinOrient {
    PIN_RIGHT = 'R',
    PIN_LEFT  = 'L',
    PIN_UP    = 'U',
    PIN_DOWN  = 'D'
};


class LIB_PIN : public LIB_ITEM
{
    // Unlike most of the other LIB_ITEMs, the SetXXX() routines on LIB_PINs are at the UI
    // level, performing additional pin checking, multi-pin editing, and setting the modified
    // flag.  So the LEGACY_PLUGIN_CACHE needs direct access to the member variables.
    friend class SCH_LEGACY_PLUGIN_CACHE;

    wxPoint  m_position;     ///< Position of the pin.
    int      m_length;       ///< Length of the pin.
    int      m_orientation;  ///< Pin orientation (Up, Down, Left, Right)
    GRAPHIC_PINSHAPE m_shape;        ///< Shape drawn around pin
    int      m_width;        ///< Line width of the pin.
    ELECTRICAL_PINTYPE m_type;  ///< Electrical type of the pin.  See enum ELECTRICAL_PINTYPE.
    int      m_attributes;   ///< Set bit 0 to indicate pin is invisible.
    wxString m_name;
    wxString m_number;
    int      m_numTextSize;
    int      m_nameTextSize; ///< Pin num and Pin name sizes

    /**
     * Print a pin, with or without the pin texts
     *
     * @param aDC Device Context (can be null)
     * @param aOffset Offset to draw
     * @param aData = used here as a boolean indicating whether or not to draw the pin
     *                electrical types
     * @param aTransform Transform Matrix (rotation, mirror ..)
     */
    void print( wxDC* aDC, const wxPoint& aOffset, void* aData,
                const TRANSFORM& aTransform ) override;

public:
    LIB_PIN( LIB_PART* aParent );

    wxString GetClass() const override
    {
        return wxT( "LIB_PIN" );
    }

    wxString GetTypeName() override
    {
        return _( "Pin" );
    }

#if defined(DEBUG)
    void Show( int nestLevel, std::ostream& os ) const override;
#endif

    bool HitTest( const wxPoint& aPosition, int aAccuracy = 0 ) const override;

    void GetMsgPanelInfo( EDA_UNITS_T aUnits, std::vector< MSG_PANEL_ITEM >& aList ) override;

    /**
     * Display pin info (given by GetMsgPanelInfo) and add some info related to aComponent
     * (schematic pin position, and sheet path)
     * @param aList is the message list to fill
     * @param aComponent is the component which "owns" the pin
     */
    void GetMsgPanelInfo( EDA_UNITS_T aUnits, std::vector< MSG_PANEL_ITEM >& aList,
                          SCH_COMPONENT* aComponent );

    bool Matches( wxFindReplaceData& aSearchData, void* aAuxData ) override;

    /* Cannot use a default parameter here as it will not be compatible with the virtual. */
    const EDA_RECT GetBoundingBox() const override { return GetBoundingBox( false ); }

    /**
     * @param aIncludeInvisibles - if false, do not include labels for invisible pins
     *      in the calculation.
     */
    const EDA_RECT GetBoundingBox( bool aIncludeInvisibles ) const;

    /**
     * @return The pin end position for a component in the normal orientation.
     */
    wxPoint PinEndPoint() const;

    /**
     * Return the pin real orientation (PIN_UP, PIN_DOWN, PIN_RIGHT, PIN_LEFT),
     * according to its orientation and the matrix transform (rot, mirror) \a aTransform.
     *
     * @param aTransform Transform matrix
     */
    int PinDrawOrient( const TRANSFORM& aTransform ) const;

    const wxString& GetName() const
    {
        return m_name;
    }

    /**
     * Set the pin name.
     *
     * This will also all of the pin names marked by EnableEditMode().
     *
     * @param aName New pin name.
     * @param aTestOtherPins determines if other pins need to be updated
     */
    void SetName( const wxString& aName, bool aTestOtherPins = true );

    /**
     * Set the \a aSize of the pin name text.
     *
     * This will also update the text size of the name of the pins marked
     * by EnableEditMode().
     *
     * @param aSize The text size of the pin name in schematic units ( mils ).
     * @param aTestOtherPins determines if other pins need to be updated
     */
    void SetNameTextSize( int aSize, bool aTestOtherPins = true );

    int GetNameTextSize() const { return m_nameTextSize; }

    const wxString& GetNumber() const
    {
        return m_number;
    }

    /**
     * Set the pin number.
     *
     * Others pin numbers marked by EnableEditMode() are not modified
     * because each pin has its own number
     * @param aNumber New pin number.
     */
    void SetNumber( const wxString& aNumber );

    /**
     * Set the size of the pin number text.
     *
     * This will also update the text size of the number of the pins marked
     * by EnableEditMode().
     *
     * @param aSize The text size of the pin number in schematic units ( mils ).
     * @param aTestOtherPins determines if other pins need to be updated
     */
    void SetNumberTextSize( int aSize, bool aTestOtherPins = true );

    int GetNumberTextSize() const { return m_numTextSize; }

    int GetOrientation() const { return m_orientation; }

    /**
     * Set orientation on the pin.
     *
     * This will also update the orientation of the pins marked by EnableEditMode().
     *
     * @param aOrientation - The orientation of the pin.
     * @param aTestOtherPins determines if other pins need to be updated
     */
    void SetOrientation( int aOrientation, bool aTestOtherPins = true );

    GRAPHIC_PINSHAPE GetShape() const { return m_shape; }

    /**
     * Set the shape of the pin to \a aShape.
     *
     * This will also update the draw style of the pins marked by EnableEditMode().
     *
     * @param aShape - The draw shape of the pin.  See enum GRAPHIC_PINSHAPE.
     */
    void SetShape( GRAPHIC_PINSHAPE aShape );

    /**
     * Get the electrical type of the pin.
     *
     * @return The electrical type of the pin (see enum ELECTRICAL_PINTYPE for values).
     */
    ELECTRICAL_PINTYPE GetType() const { return m_type; }

    /**
     * return a string giving the electrical type of a pin.
     * Can be used when a known, not translated name is needed (for instance in net lists)
     * @param aType is the electrical type (see enum ELECTRICAL_PINTYPE )
     * @return The electrical name for a pin type (see enun MsgPinElectricType for names).
     */
    static const wxString GetCanonicalElectricalTypeName( ELECTRICAL_PINTYPE aType );

    /**
     * return a string giving the electrical type of the pin.
     * Can be used when a known, not translated name is needed (for instance in net lists)
     * @return The canonical electrical name of the pin.
     */
    wxString const GetCanonicalElectricalTypeName() const
    {
        return GetCanonicalElectricalTypeName( m_type );
    }

    /**
     * return a translated string for messages giving the electrical type of the pin.
     * @return The electrical name of the pin.
     */
    wxString const GetElectricalTypeName() const
    {
        return GetText( m_type );
    }

    /**
     * Set the electrical type of the pin.
     *
     * This will also update the electrical type of the pins marked by
     * EnableEditMode().
     *
     * @param aType - The electrical type of the pin(see enun ELECTRICAL_PINTYPE for values).
     * @param aTestOtherPins determines if other pins need to be updated
     */
    void SetType( ELECTRICAL_PINTYPE aType, bool aTestOtherPins = true );

    /**
     * Set the pin length.
     *
     * This will also update the length of the pins marked by EnableEditMode().
     *
     * @param aLength - The length of the pin in mils.
     * @param aTestOtherPins determines if other pins need to be updated
     */
    void SetLength( int aLength, bool aTestOtherPins = true );

    int GetLength() { return m_length; }

    /**
     * Set the pin part number.
     *
     * If the pin is changed from not common to common to all parts, any
     * linked pins will be removed from the parent component.
     *
     * @param aPart - Number of the part the pin belongs to.  Set to zero to
     *                make pin common to all parts in a multi-part component.
     */
    void SetPartNumber( int aPart );

    /** Get the pin part number. */
    int GetPartNumber() const { return m_Unit; }

    /**
     * Set the body style (conversion) of the pin.
     *
     * If the pin is changed from not common to common to all body styles, any
     * linked pins will be removed from the parent component.
     *
     * @param aConversion - Body style of the pin.  Set to zero to make pin
     *                      common to all body styles.
     */
    void SetConversion( int aConversion );

    /**
     * Set or clear the visibility flag for the pin.
     *
     * This will also update the visibility of the pins marked by
     * EnableEditMode().
     *
     * @param aVisible - True to make the pin visible or false to hide the pin.
     */
    void SetVisible( bool aVisible );

    /**
     * Enable or clear pin editing mode.
     *
     * The pin editing mode marks or unmarks all pins common to this
     * pin object for further editing.  If any of the pin modification
     * methods are called after enabling the editing mode, all pins
     * marked for editing will have the same attribute changed.  The
     * only case were this is not true making this pin common to all
     * parts or body styles in the component.  See SetCommonToAllParts()
     * and SetCommonToAllBodyStyles() for more information.
     *
     * @param aEnable = true marks all common pins for editing mode.  False
     *                clears the editing mode.
     * @param aEditPinByPin == true enables the edit pin by pin mode.
     * aEditPinByPin == false enables the pin edit coupling between pins at the same location
     * if aEnable == false, aEditPinByPin is not used
     */
    void EnableEditMode( bool aEnable, bool aEditPinByPin = false );

    /**
     * Return the visibility status of the draw object.
     *
     * @return True if draw object is visible otherwise false.
     */
    bool IsVisible() const { return ( m_attributes & PIN_INVISIBLE ) == 0; }

    /**
     * Return whether this pin forms an implicit power connection: i.e., is hidden
     * and of type POWER_IN.
     */
    bool IsPowerConnection() const {
        return GetType() == PIN_POWER_IN && ( !IsVisible() || (LIB_PART*) GetParent()->IsPower() );
    }

    int GetPenSize() const override;

    /**
     * Print the pin symbol without text.
     * If \a aColor != 0, draw with \a aColor, else with the normal pin color.
     */
    void PrintPinSymbol( wxDC* aDC, const wxPoint& aPos, int aOrientation );

    /**
     * Put the pin number and pin text info, given the pin line coordinates.
     * The line must be vertical or horizontal.
     * If DrawPinName == false the pin name is not printed.
     * If DrawPinNum = false the pin number is not printed.
     * If TextInside then the text is been put inside,otherwise all is drawn outside.
     * Pin Name:    substring between '~' is negated
     */
    void PrintPinTexts( wxDC* aDC, wxPoint& aPosition, int aOrientation, int TextInside,
                        bool DrawPinNum, bool DrawPinName );

    /**
     * Draw the electrical type text of the pin (only for the footprint editor)
     */
    void PrintPinElectricalTypeName( wxDC* aDC, wxPoint& aPosition, int aOrientation );

    /**
     * Plot the pin number and pin text info, given the pin line coordinates.
     * Same as DrawPinTexts((), but output is the plotter
     * The line must be vertical or horizontal.
     * If TextInside then the text is been put inside (moving from x1, y1 in
     * the opposite direction to x2,y2), otherwise all is drawn outside.
     */
    void PlotPinTexts( PLOTTER *aPlotter, wxPoint& aPosition, int aOrientation,
                       int aTextInside, bool aDrawPinNum, bool aDrawPinName, int aWidth );

    void PlotSymbol( PLOTTER* aPlotter, const wxPoint& aPosition, int aOrientation );

    /**
     * Get a list of pin orientation names.
     *
     * @return List of valid pin orientation names.
     */
    static wxArrayString GetOrientationNames();

    /**
     * Get a list of pin orientation bitmaps for menus and dialogs.
     *
     * @return  List of valid pin orientation bitmaps symbols in .xpm format
     */
    static const BITMAP_DEF* GetOrientationSymbols();

    /**
     * Get the orientation code by index used to set the pin orientation.
     *
     * @param aIndex - The index of the orientation code to look up.
     * @return Orientation code if index is valid.  Returns right
     *         orientation on index error.
     */
    static int GetOrientationCode( int aIndex );

    /**
     * Get the index of the orientation code.
     *
     * @param aCode - The orientation code to look up.
     * @return  The index of the orientation code if found.  Otherwise,
     *          return wxNOT_FOUND.
     */
    static int GetOrientationIndex( int aCode );

    void Offset( const wxPoint& aOffset ) override;

    bool Inside( EDA_RECT& aRect ) const override;

    void MoveTo( const wxPoint& aPosition ) override;

    wxPoint GetPosition() const override { return m_position; }

    /**
     * move this and all linked pins to the new position
     * used in pin editing.
     * use SetPinPosition to set the position of this only
     * @param aPosition is the new position of this and linked pins
     */
    void SetPinPosition( wxPoint aPosition );

    void MirrorHorizontal( const wxPoint& aCenter ) override;
    void MirrorVertical( const wxPoint& aCenter ) override;
    void Rotate( const wxPoint& aCenter, bool aRotateCCW = true ) override;

    void Plot( PLOTTER* aPlotter, const wxPoint& aOffset, bool aFill,
               const TRANSFORM& aTransform ) override;

    int GetWidth() const override { return m_width; }
    void SetWidth( int aWidth ) override;

    BITMAP_DEF GetMenuImage() const override;

    wxString GetSelectMenuText( EDA_UNITS_T aUnits ) const override;

    EDA_ITEM* Clone() const override;

    void CalcEdit( const wxPoint& aPosition ) override;

private:
    /**
     * Build the pin basic info to display in message panel.
     * they are pin info without the actual pin position, which
     * is not known in schematic without knowing the parent component
     */
    void getMsgPanelInfoBase( EDA_UNITS_T aUnits, std::vector< MSG_PANEL_ITEM >& aList );


    /**
     * @copydoc LIB_ITEM::compare()
     *
     * The pin specific sort order is as follows:
     *      - Pin number.
     *      - Pin name, case insensitive compare.
     *      - Pin horizontal (X) position.
     *      - Pin vertical (Y) position.
     */
    int compare( const LIB_ITEM& aOther ) const override;
};


#endif  //  CLASS_PIN_H

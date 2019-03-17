/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2015 Jean-Pierre Charras, jp.charras at wanadoo.fr
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

#ifndef SCH_SHEEET_H
#define SCH_SHEEET_H

#include <boost/ptr_container/ptr_vector.hpp>
#include <sch_text.h>


class LINE_READER;
class SCH_SCREEN;
class SCH_SHEET;
class SCH_SHEET_PIN;
class SCH_SHEET_PATH;
class DANGLING_END_ITEM;
class SCH_EDIT_FRAME;
class NETLIST_OBJECT_LIST;


#define MIN_SHEET_WIDTH  500
#define MIN_SHEET_HEIGHT 150


/**
 * Defines the edge of the sheet that the sheet pin is positioned
 * SHEET_LEFT_SIDE = 0: pin on left side
 * SHEET_RIGHT_SIDE = 1: pin on right side
 * SHEET_TOP_SIDE = 2: pin on top side
 * SHEET_BOTTOM_SIDE =3: pin on bottom side
 *
 * For compatibility reasons, this does not follow same values as text orientation.
 */
enum SHEET_SIDE
{
    SHEET_LEFT_SIDE = 0,
    SHEET_RIGHT_SIDE,
    SHEET_TOP_SIDE,
    SHEET_BOTTOM_SIDE,
    SHEET_UNDEFINED_SIDE
};


/**
 * Define a sheet pin (label) used in sheets to create hierarchical schematics.
 *
 * A SCH_SHEET_PIN is used to create a hierarchical sheet in the same way a
 * pin is used in a component.  It connects the objects in the sheet object
 * to the objects in the schematic page to the objects in the page that is
 * represented by the sheet.  In a sheet object, a SCH_SHEET_PIN must be
 * connected to a wire, bus, or label.  In the schematic page represented by
 * the sheet, it corresponds to a hierarchical label.
 */
class SCH_SHEET_PIN : public SCH_HIERLABEL
{
private:
    int m_number;       ///< Label number use for saving sheet label to file.
                        ///< Sheet label numbering begins at 2.
                        ///< 0 is reserved for the sheet name.
                        ///< 1 is reserve for the sheet file name.

    SHEET_SIDE m_edge;

public:
    SCH_SHEET_PIN( SCH_SHEET* parent,
                   const wxPoint& pos = wxPoint( 0, 0 ),
                   const wxString& text = wxEmptyString );

    wxString GetClass() const override
    {
        return wxT( "SCH_SHEET_PIN" );
    }

    bool operator ==( const SCH_SHEET_PIN* aPin ) const;

    /**
     * Return true for items which are moved with the anchor point at mouse cursor
     * and false for items moved with no reference to anchor (usually large items)
     *
     * @return true for a hierarchical sheet pin
     */
    bool IsMovableFromAnchorPoint() override { return true; }

    void Print( wxDC* aDC, const wxPoint& aOffset ) override;

    /**
     * Calculate the graphic shape (a polygon) associated to the text.
     *
     * @param aPoints = a buffer to fill with polygon corners coordinates
     * @param aPos = Position of the shape
     */
    void CreateGraphicShape( std::vector <wxPoint>& aPoints, const wxPoint& aPos ) override;

    void SwapData( SCH_ITEM* aItem ) override;

    int GetPenSize() const override;

    /**
     * Get the sheet label number.
     *
     * @return Number of the sheet label.
     */
    int GetNumber() const { return m_number; }

    /**
     * Set the sheet label number.
     *
     * @param aNumber - New sheet number label.
     */
    void SetNumber( int aNumber );

    void SetEdge( SHEET_SIDE aEdge );

    SHEET_SIDE GetEdge() const;

    /**
     * Adjust label position to edge based on proximity to vertical or horizontal edge
     * of the parent sheet.
     */
    void ConstrainOnEdge( wxPoint Pos );

    /**
     * Get the parent sheet object of this sheet pin.
     *
     * @return The sheet that is the parent of this sheet pin or NULL if it does
     *         not have a parent.
     */
    SCH_SHEET* GetParent() const { return (SCH_SHEET*) m_Parent; }

#if defined(DEBUG)
    void Show( int nestLevel, std::ostream& os ) const override;
#endif

    // Geometric transforms (used in block operations):

    void Move( const wxPoint& aMoveVector ) override
    {
        Offset( aMoveVector );
    }

    void MirrorX( int aXaxis_position ) override;
    void MirrorY( int aYaxis_position ) override;
    void Rotate( wxPoint aPosition ) override;

    bool Matches( wxFindReplaceData& aSearchData, void* aAuxData ) override;

    bool Replace( wxFindReplaceData& aSearchData, void* aAuxData = NULL ) override
    {
        return EDA_ITEM::Replace( aSearchData, m_Text );
    }

    bool IsReplaceable() const override { return true; }

    void GetEndPoints( std::vector< DANGLING_END_ITEM >& aItemList ) override;

    bool IsConnectable() const override { return true; }

    wxString GetSelectMenuText( EDA_UNITS_T aUnits ) const override;

    BITMAP_DEF GetMenuImage() const override;

    void SetPosition( const wxPoint& aPosition ) override { ConstrainOnEdge( aPosition ); }

    bool HitTest( const wxPoint& aPosition, int aAccuracy = 0 ) const override;

    EDA_ITEM* Clone() const override;
};


typedef boost::ptr_vector<SCH_SHEET_PIN> SCH_SHEET_PINS;


/**
 * Sheet symbol placed in a schematic, and is the entry point for a sub schematic.
 */
class SCH_SHEET : public SCH_ITEM
{
    friend class SCH_SHEET_PIN;

    /// Screen that contains the physical data for the sheet.  In complex hierarchies
    /// multiple sheets can share a common screen.
    SCH_SCREEN* m_screen;

    /// The list of sheet connection points.
    SCH_SHEET_PINS m_pins;

    /// The file name is also in the #SCH_SCREEN object associated with the sheet.  It is
    /// also needed here for loading after reading the sheet description from file.
    wxString m_fileName;

    /// This is equivalent to the reference designator for components and is stored in F0
    /// sheet pin in the schematic file.
    wxString m_name;

    /// The height of the text used to draw the sheet name.
    int m_sheetNameSize;

    /// The height of the text used to draw the file name.
    int m_fileNameSize;

    /// The position of the sheet.
    wxPoint m_pos;

    /// The size of the sheet.
    wxSize m_size;

public:
    SCH_SHEET( const wxPoint& pos = wxPoint( 0, 0 ) );

    /**
     * Copy \a aSheet into a new object.  All sheet pins are copied as is except and
     * the SCH_SHEET_PIN's #m_Parent pointers are set to the new copied parent object.
     */
    SCH_SHEET( const SCH_SHEET& aSheet );

    ~SCH_SHEET();

    wxString GetClass() const override
    {
        return wxT( "SCH_SHEET" );
    }

    /**
     * Return true for items which are moved with the anchor point at mouse cursor
     * and false for items moved with no reference to anchor.
     *
     * Usually return true for small items (labels, junctions) and false for
     * items which can be large (hierarchical sheets, compoments)
     *
     * @return false for a hierarchical sheet
     */
    bool IsMovableFromAnchorPoint() override { return false; }

    wxString GetName() const { return m_name; }
    void SetName( const wxString& aName ) { m_name = aName; }

    int GetSheetNameSize() const { return m_sheetNameSize; }
    void SetSheetNameSize( int aSize ) { m_sheetNameSize = aSize; }

    int GetFileNameSize() const { return m_fileNameSize; }
    void SetFileNameSize( int aSize ) { m_fileNameSize = aSize; }

    SCH_SCREEN* GetScreen() { return m_screen; }

    wxSize GetSize() { return m_size; }
    void SetSize( const wxSize& aSize ) { m_size = aSize; }

    /**
     * Return the root sheet of this SCH_SHEET object.
     *
     * The root (top level) sheet can be found by walking up the parent links until the only
     * sheet that has no parent is found.  The root sheet can be found from any sheet without
     * having to maintain a global root sheet pointer.
     *
     * @return a SCH_SHEET pointer to the root sheet.
     */
    SCH_SHEET* GetRootSheet();

    /**
     * Set the #SCH_SCREEN associated with this sheet to \a aScreen.
     *
     * The screen reference counting is performed by SetScreen.  If \a aScreen is not
     * the same as the current screen, the current screen reference count is decremented
     * and \a aScreen becomes the screen for the sheet.  If the current screen reference
     * count reaches zero, the current screen is deleted.  NULL is a valid value for
     * \a aScreen.
     *
     * @param aScreen The new screen to associate with the sheet.
     */
    void SetScreen( SCH_SCREEN* aScreen );

    /**
     * Return the number of times the associated screen for the sheet is being used.
     *
     * If no screen is associated with the sheet, then zero is returned.
     */
    int GetScreenCount() const;

    void GetMsgPanelInfo( EDA_UNITS_T aUnits, std::vector< MSG_PANEL_ITEM >& aList ) override;

    /* there is no member for orientation in sch_sheet, to preserve file
     * format, we detect orientation based on pin edges
     */
    bool IsVerticalOrientation() const;

    /**
     * Add aSheetPin to the sheet.
     *
     * Note: Once a sheet pin is added to the sheet, it is owned by the sheet.
     *       Do not delete the sheet pin object or you will likely get a segfault
     *       when the sheet is destroyed.
     *
     * @param aSheetPin The sheet pin item to add to the sheet.
     */
    void AddPin( SCH_SHEET_PIN* aSheetPin );

    SCH_SHEET_PINS& GetPins() { return m_pins; }

    SCH_SHEET_PINS& GetPins() const
    {
        return const_cast< SCH_SHEET_PINS& >( m_pins );
    }

    /**
     * Remove \a aSheetPin from the sheet.
     *
     * @param aSheetPin The sheet pin item to remove from the sheet.
     */
    void RemovePin( SCH_SHEET_PIN* aSheetPin );

    /**
     * Delete sheet label which do not have a corresponding hierarchical label.
     *
     * Note: Make sure you save a copy of the sheet in the undo list before calling
     *       CleanupSheet() otherwise any unreferenced sheet labels will be lost.
     */
    void CleanupSheet();

    /**
     * Return the sheet pin item found at \a aPosition in the sheet.
     *
     * @param aPosition The position to check for a sheet pin.
     *
     * @return The sheet pin found at \a aPosition or NULL if no sheet pin is found.
     */
    SCH_SHEET_PIN* GetPin( const wxPoint& aPosition );

    /**
     * Checks if the sheet already has a sheet pin named \a aName.
     *
     * @param aName Name of the sheet pin to search for.
     *
     * @return  True if sheet pin with \a aName is found, otherwise false.
     */
    bool HasPin( const wxString& aName );

    bool HasPins() { return !m_pins.empty(); }

    /**
     * Check all sheet labels against schematic for undefined hierarchical labels.
     *
     * @return True if there are any undefined labels.
     */
    bool HasUndefinedPins();

    /**
     * Return the minimum width of the sheet based on the widths of the sheet pin text.
     *
     * The minimum sheet width is determined by the width of the bounding box of each
     * hierarchical sheet pin.  If two pins are horizontally adjacent ( same Y position )
     * to each other, the sum of the bounding box widths is used.  If at some point in
     * the future sheet objects can be rotated or pins can be placed in the vertical
     * orientation, this function will need to be changed.
     *
     * @return The minimum width the sheet can be resized.
     */
    int GetMinWidth() const;

    /**
     * Return the minimum height that the sheet can be resized based on the sheet pin positions.
     *
     * The minimum width of a sheet is determined by the Y axis location of the bottom
     * most sheet pin.  If at some point in the future sheet objects can be rotated or
     * pins can be placed in the vertical orientation, this function will need to be
     * changed.
     *
     * @return The minimum height the sheet can be resized.
     */
    int GetMinHeight() const;

    int GetPenSize() const override;

    void Print( wxDC* aDC, const wxPoint& aOffset ) override;

    EDA_RECT const GetBoundingBox() const override;

    /**
     * Return the position of the lower right corner of the sheet in drawing units.
     *
     * @return A wxPoint containing lower right corner of the sheet in drawing units.
     */
    wxPoint GetResizePosition() const;

    void SwapData( SCH_ITEM* aItem ) override;

    /**
     * Count our own components, without the power components.
     *
     *  @return the component count.
     */
    int ComponentCount();

    /**
     * Search the existing hierarchy for an instance of screen loaded from \a aFileName.
     *
     * @param aFilename = the filename to find (MUST be absolute, and in wxPATH_NATIVE encoding)
     * @param aScreen = a location to return a pointer to the screen (if found)
     * @return bool if found, and a pointer to the screen
     */
    bool SearchHierarchy( const wxString& aFilename, SCH_SCREEN** aScreen );

    /**
     * Search the existing hierarchy for an instance of screen loaded from \a aFileName.
     *
     * Don't bother looking at the root sheet, it must be unique.  No other references to
     * its m_screen otherwise there would be loops in the hierarchy.
     *
     * @param aScreen = the SCH_SCREEN* screen that we search for
     * @param aList = the SCH_SHEET_PATH*  that must be used
     * @return true if found
     */
    bool LocatePathOfScreen( SCH_SCREEN* aScreen, SCH_SHEET_PATH* aList );

    /**
     * Count the number of sheets found in "this" sheet includeing all of the subsheets.
     *
     * @return the full count of sheets+subsheets contained by "this"
     */
    int CountSheets();

    /**
     * Return the filename corresponding to this sheet.
     *
     * @return a wxString containing the filename
     */
    wxString GetFileName( void ) const;

    // Set a new filename without changing anything else
    void SetFileName( const wxString& aFilename )
    {
        m_fileName = aFilename;
        // Filenames are stored using unix notation
        m_fileName.Replace( wxT("\\"), wxT("/") );
    }

    bool ChangeFileName( SCH_EDIT_FRAME* aFrame, const wxString& aFileName );

    // Geometric transforms (used in block operations):

    void Move( const wxPoint& aMoveVector ) override
    {
        m_pos += aMoveVector;

        for( SCH_SHEET_PIN& pin : m_pins )
        {
            pin.Move( aMoveVector );
        }
    }

    void MirrorY( int aYaxis_position ) override;
    void MirrorX( int aXaxis_position ) override;
    void Rotate( wxPoint aPosition ) override;

    bool Matches( wxFindReplaceData& aSearchData, void* aAuxData ) override;

    bool Replace( wxFindReplaceData& aSearchData, void* aAuxData = NULL ) override;

    bool IsReplaceable() const override { return true; }

    /**
     * Resize this sheet to aSize and adjust all of the labels accordingly.
     *
     * @param aSize - The new size for this sheet.
     */
    void Resize( const wxSize& aSize );

    /**
     * @return the position of the anchor of sheet name text
     */
    wxPoint GetSheetNamePosition();

    /**
     * @return the position of the anchor of filename text
     */
    wxPoint GetFileNamePosition();

    void GetEndPoints( std::vector <DANGLING_END_ITEM>& aItemList ) override;

    bool UpdateDanglingState( std::vector<DANGLING_END_ITEM>& aItemList ) override;

    bool IsConnectable() const override { return true; }

    bool CanConnect( const SCH_ITEM* aItem ) const override
    {
        return ( aItem->Type() == SCH_LINE_T && aItem->GetLayer() == LAYER_WIRE ) ||
                ( aItem->Type() == SCH_LINE_T && aItem->GetLayer() == LAYER_BUS ) ||
                ( aItem->Type() == SCH_NO_CONNECT_T );
    }

    void GetConnectionPoints( std::vector< wxPoint >& aPoints ) const override;

    SEARCH_RESULT Visit( INSPECTOR inspector, void* testData, const KICAD_T scanTypes[] ) override;

    wxString GetSelectMenuText( EDA_UNITS_T aUnits ) const override;

    BITMAP_DEF GetMenuImage() const override;

    void GetNetListItem( NETLIST_OBJECT_LIST& aNetListItems,
                         SCH_SHEET_PATH*      aSheetPath ) override;

    SCH_ITEM& operator=( const SCH_ITEM& aSheet );

    void ViewGetLayers( int aLayers[], int& aCount ) const override;

    wxPoint GetPosition() const override { return m_pos; }
    void SetPosition( const wxPoint& aPosition ) override;

    bool HitTest( const wxPoint& aPosition, int aAccuracy ) const override;
    bool HitTest( const EDA_RECT& aRect, bool aContained, int aAccuracy = 0 ) const override;

    void Plot( PLOTTER* aPlotter ) override;

    EDA_ITEM* Clone() const override;

#if defined(DEBUG)
    void Show( int nestLevel, std::ostream& os ) const override;
#endif

protected:

    /**
     * Renumber the sheet pins in the sheet.
     *
     * This method is used internally by SCH_SHEET to update the pin numbering
     * when the pin list changes.  Make sure you call this method any time a
     * sheet pin is added or removed.
     */
    void renumberPins();
};


//typedef std::vector< SCH_SHEET* > SCH_SHEETS;   // no ownership over contained SCH_SHEETs

#endif // SCH_SHEEET_H

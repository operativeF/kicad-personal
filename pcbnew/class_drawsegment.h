/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2018 Jean-Pierre Charras jp.charras at wanadoo.fr
 * Copyright (C) 1992-2018 KiCad Developers, see AUTHORS.txt for contributors.
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
 * @file class_drawsegment.h
 * @brief Class to handle a graphic segment.
 */

#ifndef CLASS_DRAWSEGMENT_H_
#define CLASS_DRAWSEGMENT_H_

#include <class_board_item.h>
#include <common.h>
#include <convert_to_biu.h>
#include <math_for_graphics.h>
#include <trigo.h>

#include <geometry/shape_poly_set.h>

class LINE_READER;
class EDA_DRAW_FRAME;
class MODULE;
class MSG_PANEL_ITEM;


class DRAWSEGMENT : public BOARD_ITEM
{
protected:
    int         m_Width;        ///< thickness of lines ...
    wxPoint     m_Start;        ///< Line start point or Circle and Arc center
    wxPoint     m_End;          ///< Line end point or circle and arc start point

    STROKE_T    m_Shape;        ///< Shape: line, Circle, Arc
    int         m_Type;         ///< Used in complex associations ( Dimensions.. )
    double      m_Angle;        ///< Used only for Arcs: Arc angle in 1/10 deg
    wxPoint     m_BezierC1;     ///< Bezier Control Point 1
    wxPoint     m_BezierC2;     ///< Bezier Control Point 2

    std::vector<wxPoint> m_BezierPoints;
    SHAPE_POLY_SET m_Poly;      ///< Stores the S_POLYGON shape

    // Computes the bounding box for an arc
    void computeArcBBox( EDA_RECT& aBBox ) const;

public:
    DRAWSEGMENT( BOARD_ITEM* aParent = NULL, KICAD_T idtype = PCB_LINE_T );

    static inline bool ClassOf( const EDA_ITEM* aItem )
    {
        return aItem && PCB_LINE_T == aItem->Type();
    }

    /** Polygonal shape is not always filled.
     * For now it is filled on all layers but Edge_Cut layer
     */
    bool IsPolygonFilled() const { return m_Layer != Edge_Cuts; }

    void SetWidth( int aWidth )             { m_Width = aWidth; }
    int GetWidth() const                    { return m_Width; }

    /**
     * Function SetAngle
     * sets the angle for arcs, and normalizes it within the range 0 - 360 degrees.
     * @param aAngle is tenths of degrees, but will soon be degrees.
     */
    void SetAngle( double aAngle );     // encapsulates the transition to degrees
    double GetAngle() const { return m_Angle; }

    void SetType( int aType )                       { m_Type = aType; }
    int GetType() const                             { return m_Type; }

    void SetShape( STROKE_T aShape )                { m_Shape = aShape; }
    STROKE_T GetShape() const                       { return m_Shape; }

    void SetBezControl1( const wxPoint& aPoint )    { m_BezierC1 = aPoint; }
    const wxPoint& GetBezControl1() const           { return m_BezierC1; }

    void SetBezControl2( const wxPoint& aPoint )    { m_BezierC2 = aPoint; }
    const wxPoint& GetBezControl2() const           { return m_BezierC2; }

    void SetPosition( const wxPoint& aPos ) override;
    const wxPoint GetPosition() const override;

    /**
     * Function GetStart
     * returns the starting point of the graphic
     */
    const wxPoint& GetStart() const         { return m_Start; }
    void SetStart( const wxPoint& aStart )  { m_Start = aStart; }
    void SetStartY( int y )                 { m_Start.y = y; }
    void SetStartX( int x )                 { m_Start.x = x; }

    /**
     * Function GetEnd
     * returns the ending point of the graphic
     */
    const wxPoint& GetEnd() const           { return m_End; }
    void SetEnd( const wxPoint& aEnd )      { m_End = aEnd; }
    void SetEndY( int y )                   { m_End.y = y; }
    void SetEndX( int x )                   { m_End.x = x; }

    // Some attributes are read only, since they are "calculated" from
    // m_Start, m_End, and m_Angle.
    // No Set...() function for these attributes.

    const wxPoint GetCenter() const override;
    const wxPoint& GetArcStart() const      { return m_End; }
    const wxPoint GetArcEnd() const;
    const wxPoint GetArcMid() const;

    /**
     * function GetArcAngleStart()
     * @return the angle of the starting point of this arc, between 0 and 3600 in 0.1 deg
     */
    double GetArcAngleStart() const;

    /**
     * Function GetRadius
     * returns the radius of this item
     * Has meaning only for arc and circle
     */
    int GetRadius() const
    {
        double radius = GetLineLength( m_Start, m_End );
        return KiROUND( radius );
    }

    /**
     * Initialize the start arc point. can be used for circles
     * to initialize one point of the cicumference
     */
    void SetArcStart( const wxPoint& aArcStartPoint )
    { m_End = aArcStartPoint; }

    /** For arcs and circles:
     */
    void SetCenter( const wxPoint& aCenterPoint ) { m_Start = aCenterPoint; }

    /**
     * Function GetParentModule
     * returns a pointer to the parent module, or NULL if DRAWSEGMENT does not
     * belong to a module.
     * @return MODULE* - pointer to the parent module or NULL.
     */
    MODULE* GetParentModule() const;

    // Accessors:
    const std::vector<wxPoint>& GetBezierPoints() const { return m_BezierPoints; }

    /** Build and return the list of corners in a std::vector<wxPoint>
     * It must be used only to convert the SHAPE_POLY_SET internal corner buffer
     * to a list of wxPoints, and nothing else, because it duplicates the buffer,
     * that is inefficient to know for instance the corner count
     */
    const std::vector<wxPoint> BuildPolyPointsList() const;

    /** @return the number of corners of the polygonal shape
     */
    int GetPointCount() const;

    // Accessors to the polygonal shape
    SHAPE_POLY_SET& GetPolyShape() { return m_Poly; }
    const SHAPE_POLY_SET& GetPolyShape() const { return m_Poly; }

    /**
     * @return true if the polygonal shape is valid (has more than 2 points)
     */
    bool IsPolyShapeValid() const;

    void SetPolyShape( const SHAPE_POLY_SET& aShape ) { m_Poly = aShape; }

    void SetBezierPoints( const std::vector<wxPoint>& aPoints )
    {
        m_BezierPoints = aPoints;
    }

    /** Rebuild the m_BezierPoints vertex list that approximate the Bezier curve
     * by a list of segments
     * Has meaning only for S_CURVE DRAW_SEGMENT shape
     * @param aMinSegLen is the min length of segments approximating the shape.
     * the last segment can be shorter
     * This param avoid having too many very short segment in list.
     * a good value is m_Width/2 to m_Width
     */
    void RebuildBezierToSegmentsPointsList( int aMinSegLen );

    void SetPolyPoints( const std::vector<wxPoint>& aPoints );

    void Print( PCB_BASE_FRAME* aFrame, wxDC* DC, const wxPoint& aOffset = ZeroOffset ) override;

    void GetMsgPanelInfo( EDA_UNITS_T aUnits, std::vector<MSG_PANEL_ITEM>& aList ) override;

    const EDA_RECT GetBoundingBox() const override;

    bool HitTest( const wxPoint& aPosition, int aAccuracy = 0 ) const override;
    bool HitTest( const EDA_RECT& aRect, bool aContained, int aAccuracy = 0 ) const override;

    wxString GetClass() const override
    {
        return wxT( "DRAWSEGMENT" );
    }

    /**
     * Function GetLength
     * returns the length of the track using the hypotenuse calculation.
     * @return double - the length of the track
     */
    double  GetLength() const;

    virtual void Move( const wxPoint& aMoveVector ) override;

    virtual void Rotate( const wxPoint& aRotCentre, double aAngle ) override;

    virtual void Flip( const wxPoint& aCentre ) override;

    /**
     * Function TransformShapeWithClearanceToPolygon
     * Convert the draw segment to a closed polygon
     * Used in filling zones calculations
     * Circles and arcs are approximated by segments
     * @param aCornerBuffer = a buffer to store the polygon
     * @param aClearanceValue = the clearance around the pad
     * @param aError = the maximum deviation from a true arc
     * @param ignoreLineWidth = used for edge cut items where the line width is only
     * for visualization
     */
    void TransformShapeWithClearanceToPolygon( SHAPE_POLY_SET& aCornerBuffer, int aClearanceValue,
            int aError = ARC_HIGH_DEF, bool ignoreLineWidth = false ) const override;

    virtual wxString GetSelectMenuText( EDA_UNITS_T aUnits ) const override;

    virtual BITMAP_DEF GetMenuImage() const override;

    virtual EDA_ITEM* Clone() const override;

    virtual const BOX2I ViewBBox() const override;

    virtual void SwapData( BOARD_ITEM* aImage ) override;

#if defined(DEBUG)
    void Show( int nestLevel, std::ostream& os ) const override { ShowDummy( os ); }
#endif
};

#endif  // CLASS_DRAWSEGMENT_H_

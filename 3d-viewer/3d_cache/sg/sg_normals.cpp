/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2015-2017 Cirilo Bernardo <cirilo.bernardo@gmail.com>
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

#include <iostream>
#include <sstream>
#include <wx/log.h>

#include "3d_cache/sg/sg_normals.h"
#include "3d_cache/sg/sg_helpers.h"


SGNORMALS::SGNORMALS( SGNODE* aParent ) : SGNODE( aParent )
{
    m_SGtype = S3D::SGTYPE_NORMALS;

    if( NULL != aParent && S3D::SGTYPE_FACESET != aParent->GetNodeType() )
    {
        m_Parent = NULL;

#ifdef DEBUG
        std::ostringstream ostr;
        ostr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
        ostr << " * [BUG] inappropriate parent to SGNORMALS (type ";
        ostr << aParent->GetNodeType() << ")";
        wxLogTrace( MASK_3D_SG, "%s\n", ostr.str().c_str() );
#endif
    }
    else if( NULL != aParent && S3D::SGTYPE_FACESET == aParent->GetNodeType() )
    {
        m_Parent->AddChildNode( this );
    }

    return;
}


SGNORMALS::~SGNORMALS()
{
    norms.clear();
    return;
}


bool SGNORMALS::SetParent( SGNODE* aParent, bool notify )
{
    if( NULL != m_Parent )
    {
        if( aParent == m_Parent )
            return true;

        // handle the change in parents
        if( notify )
            m_Parent->unlinkChildNode( this );

        m_Parent = NULL;

        if( NULL == aParent )
            return true;
    }

    // only a SGFACESET may be parent to a SGNORMALS
    if( NULL != aParent && S3D::SGTYPE_FACESET != aParent->GetNodeType() )
        return false;

    m_Parent = aParent;

    if( m_Parent )
        m_Parent->AddChildNode( this );

    return true;
}


SGNODE* SGNORMALS::FindNode(const char *aNodeName, const SGNODE *aCaller)
{
    if( NULL == aNodeName || 0 == aNodeName[0] )
        return NULL;

    if( m_Name == aNodeName )
        return this;

    return NULL;
}


void SGNORMALS::unlinkChildNode( const SGNODE* aCaller )
{
    #ifdef DEBUG
    std::ostringstream ostr;
    ostr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
    ostr << " * [BUG] unexpected code branch; node should have no children or refs";
    wxLogTrace( MASK_3D_SG, "%s\n", ostr.str().c_str() );
    #endif

    return;
}


void SGNORMALS::unlinkRefNode( const SGNODE* aCaller )
{
    #ifdef DEBUG
    std::ostringstream ostr;
    ostr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
    ostr << " * [BUG] unexpected code branch; node should have no children or refs";
    wxLogTrace( MASK_3D_SG, "%s\n", ostr.str().c_str() );
    #endif

    return;
}


bool SGNORMALS::AddRefNode( SGNODE* aNode )
{
    #ifdef DEBUG
    std::ostringstream ostr;
    ostr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
    ostr << " * [BUG] this node does not accept children or refs";
    wxLogTrace( MASK_3D_SG, "%s\n", ostr.str().c_str() );
    #endif

    return false;
}


bool SGNORMALS::AddChildNode( SGNODE* aNode )
{
    #ifdef DEBUG
    std::ostringstream ostr;
    ostr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
    ostr << " * [BUG] this node does not accept children or refs";
    wxLogTrace( MASK_3D_SG, "%s\n", ostr.str().c_str() );
    #endif

    return false;
}


bool SGNORMALS::GetNormalList( size_t& aListSize, SGVECTOR*& aNormalList )
{
    if( norms.empty() )
    {
        aListSize = 0;
        aNormalList = NULL;
        return false;
    }

    aListSize = norms.size();
    aNormalList = &norms[0];
    return true;
}


void SGNORMALS::SetNormalList( size_t aListSize, const SGVECTOR* aNormalList )
{
    norms.clear();

    if( 0 == aListSize || NULL == aNormalList )
        return;

    for( int i = 0; i < (int)aListSize; ++i )
        norms.push_back( aNormalList[i] );

    return;
}


void SGNORMALS::AddNormal( double aXValue, double aYValue, double aZValue )
{
    norms.push_back( SGVECTOR( aXValue, aYValue, aZValue ) );
    return;
}


void SGNORMALS::AddNormal( const SGVECTOR& aNormal )
{
    norms.push_back( aNormal );
    return;
}


void SGNORMALS::ReNameNodes( void )
{
    m_written = false;

    // rename this node
    m_Name.clear();
    GetName();
}


bool SGNORMALS::WriteVRML( std::ostream& aFile, bool aReuseFlag )
{
    if( norms.empty() )
        return false;

    if( aReuseFlag )
    {
        if( !m_written )
        {
            aFile << "  normal DEF " << GetName() << " Normal { vector [\n  ";
            m_written = true;
        }
        else
        {
            aFile << "  normal USE " << GetName() << "\n";
            return true;
        }
    }
    else
    {
        aFile << "  normal Normal { vector [\n  ";
    }

    std::string tmp;
    size_t n = norms.size();
    bool nline = false;

    for( size_t i = 0; i < n; )
    {
        S3D::FormatVector( tmp, norms[i] );
        aFile << tmp ;
        ++i;

        if( i < n )
        {
            aFile << ",";

            if( nline )
            {
                aFile << "\n  ";
                nline = false;
            }
            else
            {
                nline = true;
            }

        }
    }

    aFile << "] }\n";

    return true;
}


bool SGNORMALS::WriteCache( std::ostream& aFile, SGNODE* parentNode )
{
    if( NULL == parentNode )
    {
        if( NULL == m_Parent )
        {
            #ifdef DEBUG
            std::ostringstream ostr;
            ostr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
            ostr << " * [BUG] corrupt data; m_aParent is NULL";
            wxLogTrace( MASK_3D_SG, "%s\n", ostr.str().c_str() );
            #endif

            return false;
        }

        SGNODE* np = m_Parent;

        while( NULL != np->GetParent() )
            np = np->GetParent();

        if( np->WriteCache( aFile, NULL ) )
        {
            m_written = true;
            return true;
        }

        return false;
    }

    if( parentNode != m_Parent )
    {
        #ifdef DEBUG
        std::ostringstream ostr;
        ostr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
        ostr << " * [BUG] corrupt data; parentNode != m_aParent";
        wxLogTrace( MASK_3D_SG, "%s\n", ostr.str().c_str() );
        #endif

        return false;
    }

    if( !aFile.good() )
    {
        #ifdef DEBUG
        std::ostringstream ostr;
        ostr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
        ostr << " * [INFO] bad stream";
        wxLogTrace( MASK_3D_SG, "%s\n", ostr.str().c_str() );
        #endif

        return false;
    }

    aFile << "[" << GetName() << "]";
    size_t npts = norms.size();
    aFile.write( (char*)&npts, sizeof(size_t) );

    for( size_t i = 0; i < npts; ++i )
        S3D::WriteVector( aFile, norms[i] );

    if( aFile.fail() )
        return false;

    m_written = true;
    return true;
}


bool SGNORMALS::ReadCache( std::istream& aFile, SGNODE* parentNode )
{
    if( !norms.empty() )
    {
        #ifdef DEBUG
        std::ostringstream ostr;
        ostr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
        ostr << " * [BUG] non-empty node";
        wxLogTrace( MASK_3D_SG, "%s\n", ostr.str().c_str() );
        #endif

        return false;
    }

    size_t npts;
    aFile.read( (char*)&npts, sizeof(size_t) );
    SGVECTOR tmp;

    if( aFile.fail() )
        return false;

    for( size_t i = 0; i < npts; ++i )
    {
        if( !S3D::ReadVector( aFile, tmp ) || aFile.fail() )
            return false;

        norms.push_back( tmp );
    }

    return true;
}

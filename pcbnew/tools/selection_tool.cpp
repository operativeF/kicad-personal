/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2013 CERN
 * @author Tomasz Wlostowski <tomasz.wlostowski@cern.ch>
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

#include <boost/foreach.hpp>
#include <boost/optional.hpp>

#include <class_drawpanel_gal.h>
#include <class_board.h>
#include <class_board_item.h>
#include <class_module.h>

#include <wxPcbStruct.h>
#include <collectors.h>

#include <tool/context_menu.h>

#include "selection_tool.h"
#include "selection_area.h"

using namespace KiGfx;
using boost::optional;


SELECTION_TOOL::SELECTION_TOOL() :
	TOOL_INTERACTIVE( "pcbnew.InteractiveSelection" )
	{
		m_selArea = new SELECTION_AREA;

	}


SELECTION_TOOL::~SELECTION_TOOL()
{
	if(m_selArea)
		delete m_selArea;
}


void SELECTION_TOOL::Reset()
{
	// the tool launches upon reception of activate ("pcbnew.InteractiveSelection")
	Go(&SELECTION_TOOL::Main, TOOL_EVENT(TC_Command, TA_ActivateTool, GetName())); //"pcbnew.InteractiveSelection"));
}

int SELECTION_TOOL::Main(TOOL_EVENT& aEvent)
{
	
	// Main loop: keep receiving events
	while(OPT_TOOL_EVENT evt = Wait ())
	{

		if(evt->IsCancel ())
			return 0;

		// single click? Select single object
		if( evt->IsClick (MB_Left) )
			selectSingle(evt->Position(), evt->Modifier( MB_ModShift ));
		
		// drag with LMB? Select multiple objects (or at least draw a selection box)
		if (evt->IsDrag ( MB_Left ))
			selectMultiple();
	}

	return 0;
}

void SELECTION_TOOL::toggleSelection ( BOARD_ITEM * aItem, bool aAdditive )
{
	
	if(m_selectedItems.find(aItem) != m_selectedItems.end())
    {
    	aItem->ViewSetHighlighted(false);
    	m_selectedItems.erase(aItem);
    } else {
		if(!aAdditive)
			clearSelection();
    	aItem->ViewSetHighlighted(true);
    	m_selectedItems.insert(aItem);
    }
}

void SELECTION_TOOL::clearSelection ()
{
	BOOST_FOREACH(BOARD_ITEM *item, m_selectedItems)
    {
		item->ViewSetHighlighted(false);
	}

	m_selectedItems.clear();
}


void SELECTION_TOOL::selectSingle( const VECTOR2I &aWhere, bool aAdditive )
{
	BOARD *pcb = getModel<BOARD> (PCB_T);
	BOARD_ITEM *item;
    GENERAL_COLLECTORS_GUIDE guide = getEditFrame<PCB_EDIT_FRAME>()->GetCollectorsGuide();
    GENERAL_COLLECTOR collector;

    collector.Collect(pcb, GENERAL_COLLECTOR::AllBoardItems , wxPoint(aWhere.x, aWhere.y), guide);
	
    switch (collector.GetCount())
    {
    	case 0:
    		if(!aAdditive)
    			clearSelection();
			break;
    	case 1:
    		toggleSelection( collector[0], aAdditive );
    		break;
    	default:
			item = disambiguationMenu(&collector);
    		if(item)
    			toggleSelection(item, aAdditive );
    		break;
    }
}


BOARD_ITEM* SELECTION_TOOL::pickSmallestComponent( GENERAL_COLLECTOR* aCollector )
{
    int count = aCollector->GetPrimaryCount();     // try to use preferred layer
    if( 0 == count ) count = aCollector->GetCount();

    for( int i = 0; i<count;  ++i )
    {
        if( (*aCollector)[i]->Type() != PCB_MODULE_T )
            return NULL;
    }

    // all are modules, now find smallest MODULE

    int minDim = 0x7FFFFFFF;
    int minNdx = 0;

    for( int i = 0;  i<count;  ++i )
    {
        MODULE* module = (MODULE*) (*aCollector)[i];

        int     lx = module->GetBoundingBox().GetWidth();
        int     ly = module->GetBoundingBox().GetHeight();

        int     lmin = std::min( lx, ly );

        if( lmin < minDim )
        {
            minDim = lmin;
            minNdx = i;
        }
    }

    return (*aCollector)[minNdx];
}

void SELECTION_TOOL::handleHighlight( const VECTOR2D& aP )
{

}

void SELECTION_TOOL::selectMultiple()
{
	OPT_TOOL_EVENT evt;
	VIEW *v = getView();

	v->Add(m_selArea);

	while (evt = Wait())
	{
		if(evt->IsCancel())
			break;

		if(evt->IsDrag( MB_Left ))
		{
			m_selArea->SetOrigin( evt->DragOrigin() );
			m_selArea->SetEnd( evt->Position() );
			m_selArea->ViewSetVisible(true);
			m_selArea->ViewUpdate(VIEW_ITEM::APPEARANCE | VIEW_ITEM::GEOMETRY);
			
			
			v->SetLayerVisible( SELECTION_AREA::SelectionLayer );
			v->SetLayerOrder( SELECTION_AREA::SelectionLayer, 1000);
			v->SetLayerTarget( SELECTION_AREA::SelectionLayer, TARGET_OVERLAY );
		}

		if(evt->IsMouseUp( MB_Left ))
		{
			m_selArea->ViewSetVisible(false);
			break;
		}
	}


	v->Remove(m_selArea);
}


BOARD_ITEM *SELECTION_TOOL::disambiguationMenu ( GENERAL_COLLECTOR *aCollector )
{
	CONTEXT_MENU cmenu;
    OPT_TOOL_EVENT evt ;
	BOARD_ITEM *current = NULL;

	cmenu.SetTitle( _("Clarify selection"));

	int limit = std::min( 10, aCollector->GetCount() );

	for( int i = 0;  i<limit;  ++i )
	{
	    wxString    text;
		BOARD_ITEM *item = (*aCollector) [i];
		text = item->GetSelectMenuText();
		cmenu.Add(text, i);
	}
				
	SetContextMenu(&cmenu, CMENU_NOW);

	while (evt = Wait())
	{
		
	   			
	   	if(evt->Action() == TA_ContextMenuUpdate )
	   	{
	   		if(current)
	   			current->ViewSetHighlighted(false);
	   		
	   		int id = *evt->GetCommandId();
	   		if(id >= 0)
	   		{
	   			current = (*aCollector) [id];
	   			current->ViewSetHighlighted(true);
	   		} else
	   			current = NULL;

	   		
    	} else if(evt->Action() == TA_ContextMenuChoice ) {

			optional<int> id = evt->GetCommandId();

			if(current)
				current->ViewSetHighlighted(false);

			if(id && (*id >= 0))
			{
				current = (*aCollector) [*id];
				current->ViewSetHighlighted(true);
				return current;
			}
			return NULL;
		} 

		
	}

	return NULL;
}
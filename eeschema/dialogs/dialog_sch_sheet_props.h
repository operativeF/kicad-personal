/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2009 Wayne Stambaugh <stambaughw@verizon.net>
 * Copyright (C) 2014 KiCad Developers, see CHANGELOG.TXT for contributors.
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

#ifndef __dialog_sch_sheet_props__
#define __dialog_sch_sheet_props__

#include <dialog_sch_sheet_props_base.h>
#include <widgets/unit_binder.h>


class SCH_SHEET;
class SCH_EDIT_FRAME;


/** Implementing DIALOG_SCH_SHEET_PROPS_BASE */
class DIALOG_SCH_SHEET_PROPS : public DIALOG_SCH_SHEET_PROPS_BASE
{
    SCH_SHEET*   m_sheet;

    UNIT_BINDER  m_filenameTextSize;
    UNIT_BINDER  m_sheetnameTextSize;

public:
    /** Constructor */
    DIALOG_SCH_SHEET_PROPS( SCH_EDIT_FRAME* parent, SCH_SHEET* aSheet );

    bool TransferDataToWindow() override;
    bool TransferDataFromWindow() override;

    wxString GetFileName();
    wxString GetSheetName() { return m_textSheetName->GetValue(); }
    int GetFileNameTextSize() { return m_filenameTextSize.GetValue(); }
    int GetSheetNameTextSize() { return m_sheetnameTextSize.GetValue(); }
};

#endif // __dialog_sch_sheet_props__

/*
   Underworld Adventures Debugger - a debugger tool for Underworld Adventures
   Copyright (c) 2004,2005 Michael Fink

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   $Id$

*/
/*! \file HotspotList.cpp

   \brief hotspot list

*/

// includes
#include "stdatl.hpp"
#include "HotspotList.hpp"
#include "DebugClient.hpp"

// data

struct CHotspotItems
{
   const char* name;
   unsigned int level;
   double xpos, ypos;
} g_aHotspotItems[] =
{
   { "Level 1: Entrance of the Abyss", 0, 32.0, 2.0 },
   { "Level 1: Bragit (7)", 0, 17.0, 6.0 },
   { "Level 1: Human Enclave", 0, 22.5, 15.5 },
   { "Level 1: Spider Lair", 0, 26.5, 25.5 },
   { "Level 1: Hostile Green Goblins", 0, 5.5, 38.5 },
   { "Level 1: Green Goblin Lair", 0, 13.5, 53.5 },
   { "Level 1: Gray Goblin Lair", 0, 54.5, 55.5 },
   { "Level 1: Lever Tile Puzzle (22)", 0, 47.5, 46.5 },
   { "Level 1: 's Grave (near 22)", 0, 49.5, 41.5 },
   { "Level 1: Shrine (17)", 0, 46.5, 15.5 },

   { "Level 2: Brawnclan (3)", 1, 13.5, 28.5 },
   { "Level 2: Hall of the Mountain King", 1, 13.5, 8.5 },
   { "Level 2: Shak", 1, 60.5, 8.5 },
   { "Level 2: Evil Gray Goblins", 1, 45.5, 58.5 },

   { "Level 3: Sseetharee (2)", 2, 24.5, 22.5 },
   { "Level 3: Gray Lizardmen", 2, 11.5, 10.5 },
   { "Level 3: Starting Place (stairs)", 2, 5.5, 1.5 },
   { "Level 3: Lizardman Lair", 2, 37.5, 30.5 },

   { "Level 4: Sethar Strongarm (11)", 3, 50.5, 22.5 },
   { "Level 4: Rodrick's Hall", 3, 32.5, 52.5 },
   { "Level 4: Knight's Home", 3, 8.5, 29.5 },

   { "Level 5: Shanclick and Eyesnack (1)", 4, 52.5, 23.5 },
   { "Level 5: On a box", 4, 37.5, 12.5 },
   { "Level 5: Pac-Man maze", 4, 8.5, 2.5 },
   { "Level 5: Level stairs", 4, 34.5, 47.5 },

   { "Level 6: Wine of Compassion (16)", 5, 26.5, 51.5 },
   { "Level 6: Golem Test", 5, 6.5, 24.5 },
   { "Level 6: Seer's Storeroom", 5, 3.5, 11.5 },
   { "Level 6: Level stairs", 5, 57.5, 31.5 },

   { "Level 7: Crystal Splinter passage", 6, 20.5, 44.5 },
   { "Level 7: Prison?", 6, 6.5, 30.5 },
   { "Level 7: Cell", 6, 26.5, 56.5 },

   { "Level 8: Before the Chamber of Virtue", 7, 32.0, 25.5 },
   { "Level 8: Monster pit?", 7, 23.0, 49.5 },

   { "Ethereal Void: Starting point", 8, 29.5, 24.5 },
   { "Ethereal Void: Green Pathway End", 8, 52.5, 48.5 },
   { "Ethereal Void: Blue Pathway End", 8, 10.5, 16.5 },
   { "Ethereal Void: Red Pathway End", 8, 7.5, 51.5 },

   { NULL, 0, 0.0, 0.0 } // end of list marker
};


// CHotspotListWindow methods

LRESULT CHotspotListWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   CRect rcDef;
   GetClientRect(rcDef);
   m_listCtrl.Create(m_hWnd, rcDef, NULL,
      WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_NOSORTHEADER );

   m_listCtrl.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

   m_listCtrl.InsertColumn(0, (LPCTSTR)_T("Hotspot"), LVCFMT_LEFT, 400, -1);
   m_listCtrl.InsertColumn(1, (LPCTSTR)_T("Location"), LVCFMT_LEFT, 200, -1);

   // insert all items
   unsigned int i=0;
   while(g_aHotspotItems[i].name != NULL)
   {
      long pos = m_listCtrl.InsertItem(i,g_aHotspotItems[i].name);

      _TCHAR szBuffer[256];
      _sntprintf(szBuffer, 256, _T("Level %u, Pos %2.1f/%2.1f"),
         g_aHotspotItems[i].level,
         g_aHotspotItems[i].xpos, g_aHotspotItems[i].ypos);

      m_listCtrl.SetItemText(i, 1, szBuffer);
      i++;
   }

   return 0;
}

LRESULT CHotspotListWindow::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
   if(wParam != SIZE_MINIMIZED )
   {
      RECT rc;
      GetClientRect(&rc);
      m_listCtrl.SetWindowPos(NULL, &rc ,SWP_NOZORDER | SWP_NOACTIVATE );
   }      
   bHandled = FALSE;
   return 1;
}

LRESULT CHotspotListWindow::OnSetFocus(UINT, WPARAM, LPARAM, BOOL& bHandled)
{
   if (m_listCtrl.m_hWnd != NULL && m_listCtrl.IsWindowVisible())
      m_listCtrl.SetFocus();

   bHandled = FALSE;
   return 1;
}

LRESULT CHotspotListWindow::OnDblClick(WPARAM /*wParam*/, NMHDR* pNMHDR, BOOL& /*bHandled*/)
{
   LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)pNMHDR;
   int nItem = lpnmitem->iItem;
   if (nItem == -1)
      return 0;

   m_pDebugClient->Lock(true);

   m_pDebugClient->GetPlayerInterface().Teleport(g_aHotspotItems[nItem].level,
      g_aHotspotItems[nItem].xpos, g_aHotspotItems[nItem].ypos);

   m_pDebugClient->Lock(false);
   return 0;
}

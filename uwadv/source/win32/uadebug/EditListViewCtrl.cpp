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
/*! \file EditListViewCtrl.cpp

   \brief editable list view control

*/

// includes
#include "stdatl.hpp"
#include "EditListViewCtrl.hpp"

// CEditListInplaceEditCtrl methods

bool CEditListInplaceEditCtrl::AcceptChanges()
{
   _TCHAR szBuffer[256];
   GetWindowText(szBuffer, 256);

   NMLVDISPINFO dispinfo;
   dispinfo.hdr.hwndFrom = m_hWnd;
   dispinfo.hdr.idFrom = 0;
   dispinfo.hdr.code = LVN_ENDLABELEDIT;

   dispinfo.item.mask = LVIF_TEXT;
   dispinfo.item.iItem = m_nItem;
   dispinfo.item.iSubItem = m_nColumn;
   dispinfo.item.pszText = szBuffer;

   HWND hWnd = ::GetParent(GetParent());
   int nRet = ::SendMessage(hWnd, WM_NOTIFY, 0, (LPARAM)&dispinfo);

   if (nRet == 1)
      ListView_SetItemText(GetParent(), m_nItem, m_nColumn, szBuffer);

   return nRet != 0;
}

void CEditListInplaceEditCtrl::Finish()
{
   if (!m_bFinished)
   {
      m_bFinished = true;
      ::SetFocus(GetParent());
      DestroyWindow();
   }
}

LRESULT CEditListInplaceEditCtrl::OnChar(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
   switch(wParam)
   {
   case VK_RETURN:
      if (!AcceptChanges())
      {
         bHandled = true;
         break;
      }

   case VK_ESCAPE:
      Finish();
      break;

   default:
      bHandled = false;
      break;
   }

   return 0;
}

LRESULT CEditListInplaceEditCtrl::OnKillFocus(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
   if (!m_bFinished)
   {
      AcceptChanges();
      Finish();
   }
   return 0;
}

LRESULT CEditListInplaceEditCtrl::OnNcDestroy(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
   ::PostMessage(GetParent(), WM_DELETEME, 0, reinterpret_cast<LPARAM>(this));
   m_hWnd = NULL;
   return 0;
}


// CEditListViewCtrl methods

LRESULT CEditListViewCtrl::OnLeftButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   SetFocus();

   POINT pt;
   pt.x = GET_X_LPARAM(lParam);
   pt.y = GET_Y_LPARAM(lParam);

   UINT flags = 0;
   int item = HitTest(pt, &flags);

   if ((flags & LVHT_ONITEMLABEL) == 0)
      return 0;

   unsigned int xpos = 0;

   int xlparam = GET_X_LPARAM(lParam);
   int column = 0;

   while(xpos < (unsigned)xlparam)
      xpos += GetColumnWidth(column++);
   column--;

   NMLVDISPINFO dispInfo;
   dispInfo.hdr.hwndFrom = m_hWnd;
   dispInfo.hdr.idFrom = -1;
   dispInfo.hdr.code = LVN_BEGINLABELEDIT;
   dispInfo.item.mask = 0;
   dispInfo.item.iItem = item;
   dispInfo.item.iSubItem = column;

   HWND hWnd = GetParent();
   int nRet = SendMessage(hWnd, WM_NOTIFY, 0, (LPARAM)&dispInfo);

   if (nRet != 0)
      return 0;

   // create edit-control
   CEditListInplaceEditCtrl* pEdit = new CEditListInplaceEditCtrl(item,column);

   RECT rect;
   GetItemRect(item, &rect, LVIR_LABEL);

   unsigned int startx = 0;
   for(int n=0; n<column; n++)
      startx += GetColumnWidth(n);

   rect.left = column == 0 ? rect.left : startx+3;
   rect.right = startx + GetColumnWidth(column);
//   rect.top;
   rect.bottom++;

   _TCHAR szBuffer[256];
   GetItemText(item,column,szBuffer,256);

   pEdit->Create(m_hWnd, rect, _T(""));
   pEdit->SetWindowText(szBuffer);

   pEdit->SetFocus();
   pEdit->SetSel(0,-1);

   HFONT hFont = GetFont();
   pEdit->SetFont(hFont);

   return 0;
}

LRESULT CEditListViewCtrl::OnDeleteMe(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
   CEditListInplaceEditCtrl* pEdit = reinterpret_cast<CEditListInplaceEditCtrl*>(lParam);
   delete pEdit;
   return 0;
}
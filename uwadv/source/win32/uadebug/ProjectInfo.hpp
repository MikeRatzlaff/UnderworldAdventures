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
/*! \file ProjectInfo.hpp

   \brief project info docking window

*/
//! \ingroup uadebug

//@{

// include guard
#pragma once

// includes
#include "WindowBase.hpp"

// classes

//! player info docking window
class CProjectInfoWindow : public CDockingWindowBase
{
   typedef CProjectInfoWindow thisClass;
   typedef CDockingWindowBase baseClass;
public:
   //! ctor
   CProjectInfoWindow():baseClass(idProjectInfoWindow){}

   //! updates project info data
   void UpdateData();

   DECLARE_WND_CLASS(_T("CProjectInfoWindow"))
   BEGIN_MSG_MAP(thisClass)   
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
      MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
      CHAIN_MSG_MAP(baseClass)      
   END_MSG_MAP()

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
   LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);

protected:
   //! project info tree
   CTreeViewCtrl m_treeCtrl;

   //! tree image list
   CImageList m_ilIcons;
};

//@}

/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Underworld Adventures Team

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
/*! \file objectlist.hpp

   \brief master object list frame

*/

// include guard
#ifndef uadebug_objectlist_hpp_
#define uadebug_objectlist_hpp_

// needed includes


// classes

class ua_objectlist_frame: public wxMDIChildFrame
{
public:
   //! ctor
   ua_objectlist_frame(/*wxDocManager* doc_manager, */
      wxMDIParentFrame* parent, wxWindowID id,
      const wxPoint& pos, const wxSize& size, long style);

   //! updates data from underworld
   void UpdateData();

   //! frame name for "object list" frame
   static const char* frame_name;

protected:
};

#endif
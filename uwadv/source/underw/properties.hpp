/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004 Underworld Adventures Team

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
/*! \file properties.hpp

   \brief object property manager

*/
//! \ingroup underworld

//@{

// include guard
#ifndef uwadv_properties_hpp_
#define uwadv_properties_hpp_

// needed includes
#include <vector>
#include "settings.hpp"


// structs

//! object properties common to all objects
struct ua_common_obj_property
{
   Uint16 radius; //!< object radius
   Uint8 height; //!< object height

   Uint16 mass; //!< mass of object in 0.1 kg

   Uint8 quality_class;
   Uint8 quality_type;

   //! indicates if object can have owner ("belonging to ...")
   bool can_have_owner;

   //! indicates if object can be looked at
   bool can_be_looked_at;

   //! indicates if object can be picked up
   bool can_be_picked_up;
};

//! info for melee weapons (ids 0x0000-0x000f)
struct ua_melee_weapon_property
{
   //! damage modifier for attack types
   Uint8 dm_slash,dm_bash,dm_stab;

   //! skill type needed for melee weapon
   Uint8 skill_type;

   //! weapon durability
   Uint8 durability;
};


// classes

//! object properties class
class ua_object_properties
{
public:
   //! ctor
   ua_object_properties(){}

   //! imports object properties from current uw path
   void import(ua_settings& settings);

   //! returns common object properties about specific object
   inline ua_common_obj_property& get_common_property(Uint16 item_id);

protected:
   std::vector<ua_common_obj_property> common_properties;

   friend class ua_uw_import;
};


// inline methods

inline ua_common_obj_property& ua_object_properties::get_common_property(
   Uint16 item_id)
{
   if (item_id >= common_properties.size())
      item_id = common_properties.size()-1;

   return common_properties[item_id];
}


#endif
//@}

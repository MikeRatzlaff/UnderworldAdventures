/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003 Underworld Adventures Team

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
/*! \file uwaccess.cpp

   \brief underworld access API implementation

*/

// needed includes
#include "common.hpp"
#include "uwaccess.hpp"
#include "core.hpp"


// static variables

//! current access api object
ua_uw_access_api* ua_uw_access_api::cur_api = NULL;


// ua_uw_access_api methods

void ua_uw_access_api::init(ua_game_core_interface* thecore, ua_debug_interface* dbgint)
{
   core = thecore;
   debug = dbgint;
   cur_api = this;
}

unsigned int ua_uw_access_api::command_func(
   unsigned int cmd, unsigned int type,
   ua_debug_param* param1, ua_debug_param* param2)
{
   unsigned int ret = 0;

   ua_game_core_interface* core = cur_api->core;
   ua_debug_interface* debug = cur_api->debug;
   ua_underworld& underw = core->get_underworld();

   switch(cmd)
   {
   case udc_nop: // do noting
      ret = 42;
      break;

   case udc_lock: // lock underworld
      debug->lock();
      break;

   case udc_unlock: // unlock underworld
      debug->unlock();
      break;

   case udc_player_get: // get player value
      {
         ua_player& pl = underw.get_player();

         unsigned int value = param1->val.i;

         if (value<4)
         {
            switch(value)
            {
            case 0: param1->val.d = pl.get_xpos(); break;
            case 1: param1->val.d = pl.get_ypos(); break;
            case 2: param1->val.d = pl.get_height(); break;
            case 3: param1->val.d = pl.get_angle_rot(); break;
            }

            param1->type = ua_param_double;
         }
         else
         {
            value -= 4;
            param1->type = ua_param_int;

            if (value<ua_attr_max)
               param1->val.i = pl.get_attr((ua_player_attributes)value);
            else
               param1->val.i = pl.get_skill((ua_player_skills)(value-ua_attr_max));
         }
      }
      break;

   case udc_player_set: // set player value
      {
         ua_player& pl = underw.get_player();

         unsigned int value = param1->val.i;

         if (value<4)
         {
            ua_level& level = underw.get_current_level();

            switch(value)
            {
            case 0: // xpos
               pl.set_pos(param2->val.d,pl.get_ypos());
               pl.set_height(level.get_floor_height(param2->val.d,pl.get_ypos()));
               break;
            case 1: // ypos
               pl.set_pos(pl.get_xpos(),param2->val.d);
               pl.set_height(level.get_floor_height(param2->val.d,pl.get_ypos()));
               break;
            case 2: pl.set_height(param2->val.d); break;
            case 3: pl.set_angle_rot(param2->val.d); break;
            }
         }
         else
         {
            value -= 4;

            if (value<ua_attr_max)
            {
               // setting attribute
               pl.set_attr((ua_player_attributes)value,param2->val.i);

               if (value==ua_attr_maplevel)
                  underw.change_level(param2->val.i);
            }
            else
            {
               value -= ua_attr_max;
               pl.set_skill((ua_player_skills)value,param2->val.i);
            }
         }
      }
      break;

   case udc_objlist_get: // get master object list value
      {
         // get object info
         ua_level& level = type > underw.get_num_levels() ? underw.get_current_level() :
            underw.get_level(type);

         ua_object& obj = level.get_mapobjects().get_object(static_cast<Uint16>(param1->val.i));
         ua_object_info& objinfo = obj.get_object_info();

         switch(param2->val.i)
         {
         case 0: // item_id
            param1->set(static_cast<unsigned int>(objinfo.item_id));
            break;

         case 1:
            param1->set(static_cast<unsigned int>(objinfo.link));
         }
      }
      break;

   case udc_objlist_set: // set master object list value
      {
      }
      break;

   case udc_strings_get: // returns string from given string block
      {
         std::string text;
         text = underw.get_strings().get_string(param1->val.i,param2->val.i);
         param1->set(text.c_str());
      }
      break;

   default:
      ret = 1;
   }

   return ret;
}

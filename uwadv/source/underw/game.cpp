//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2006,2019 Michael Fink
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
/// \file game.cpp
/// \brief game object
//
#include "underw.hpp"
#include "game.hpp"

//#include "uamath.hpp"
#include "scripting.hpp"
//#include "geometry.hpp"

using Underworld::Game;

Game::Game()
:callback(NULL), scripting(NULL)
{
}

#if 0
void Game::Init(Base::Settings& settings, ResourceManager& filesmgr)
{
   enhanced_features = settings.get_bool(ua_setting_uwadv_features);

   last_evaltime = -1.0;

   stopped = false;
   attacking = false;
   attack_power = 0;

   physics.add_track_body(&player);
}
#endif

void Game::EvaluateUnderworld(double time)
{
   if (stopped)
      return;

   double elapsed = last_evaltime > 0 ? time - last_evaltime : 0.1;

   if (attacking)
   {
      attack_power += 5;
      if (attack_power > 100)
         attack_power = 100;

      if (callback != NULL)
         callback->uw_notify(ua_notify_update_powergem);
   }

   // evaluate physics
// TODO   physics.eval_physics(elapsed);

   m_underworld.GetPlayer().rotate_move(elapsed);

   check_move_trigger();

   last_evaltime = time;
}

void Game::user_action(EGameUserAction action,
   unsigned int param)
{
   if (m_scripting == NULL) return;

   UaTrace("user action: action=%u param=%u\n", action,param);

   switch(action)
   {
   case ua_action_combat_draw:
      attacking = true;
      attack_power = 0;
      // additionally tell scripting with type of attack
      m_scripting->user_action(action, param);
      break;

   case ua_action_combat_release:
      // do the actual attack
      m_scripting->user_action(action, attack_power);
      UaTrace("attacking with power=%u\n", attack_power);

      // switch off attacking
      attacking = false;
      attack_power = 0;

      if (callback != NULL)
         callback->uw_notify(ua_notify_update_powergem);
      break;

   default:
      // pass on to scripting
      m_scripting->user_action(action, param);
      break;
   }
}

Underworld::Level& Game::get_current_level()
{
   unsigned int curlevel = m_underworld.GetPlayer().GetAttribute(Underworld::attr_maplevel);
   return m_underworld.GetLevelMaps().get_level(curlevel);
}

const Underworld::Level &Game::get_current_level() const
{
   unsigned int curlevel = m_underworld.GetPlayer().GetAttribute(ua_attr_maplevel);
   return levelmaps.get_level(curlevel);
}

void Game::ChangeLevel(unsigned int level)
{
   // check if game wants to change to unknown level
   UaAssert(level<levelmaps.get_num_levels());

   // set new level
   m_underworld.GetPlayer().set_attr(ua_attr_maplevel,level);

   // clear activated move triggers
   trigger_active.clear();

   // tell callback and scripting about change
   if (callback != NULL)
      callback->uw_notify(ua_notify_level_change);

   if (scripting != 0)
      m_scripting->on_changing_level();
}

void Game::check_move_trigger()
{
   Vector3d pl_pos(m_underworld.GetPlayer().get_xpos(), m_underworld.GetPlayer().get_ypos(), m_underworld.GetPlayer().get_height());

   unsigned int tilex, tiley;
   tilex = static_cast<unsigned int>(pl_pos.x);
   tiley = static_cast<unsigned int>(pl_pos.y);

   // check all surrounding tiles
   for(int i=-1; i<2; i++)
   for(int j=-1; j<2; j++)
   {
      int x = static_cast<int>(tilex) + i;
      int y = static_cast<int>(tiley) + j;
      if (x<0 || y<0 || x>64-1 || y>64-1) continue;

      ua_object_list& objlist = get_current_level().get_mapobjects();

      // check tile objects for move trigger
      Uint16 pos =
         objlist.get_tile_list_start(
            static_cast<unsigned int>(x),static_cast<unsigned int>(y));

      while (pos != 0)
      {
         // check if move trigger
         Object& obj = objlist.get_object(pos);
         if (obj.get_object_info().item_id == 0x01a0)
         {
            // found move trigger; check if it's in range
            NpcInfo& extinfo = obj.get_ext_object_info();
            ua_vector3d trig_pos(
               static_cast<double>(x)+(extinfo.xpos+0.5)/8.0,
               static_cast<double>(y)+(extinfo.ypos+0.5)/8.0,
               extinfo.zpos);

            trig_pos -= pl_pos;

            if (trig_pos.length()<0.5)
            {
               // trigger in range

               // check if trigger already active
               if (trigger_active.find(pos) == trigger_active.end())
               {
                  // not active yet
                  trigger_active.insert(pos);

                  UaTrace("move trigger: activate trigger at %04x\n",pos);

                  unsigned int curlevel = player.GetAttribute(ua_attr_maplevel);
                  if (scripting != NULL)
                     m_scripting->trigger_set_off(pos);
               }
               else
               {
                  // trigger is active; do nothing
               }
            }
            else
            {
               // not in range; check if we can deactivate it
               if (trigger_active.find(pos) != trigger_active.end())
               {
                  UaTrace("move trigger: deactivate trigger at %04x\n",pos);
                  trigger_active.erase(pos);
               }
            }
         }

         // next object in chain
         pos = obj.get_object_info().link;
      }
   }
}

/*! \todo also collect triangles from 3d models and critter objects */
void Game::get_surrounding_triangles(unsigned int xpos,
   unsigned int ypos, std::vector<ua_triangle3d_textured>& alltriangles)
{
   unsigned int xmin, xmax, ymin, ymax;

   xmin = xpos>0 ? xpos-1 : 0;
   xmax = xpos<64 ? xpos+1 : 64;
   ymin = ypos>0 ? ypos-1 : 0;
   ymax = ypos<64 ? ypos+1 : 64;

   // collect all triangles
   ua_geometry_provider prov(get_current_level());

   for(unsigned int x=xmin; x<xmax; x++)
   for(unsigned int y=ymin; y<ymax; y++)
      prov.get_tile_triangles(x,y,alltriangles);

   // also collect triangles from 3d models and critter objects
   if (callback != NULL)
   {
      const ua_object_list& objlist = get_current_level().get_mapobjects();
      for(unsigned int x=xmin; x<xmax; x++)
      for(unsigned int y=ymin; y<ymax; y++)
      {
         // get first object link
         Uint16 link = objlist.get_tile_list_start(x,y);
         while(link != 0)
         {
            // collect triangles
            callback->uw_get_object_triangles(x,y,objlist.get_object(link),
               alltriangles);

            // next object in link chain
            link = objlist.get_object(link).get_object_info().link;
         }
      }
   }
}
#endif
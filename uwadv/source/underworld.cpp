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
/*! \file underworld.cpp

   \brief underworld object

*/

// needed includes
#include "common.hpp"
#include "underworld.hpp"
#include "core.hpp"
#include "uamath.hpp"


// ua_underworld methods

void ua_underworld::init(ua_settings& settings, ua_files_manager& filesmgr)
{
   enhanced_features = settings.get_bool(ua_setting_uwadv_features);

   stopped = false;

   // init underworld members
   levels.clear();

   physics.init(this);

   inventory.init(this);

   player.init();

   questflags.resize(0x0040,0);

   // load game strings
   ua_trace("loading game strings ... ");
   gstr.load(settings);
   ua_trace("done\n");
}

void ua_underworld::done()
{
   script.done();
}

void ua_underworld::eval_underworld(double time)
{
   if (stopped)
      return;

   // evaluate physics
   physics.eval_physics(time);

   // call Lua tick script
   script.lua_game_tick(time);
}

ua_level &ua_underworld::get_current_level()
{
   unsigned int curlevel = player.get_attr(ua_attr_maplevel);
   return levels[curlevel];
}

void ua_underworld::change_level(unsigned int level)
{
   if (level>levels.size())
      throw ua_exception("game wanted to change to unknown level");

   // set new level
   player.set_attr(ua_attr_maplevel,level);

   script.lua_change_level(level);
}

void ua_underworld::load_game(ua_savegame &sg)
{
   // load all levels
   {
      levels.clear();

      sg.begin_section("tilemaps");

      Uint32 max = sg.read32();

      for(Uint32 i=0; i<max; i++)
      {
         ua_level newlevel;

         // load tilemap / objects list / annotations
         newlevel.load_game(sg);

         levels.push_back(newlevel);
      }

      sg.end_section();
   }

   // load map annotations

   // load objects list

   // load inventory
   inventory.load_game(sg);

   // save player infos
   player.load_game(sg);

   // conv. globals
   conv_globals.load_game(sg);

   // Lua script values
   script.load_game(sg);

   sg.close();

   // reload level
   change_level(player.get_attr(ua_attr_maplevel));
}

void ua_underworld::save_game(ua_savegame &sg)
{
   // save all levels
   {
      sg.begin_section("tilemaps");

      unsigned int max = levels.size();
      sg.write32(max);

      for(unsigned int i=0; i<max; i++)
      {
         // save tilemap / objects list / annotations
         levels[i].save_game(sg);
      }

      sg.end_section();
   }

   // save inventory
   inventory.save_game(sg);

   // save player infos
   player.save_game(sg);

   // conv. globals
   conv_globals.save_game(sg);

   // Lua script values
   script.save_game(sg);

   sg.close();
}

extern void ua_import_levelmaps(ua_settings &settings, const char *folder,
   std::vector<ua_level> &levels);

void ua_underworld::import_savegame(ua_settings &settings,const char *folder,bool initial)
{
   // load level maps
   ua_import_levelmaps(settings,folder,levels);

   // load conv globals
   {
      std::string bgname(settings.get_string(ua_setting_uw_path));
      bgname.append(folder);
      bgname.append(initial ? "babglobs.dat" : "bglobals.dat");
      conv_globals.import(bgname.c_str(),initial);
   }

   // load player infos
   if (!initial)
      player.import_player(settings,folder);

   // reload level
   change_level(player.get_attr(ua_attr_maplevel));
}


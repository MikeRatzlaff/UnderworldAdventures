/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Michael Fink

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
/*! \file player.cpp

   \brief player object

*/

// needed includes
#include "common.hpp"
#include "player.hpp"


// ua_player methods

ua_player::ua_player()
{
}

void ua_player::init()
{
   xpos = ypos = 32.0;
   height = 24.0; // start height at entrance
   rotangle = panangle = 0.0;
   move_mode = 0;

   memset(attributes,0,sizeof(attributes));
   memset(skills,0,sizeof(skills));

   move_factors[ua_move_walk] = 0.0;
   move_factors[ua_move_rotate] = 0.0;
   move_factors[ua_move_lookup] = 0.0;
   move_factors[ua_move_jump] = 0.0;
   move_factors[ua_move_slide] = 0.0;
   move_factors[ua_move_float] = 0.0;
}

void ua_player::set_movement_mode(unsigned int set,unsigned int del)
{
   move_mode = (move_mode | set) & (~del);
}

void ua_player::set_attr(ua_player_attributes which, unsigned int value)
{
   if (which<ua_attr_max)
      attributes[static_cast<unsigned int>(which)]=value;
}

void ua_player::set_skill(ua_player_skills which, unsigned int value)
{
   if (which<ua_skill_max)
      skills[static_cast<unsigned int>(which)]=value;
}

void ua_player::set_name(std::string name)
{
    this->name = name;
}

unsigned int ua_player::get_attr(ua_player_attributes which)
{
   if (which>=ua_attr_max) return 0;
   return attributes[static_cast<unsigned int>(which)];
}

unsigned int ua_player::get_skill(ua_player_skills which)
{
   if (which>=ua_skill_max) return 0;
   return skills[static_cast<unsigned int>(which)];
}

void ua_player::load_game(ua_savegame &sg)
{
   sg.begin_section("player");

   unsigned int n=0;

   // read in name
   sg.read_string(name);

   // read position
   xpos = sg.read32()/256.0;
   ypos = sg.read32()/256.0;
   rotangle = sg.read32()/256.0;
   panangle = sg.read32()/256.0;
   height = sg.read32()/256.0;

   // read attributes and skills
   for(n=0; n<SDL_TABLESIZE(attributes); n++)
      attributes[n] = sg.read16();

   for(n=0; n<SDL_TABLESIZE(skills); n++)
      skills[n] = sg.read16();

   sg.end_section();
}

void ua_player::save_game(ua_savegame &sg)
{
   sg.begin_section("player");

   unsigned int n=0;

   // write name
   sg.write_string(name.c_str());

   // write position
   sg.write32(Uint32(xpos*256.0));
   sg.write32(Uint32(ypos*256.0));
   sg.write32(Uint32(rotangle*256.0));
   sg.write32(Uint32(panangle*256.0));
   sg.write32(Uint32(height*256.0));

   // write attributes and skills
   for(n=0; n<SDL_TABLESIZE(attributes); n++)
      sg.write16(attributes[n]);

   for(n=0; n<SDL_TABLESIZE(skills); n++)
      sg.write16(skills[n]);

   sg.end_section();
}

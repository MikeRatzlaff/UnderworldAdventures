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
/*! \file files.cpp

   \brief resource files management

   functions to create a image from a string, in a specific font. fonts are

*/

// needed includes
#include "common.hpp"
#include "files.hpp"
#include "resource/zziplib/SDL_rwops_zzip.h"
#include <iostream>


// ua_files_manager methods

ua_files_manager::ua_files_manager()
{
}

void ua_files_manager::init(ua_settings &settings)
{
   init_cfgfiles_list();

   uadata_path = settings.uadata_path;
}

void ua_files_manager::init_cfgfiles_list()
{
   cfgfiles_list.clear();


#ifdef CONFIGDIR

   std::string globalcfgfile = CONFIGDIR;
   globalcfgfile += "uwadv.cfg";

   cfgfiles_list.push_back(globalcfgfile);

#endif


#ifdef HAVE_HOME

   std::string homecfgfile;

   const char *homedir = getenv("HOME");
   if(homedir)
   {
      // User has a home directory
      homecfgfile = homedir;

#ifndef BEOS
      homecfgfile += "/.";
#else
      homecfgfile += "/config/settings/";
#endif

      homecfgfile += "uwadv.cfg";
   }
   else
   {
      homecfgfile = "uwadv.cfg";
   }

   cfgfiles_list.push_back(homecfgfile);

#else

   // no home dir? get file from local directory

   cfgfiles_list.push_back("uwadv.cfg");

#endif


#if 1
   // some debugging info

   std::cout << "Config files:" << std::endl;
   std::vector<std::string>::iterator iter;
   for (iter = cfgfiles_list.begin();
        iter != cfgfiles_list.end();
        ++iter)
      std::cout << *iter << std::endl;
#endif

}

SDL_RWops *ua_files_manager::get_uadata_file(const char *relpath)
{
   // first, we search for the real
   std::string filename(uadata_path);
   filename.append(relpath);

   // try to open file
   SDL_RWops *ret = SDL_RWFromFile(filename.c_str(),"rb");

   if (ret == NULL)
   {
      // not found

      // try all uadata resource files
      unsigned int uacount=0;
      bool found=true;
      do
      {
         // try to open file
         char buffer[256];
         sprintf(buffer,"uadata%02u.uar",uacount);

         std::string uarfile(uadata_path);
         uarfile.append(buffer);

         // check if file can be found
         FILE *fd = fopen(uarfile.c_str(),"rb");
         found = (fd != NULL);
         if (fd != NULL) fclose(fd);

         if (found)
         {
            // try to open from zip file
            std::string zippath(uarfile);
            zippath.append("/");
            zippath.append(relpath);

            ret = SDL_RWFromZZIP(zippath.c_str(),"rb");
            if (ret != NULL)
               break;
         }

         uacount++;
      }
      while(found);
   }

   return ret;
}

int ua_files_manager::load_lua_script(lua_State *L, const char *basename)
{
   std::string scriptname(basename);
   scriptname.append(".lua");

   bool compiled = false;

   // load lua script
   SDL_RWops *script = get_uadata_file(scriptname.c_str());

   // not found? try binary one
   if (script==NULL)
   {
      scriptname.assign(basename);
      scriptname.append(".lob");
      compiled = true;

      script = get_uadata_file(scriptname.c_str());
   }

   // still not found? exception
   if (script==NULL)
   {
      std::string text("could not load Lua script from uadata: ");
      text.append(scriptname.c_str());
      throw ua_exception(text.c_str());
   }

   ua_trace("loaded Lua %sscript \"%s\"\n",
      compiled ? "compiled " : "", scriptname.c_str());

   // load script into buffer
   std::vector<char> buffer;
   unsigned int len=0;
   {
      SDL_RWseek(script,0,SEEK_END);
      len = SDL_RWtell(script);
      SDL_RWseek(script,0,SEEK_SET);

      buffer.resize(len+1,0);

      SDL_RWread(script,&buffer[0],len,1);
      buffer[len]=0;
   }
   SDL_RWclose(script);

   // execute script
   int ret = lua_dobuffer(L,&buffer[0],len,"");

   if (ret!=0)
      ua_trace("Lua script ended with error code %u\n",ret);

   return ret;
}

bool ua_files_manager::savegames_avail()
{
   // for now we pretend there always are some savegames
   return true;
}

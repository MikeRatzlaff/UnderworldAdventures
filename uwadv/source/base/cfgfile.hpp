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
/*! \file cfgfile.hpp

   \brief general config file handling

*/
//! \ingroup base

//@{

// include guard
#ifndef uwadv_cfgfile_hpp_
#define uwadv_cfgfile_hpp_

// needed includes


// classes

//! config file class
/*! The ua_cfgfile class supports reading and writing configuration files,
    stored as text files that have "key" and "value" pairs. The file might
    look as follows:

    ; one-line comment
    key1 value1
    key2 value2

    Keys and values are separated with at least one whitespace. When saving
    files one space character is put between them. Comments are preserved
    during writing; comments that are on a line with a key/value pair are
    written to a new line.

    To use the class, derive from it and implement at least:
    - load_value()           for reading
    - write_replace()        for writing

    Note: After calling ua_cfgfile::load(SDL_RWops*) the file is not closed
    using SDL_RWclose(). It has to be closed manually.
*/
class ua_cfgfile
{
public:
   //! ctor
   ua_cfgfile();

   //! dtor
   virtual ~ua_cfgfile(){}


   // config file loading

   //! loads a filename
   void load(const char* filename);

   //! loads a config file from SDL_RWops
   void load(SDL_RWops* file);

   //! called to load a specific value
   virtual void load_value(const char* name, const char* value){}


   // config file (re)writing

   //! creates a new config file using the original file as template
   void write(const char* origfile, const char* newfile);

   //! called to replace a value
   virtual void write_replace(const char* name, std::string& value){}

protected:
   //! reads a raw line
   virtual void read_raw_line(const char* line){}

   //! writes a raw line (that didn't contain a key/value pair) to the new file
   void write_raw_line(const char* line);

protected:
   //! indicates if write() is currently called
   bool is_writing;

   //! new file, used by write()
   FILE* newfp;
};


#endif
//@}

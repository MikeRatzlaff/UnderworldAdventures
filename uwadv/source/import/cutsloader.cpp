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
/*! \file cutsloader.cpp

   \brief cutscenes loader

   for more infos about the cutscene format, see the file docs/uw-formats.txt

*/

// needed includes
#include "common.hpp"
#include "cutscene.hpp"
#include "import.hpp"
#include "io_endian.hpp"


// ua_uw_import methods

void ua_uw_import::load_cutscene(const char* filename, ua_image& image,
   std::vector<ua_lp_descriptor>& lpdarray,
   std::vector<Uint8>& lpages, unsigned int& records)
{
   FILE* fd = fopen(filename,"rb");
   if (fd==NULL)
   {
      std::string text("could not open cutscene file ");
      text.append(filename);
      throw ua_exception(text.c_str());
   }

   // get file length
   fseek(fd,0,SEEK_END);
   long filesize = ftell(fd);
   fseek(fd,0,SEEK_SET);

   // read file id
   char sign[4];
   fread(sign,1,4,fd);

   if (0 != strncmp(sign,"LPF ",4))
      throw ua_exception("wrong cutscene file type");

   // number of large pages and records
   fseek(fd,6,SEEK_SET);
   Uint16 largepages = fread16(fd);
   records = fread32(fd);

   fseek(fd,0x10,SEEK_SET);
   fread(sign,1,4,fd);
   if (0 != strncmp(sign,"ANIM",4))
      throw ua_exception("wrong cutscene file type");

   // width and height
   unsigned int xres = fread16(fd);
   unsigned int yres = fread16(fd);
   image.create(xres,yres);

   // skip color cycling infos
   fseek(fd,0x0100,SEEK_SET);

   // create new palette
   image.create_new_palette();

   ua_palette256& anim_palette = *image.get_palette();

   // read in palette
   for(int n=0; n<256; n++)
   {
      anim_palette.set(n,2, fgetc(fd)); // blue
      anim_palette.set(n,1, fgetc(fd)); // green
      anim_palette.set(n,0, fgetc(fd)); // red
      fgetc(fd); // extra pad byte
      anim_palette.set(n,3, 0xff); // transparency
   }

   // now we are at the large page descriptor array
   lpdarray.resize(largepages);

   for(unsigned int i=0; i<largepages; i++)
   {
      lpdarray[i].base = fread16(fd);    // base record
      lpdarray[i].records = fread16(fd); // number of records
      lpdarray[i].bytes = fread16(fd);   // bytes in large page
   }

   // seek to start of the real large pages
   fseek(fd,0x0100 + 256*4 + 6*256,SEEK_SET);

   int pos = ftell(fd);

   // read in all large pages
   unsigned int lpsize = filesize-ftell(fd);
   lpages.resize((lpsize|0xffff)+1,0); // fill up to whole 64k page
   fread(&lpages[0],lpsize,1,fd);

   fclose(fd);
}


// global functions

// dumps pixels to output buffer
void ua_cuts_dump_pixel(Uint8*& src,Uint8*& dst,unsigned int& maxpix,unsigned int size)
{
   unsigned int len = ua_min(size,maxpix);
   memcpy(dst,src,len);
   dst += len;
   src += len;
   maxpix -= len;
}

// does a run with a pixel to output buffer
void ua_cuts_run_pixel(Uint8*& src,Uint8*& dst,unsigned int& maxpix,unsigned int size)
{
   Uint8 pixel = *src++;
   unsigned int len = ua_min(size,maxpix);

   memset(dst,pixel,len);
   dst += len;
   maxpix -= len;
}

// skips some pixels in the output buffer
void ua_cuts_skip_pixel(Uint8*& dst,unsigned int& maxpix,unsigned int size)
{
   if (size>maxpix) size=maxpix;
   dst += size;
   maxpix -= size;
}


// ua_uw_import methods

void ua_uw_import::extract_cutscene_data(Uint8* src,Uint8* dst,unsigned int maxpix)
{
   while(maxpix>0)
   {
      Sint8 cnt = static_cast<Sint8>(*src++);

      // short operations

      if (cnt>0)
      {
         // short dump
         ua_cuts_dump_pixel(src,dst,maxpix,cnt);
         continue;
      }

      if (cnt==0)
      {
         // short run
         Uint8 wordcnt = *src++;
         ua_cuts_run_pixel(src,dst,maxpix,wordcnt);
         continue;
      }

      // remove sign bit
      cnt &= 0x7f;

      if (cnt!=0)
      {
         // short skip
         ua_cuts_skip_pixel(dst,maxpix,cnt);
         continue;
      }

      // long operations

      // get next 16 bit word
#if (SDL_BYTEORDER==SDL_BIG_ENDIAN)
      Uint16 wordcnt = ua_endian_convert16(*(reinterpret_cast<Uint16*>(src)));
#else
      Uint16 wordcnt = *(reinterpret_cast<Uint16*>(src));
#endif
      src+=2;

      // end of decoding?
      if (wordcnt == 0)
         break;

      if (static_cast<Sint16>(wordcnt)>0)
      {
         // long skip
         ua_cuts_skip_pixel(dst,maxpix,wordcnt);
         continue;
      }

      // remove sign bit
      wordcnt &= 0x7fff;

      if (wordcnt>=0x4000)
      {
         // clear "longRun" bit
         wordcnt -= 0x4000;

         // long run
         ua_cuts_run_pixel(src,dst,maxpix,wordcnt);
      }
      else
      {
         // long dump
         ua_cuts_dump_pixel(src,dst,maxpix,wordcnt);
      }
   }
}

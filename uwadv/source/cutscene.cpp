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
/*! \file cutscene.cpp

   cutscene frame extraction

*/

// needed includes
#include "common.hpp"
#include "cutscene.hpp"
#include "resource/fread_endian.hpp"


// global functions

// dumps pixels to output buffer
void ua_cuts_dump_pixel(Uint8 *&src,Uint8 *&dst,unsigned int &maxpix,unsigned int size)
{
   while(size>0 && maxpix>0)
   {
      *dst++ = *src++;
      size--; maxpix--;
   }
}

// does a run with a pixel to output buffer
void ua_cuts_run_pixel(Uint8 *&src,Uint8 *&dst,unsigned int &maxpix,unsigned int size)
{
   Uint8 pixel = *src++;
   while(size>0 && maxpix>0)
   {
      *dst++ = pixel;
      size--; maxpix--;
   }
}

// skips some pixels in the output buffer
void ua_cuts_skip_pixel(Uint8 *&dst,unsigned int &maxpix,unsigned int size)
{
   if (size>maxpix) size=maxpix;
   dst += size;
   maxpix -= size;
}

void ua_cuts_extract_data(Uint8 *src,Uint8 *dst,unsigned int maxpix)
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

const Uint8 *ua_cutscene::get_frame(unsigned int framenum)
{
   // first, search large page to use
   int i=0;
   for(;i<largepages; i++)
      if(lpdarray[i].base <= framenum && lpdarray[i].base + lpdarray[i].records > framenum)
         break;

   if (i>=largepages)
      throw ua_exception("could not find frame in large pages");

   // calculate large page pointer
   Uint16 *curlp16 = reinterpret_cast<Uint16*>(&lpages[0x10000*i]);
   Uint8 *curlp = reinterpret_cast<Uint8*>(curlp16);

   unsigned int destframe = framenum - lpdarray[i].base;

   // calculate offset to proper record
   unsigned int offset=0;
#if (SDL_BYTEORDER==SDL_BIG_ENDIAN)
   for(unsigned int j=0; j<destframe; j++)
      offset += ua_endian_convert16(curlp16[j+4]);
#else
   for(unsigned int j=0; j<destframe; j++)
      offset += curlp16[j+4];
#endif

   // calculate start of "record" struct
   Uint8 *src = curlp+8+lpdarray[i].records*2+offset;
   Uint16 *src16 = reinterpret_cast<Uint16*>(src);

   // add extra offset
   if(src[1])
      src += ( src16[1] + ( src16[1] & 1 ));

   // extract the pixel data
   ua_cuts_extract_data(&src[4],&outbuffer[0],width*height);

   return &outbuffer[0];
}

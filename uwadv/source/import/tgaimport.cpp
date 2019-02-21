//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002,2003,2004,2019 Underworld Adventures Team
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
/// \file tgaimport.cpp
/// \brief tga file import
///
/// does tga texture import like unpacking and scanline reading
///
/// list of all tga types (field "tgatype", 3rd byte in file):
///  0   no image data included
///  1   uncompressed, color-mapped images.
///  2   uncompressed, RGB images.
///  3   uncompressed, black and white images.
///  9   runlength encoded color-mapped images.
/// 10   runlength encoded RGB images.
/// 11   compressed, black and white images.
/// 32   compressed color-mapped data, using Huffman, Delta, and runlength encoding.
/// 33   compressed color-mapped data, using Huffman, Delta, and runlength encoding.
///      4-pass quadtree-type process.
///
/// runlength encoded tga format (tga type 9 or 10):
/// the first byte's msb indicates what type of data follows.
/// 1 indicates that the bits 0..7 specify a repeat count for the
///   next color value
/// 0 indicates that bits 0..7 specify the count of uncompressed
///   color values that follows
/// note that color value means 2, 3 or even 4 bytes of data, dependent
/// on the nuber of bits per pixel
///
/// when decoding 16-bits-per-pixel-images, the rgb intensities are crunched
/// into one 16-bit word. look for the code comment below
//
#include "import.hpp"
#include "file.hpp"
#include <algorithm>

/// tga decoding info struct
struct ua_tga_info
{
   // tga header

   /// length of id area
   unsigned int idlength;        // 1 byte
   /// color map type;
   unsigned int colormaptype;    // 1 byte
   /// tga type; 2: uncompressed RGB image
   unsigned int tgatype;         // 1 byte

   // colormap (not used)
   /// start of color map
   unsigned int colormaporigin;  // 2 byte
   /// length of color map
   unsigned int colormaplength;  // 2 byte
   /// bit depth of color map
   unsigned int colormapdepth;   // 1 byte

   /// x and y origin
   unsigned int xorigin, yorigin; // 2 byte
   /// width and height
   unsigned int width, height;    // 2 byte

   /// bits per pixel, either 16, 24 or 32
   unsigned int bitsperpixel;    // 1 byte
   /// image descriptor
   unsigned int imagedescriptor; // 1 byte

   // tga decoding data

   /// texture resolution (must be 2^n each)
   unsigned int xres, yres;

   /// current scanline
   std::vector<Uint8> scanline;

   /// number of bytes per pixel
   unsigned int bytesperpixel;

   /// count of raw bytes to pass
   unsigned int rawcount;
   /// count of bytes to repeat
   int repeatcount;

   /// bytes to repeat (length depends on bytesperpixel)
   Uint8 repeatbytes[4];
};

void ua_tga_read_scanline(Base::File& file, ua_tga_info& info, Uint32* line);

/// \todo implement reading bottom-up images
void ua_import_tga(Base::File& file, unsigned int& xres, unsigned int& yres,
   unsigned int& origx, unsigned int& origy, std::vector<Uint32>& texels)
{
   ua_tga_info info;

   // read in tga header
   {
      info.idlength = file.Read8();
      info.colormaptype = file.Read8();
      info.tgatype = file.Read8();

      info.colormaporigin = file.Read16();
      info.colormaplength = file.Read16();
      info.colormapdepth = file.Read8();

      info.xorigin = file.Read16();
      info.yorigin = file.Read16();
      origx = info.width = file.Read16();
      origy = info.height = file.Read16();

      info.bitsperpixel = file.Read8();
      info.imagedescriptor = file.Read8();

      info.bytesperpixel = info.bitsperpixel >> 3;
   }

   // check if supported tga type
   if (info.tgatype != 2 && info.tgatype != 10)
      return;

   // init decoding data
   info.rawcount = info.repeatcount = 0;

   // determine texture resolution (must be 2^n)
   {
      xres = 16;
      while (xres < origx && xres < 2048) xres <<= 1;

      yres = 16;
      while (yres < origy && yres < 2048) yres <<= 1;

      info.xres = xres;
      info.yres = yres;

      texels.resize(xres*yres, 0x00000000);
      info.scanline.resize(xres*yres * 4);
   }

   // skip id header and color table
   if (info.idlength + info.colormaplength > 0)
      file.Seek(info.idlength + info.colormaplength, Base::seekCurrent);

   // load scanlines (saved top-down)
   for (unsigned int line = 0; line < origy; line++)
      ua_tga_read_scanline(file, info, &texels[line*xres]);

   // (saved bottom-up)
//   for(int line=origy-1; line>=0; line--)
//      ua_tga_read_scanline(rwops,info,&texels[line*xres]);
}

/// \todo implement 16- and 32-bit scanline reading
void ua_tga_read_scanline(Base::File& file, ua_tga_info& info, Uint32* line)
{
   // read in one line into scanline buffer
   switch (info.tgatype)
   {
   case 2: // uncompressed rgb image
      file.ReadBuffer(&info.scanline[0], info.width * info.bytesperpixel);
      break;

   case 10: // rle-compressed rgb image
   {
      unsigned int size = 0; // size in 'color values'

      while (size < info.width)
      {
         // raw bytes available?
         while (info.rawcount > 0 && size < info.width)
         {
            // read in color values
            unsigned int take = std::max(info.rawcount, info.width - size);
            file.ReadBuffer(&info.scanline[size * info.bytesperpixel], take * info.bytesperpixel);
            size += take;
            info.rawcount -= take;
         }

         // repeat color values?
         while (info.repeatcount > 0 && size < info.width)
         {
            memcpy(&info.scanline[size * info.bytesperpixel], info.repeatbytes, info.bytesperpixel);
            size++;
            info.repeatcount--;
         }

         if (size < info.width)
         {
            unsigned int next = file.Read8();

            if ((next & 0x80) != 0)
            {
               // repeat value
               info.repeatcount = (next & 0x7f) + 1;
               file.ReadBuffer(info.repeatbytes, info.bytesperpixel);
            }
            else
               // raw count
               info.rawcount = (next & 0x7f) + 1;
         }
      }
   }
   break;
   }

   // copy color values into the line buffer
   switch (info.bytesperpixel)
   {
   case 2: // 16-bit
   {
      // uses GGGBBBBB ARRRRRGG storage
      for (unsigned int i = 0; i < info.width; i++)
      {
         /* TODO
                     int offs1 = i<<2, offs2 = (i<<1)+i;
                     line[offs1+0] = (scanline[offs2+1] & (~0x7c)) << 1;
                     line[offs1+1] = ((scanline[offs2+1] & (~0x03)) << 6)
                                    | ((scanline[offs2+0] & (~0xe0)) >> 2);
                     line[offs1+2] = (scanline[offs2+0] & (~0x1f)) << 3;
                     line[offs1+3] = (scanline[offs2+1] & 0x80)>0 ? 255 : 0; // alpha value
         */
      }
   }
   break;

   case 3: // 24-bit
   {
      for (unsigned int i = 0; i < info.width; i++)
      {
         for (int j = 0; j < 3; j++)
            line[i] |= Uint32(info.scanline[(i << 1) + i + (2 - j)]) << (8 * j);

         line[i] |= 0xff000000; // alpha value
      }
   }
   break;

   case 4: // 32-bit
   {
      /* TODO
               memcpy(line,scanline,xres*4);

               // change order from BGRA to RGBA
               for(int i=0;i<info.width;i++)
               {
                  int offs = i<<2, help;
                  help = line[offs+0];
                  line[offs+0] = line[offs+2];
                  line[offs+2] = help;
               }
      */
   }
   break;
   }
}

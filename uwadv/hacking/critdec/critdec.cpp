// critdec.cpp
//

#include <stdio.h>
#include <string>


void tga_writeheader(FILE *fd, int width, int height, int type=2, int colmap=0)
{
   #pragma pack(push,1)

   // tga header struct
   struct tgaheader
   {
      unsigned char idlength;     // length of id field after header
      unsigned char colormaptype; // 1 if a color map is present
      unsigned char tgatype;      // tga type

      // colormap not used
      unsigned short colormaporigin;
      unsigned short colormaplength;
      unsigned char colormapdepth;

      // x and y origin
      unsigned short xorigin,yorigin;
      // width and height
      unsigned short width,height;

      // bits per pixel, either 16, 24 or 32
      unsigned char bitsperpixel;
      unsigned char imagedescriptor;
   } tgaheader =
   {
      0, colmap, type,   0, (colmap==1?256:0), (colmap==1?24:0),
      0, 0, width, height, colmap==1?8:32, 0x20
   };
#pragma pack(pop)

   fwrite(&tgaheader,1,18,fd);
}


void decode_rle(FILE *fd,FILE *out,unsigned int bits,unsigned int datalen,unsigned char *auxpalidx)
{
   // bit extraction variables
   unsigned int bits_avail=0;
   unsigned int rawbits;
   unsigned int bitmask = ((1<<bits)-1) << (8-bits);
   unsigned int nibble;

   // rle decoding vars
   int pixcount=0;
   int stage=0; // we start in stage 0
   int count;
   int record=3; // we start with record 0=repeat (3=run)
   int repeatcount=0;

   while(pixcount<datalen)
   {
      // get new bits
      if (bits_avail<bits)
      {
         // not enough bits available
         if (bits_avail>0)
         {
            nibble = ((rawbits & bitmask) >> (8-bits_avail));
            nibble <<= (bits - bits_avail);
         }
         else
            nibble = 0;

         rawbits = fgetc(fd);
         if (rawbits==EOF)
            return;

//         printf("fgetc: %02x\n",rawbits);

         unsigned int shiftval = 8 - (bits - bits_avail);

         nibble |= (rawbits >> shiftval);

         rawbits = (rawbits << (8-shiftval)) & 0xFF;

         bits_avail = shiftval;
      }
      else
      {
         // we still have enough bits
         nibble = (rawbits & bitmask)>>(8-bits);
         bits_avail -= bits;
         rawbits <<= bits;
      }

//      printf("nibble: %02x\n",nibble);

      // now that we have a nibble

      switch(stage)
      {
      case 0: // we retrieve a new count
         if (nibble==0)
            stage++;
         else
         {
            count = nibble;
            stage=6;
         }
         break;
      case 1:
         count = nibble;
         stage++;
         break;

      case 2:
         count = (count<<4) | nibble;
         if (count==0)
            stage++;
         else
            stage = 6;
         break;

      case 3:
      case 4:
      case 5:
         count = (count<<4) | nibble;
         stage++;
         break;
      }

      if (stage<6) continue;

      switch(record)
      {
      case 0:
         // repeat record stage 1
//         printf("repeat: new count: %x\n",count);

         if (count==1)
         {
            record=3; // skip this record; a run follows
            break;
         }

         if (count==2)
         {
            record=2; // multiple run records
            break;
         }

         record=1; // read next nibble; it's the color to repeat
         continue;

      case 1:
         // repeat record stage 2

         {
            // repeat 'nibble' color 'count' times
            for(int n=0; n<count; n++)
            {
               fputc(auxpalidx[nibble],out);
               pixcount++;
            }
         }

//         printf("repeat: wrote %x times a '%x'\n",count,nibble);

         if (repeatcount==0)
         {
            record=3; // next one is a run record
         }
         else
         {
            repeatcount--;
            record=0; // continue with repeat records
         }
         break;

      case 2:
         // multiple repeat stage

         // 'count' specifies the number of repeat record to appear
//         printf("multiple repeat: %u\n",count);
         repeatcount = count-1;
         record=0;
         break;

      case 3:
         // run record stage 1
         // copy 'count' nibbles

//         printf("run: count: %x\n",count);

         record=4; // retrieve next nibble
         continue;

      case 4:
         // run record stage 2

         // now we have a nibble to write
         fputc(auxpalidx[nibble],out);
         pixcount++;

         if (--count==0)
         {
//            printf("run: finished\n");
            record = 0; // next one is a repeat again
         }
         else
            continue;
         break;
      }

      stage=0;
      // end of while loop
   }
}


const char *cr_fmt = "d:\\store\\uw_demo\\crit\\cr%02opage.n%02o";

int main(int argc, char* argv[])
{
   const char *assocfilename = "d:\\store\\uw_demo\\crit\\dassoc.anm";

   // get 256 colors palette
   char palette[256*3];
   {
      FILE *pal = fopen("d:\\store\\uw_demo\\data\\pals.dat","rb");

      fread(palette,1,256*3,pal);

      for(int j=0; j<256*3; j++) palette[j]<<=2;

      for(int k=0; k<256; k++)
      {
         char h = palette[k*3+0];
         palette[k*3+0] = palette[k*3+2];
         palette[k*3+2] = h;
      }

      fclose(pal);
   }

   for(int crit=0; crit<32; crit++)
   {
      // retrieve critter name
      char critname[8]; critname[0]=0;
      unsigned char anim=0;
      unsigned char variant=0;

      FILE *assoc = fopen(assocfilename,"rb");
      fseek(assoc,crit*8,SEEK_SET);
      fread(critname,1,8,assoc);

      fseek(assoc,256+crit*2,SEEK_SET);
      anim = fgetc(assoc);
      variant = fgetc(assoc);
      fclose(assoc);

      {
         char buffer[256];
         sprintf(buffer,cr_fmt,crit,0);

         FILE *pfile = fopen(buffer,"rb");
         if (pfile==NULL)
            continue;

         fclose(pfile);
      }

      printf("critter #%u: \"%.8s\", anim=%02x, variant=%02x\n",crit,critname,anim,variant);

      // read in all pages
      for(int page=0;;page++)
      {
         char buffer[256];
         sprintf(buffer,cr_fmt,crit,page);
         FILE *pfile = fopen(buffer,"rb");
         if (pfile==NULL)
            break;

         // read in segment directory
         int segbase = fgetc(pfile);
         int nsegs = fgetc(pfile);

         if (nsegs==0)
         {
            printf("no segments in file\n");
            continue;
         }

         unsigned char *segdiroffsets = new unsigned char[nsegs];
         fread(segdiroffsets,1,nsegs,pfile);

         printf("cr%02opage.n%02o:\n",crit,page);

         int nanimsegs = fgetc(pfile);

         for(int i=0; i<nanimsegs; i++)
         {
            printf("segment #%u (%02x): ",i,segdiroffsets[i]);

            for(int n=0; n<8; n++)
               printf("%02x ",fgetc(pfile));
            printf("\n");
         }

         int nauxpals = fgetc(pfile);
         if (nauxpals==-1) continue;

         unsigned char *auxpals = new unsigned char[32*nauxpals];
         fread(auxpals,32,nauxpals,pfile);

         int noffsets = fgetc(pfile);
         int unk1 = fgetc(pfile);

         unsigned short *alloffsets = new unsigned short[noffsets];
         fread(alloffsets,2,noffsets,pfile);

         int pos = ftell(pfile);

         printf("content: %u segs, %u auxpals, %u frames\n",
            nsegs,nauxpals,noffsets);

         for(int frame=0; frame<noffsets; frame++)
         {
            fseek(pfile,alloffsets[frame],SEEK_SET);
            int width,height,hotx,hoty,type;

            width = fgetc(pfile);
            height = fgetc(pfile);
            hotx = fgetc(pfile);
            hoty = fgetc(pfile);
            type = fgetc(pfile);

            printf("frame #%d, %ux%u, hot:(%u,%u), type=%u\n",
               frame,width,height,hotx,hoty,type);

            // seek to bitmap data
            fseek(pfile,alloffsets[frame],SEEK_SET);

            // decode bitmap
            char buffer[256];
            sprintf(buffer,"crit%03opage%02oframe%02u.tga",crit,page,frame);

            FILE *tga = fopen(buffer,"wb");

            tga_writeheader(tga,width,height,1,1);

            // write palette
            fwrite(palette,1,256*3,tga);

            decode_rle(pfile,tga,5,width*height,auxpals);

            fclose(tga);
         }

         delete[] alloffsets;
         delete[] auxpals;
         delete[] segdiroffsets;
         fclose(pfile);
      }

      printf("\n");
   }

   return 0;
}

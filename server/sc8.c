//This file has a lot of unused things that have to be removed. 
//(I used the original and adapted to convert to screen8).
//It works but 2/3 of the code is not actually useful for this task.
//Thomas Glufke 


/*
---------------------------------------------------------------
TMSOPT v.0.1 - Eduardo A. Robsy Petrus & Arturo Ragozini 2007
Credits to Rafael Jannone for his Floyd-Steinberg implementation
---------------------------------------------------------------
 TGA image converter (24 bpp, uncompressed) to TMS9918 format
---------------------------------------------------------------
Overview
---------------------------------------------------------------
Selects the best solution for each 8x1 pixel block
Optimization uses the following algorithm:

(a) Select one 1x8 block, select a couple of colors, apply
    Floyd-Steinberg within the block, compute the squared error,
    repeat for all 105 color combinations, keep the best couple
    of colors.

(b) Apply Floyd-Steinberg to the current 1x8 block with the best
    two colors seleted before and spread the errors to the
    adjacent blocks.

(c) repeat (a) and (b) on the next 1x8 block, scan all lines.

(d) Convert the image in pattern and color definitions (CHR & CLR)

To load in MSX basic use something like this:

10 screen 2: color 15,0,0
20 bload"FILE.CHR",s
30 bload"FILE.CLR",s
40 goto 40

---------------------------------------------------------------
Compilation instructions
---------------------------------------------------------------
 Tested with GCC/Win32 [mingw]:

   GCC TMSopt.c -oTMSopt.exe -O3 -s

 It is standard C, so there is a fair chance of being portable!
 NOTE
 In the current release the name of the C file has become scr2floyd.c
---------------------------------------------------------------
History
---------------------------------------------------------------
 Ages ago   - algorithm created
 16/05/2007 - first C version (RAW format)
 17/05/2007 - TGA format included, some optimization included
 18/05/2007 - Big optimization (200 times faster), support for
              square errors
 19/05/2007 - Floyd-Stenberg added, scaling for better rounding
 24/05/2007 - Floyd-Stenberg included in the color optimization.
---------------------------------------------------------------
Legal disclaimer
---------------------------------------------------------------
 Do whatever you want to do with this code/program.
 Use at your own risk, all responsability would be declined.
 It would be nice if you credit the authors, though.
---------------------------------------------------------------
*/

// Headers!

#include<stdio.h>
#include<time.h>
#include<limits.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>

typedef unsigned int    uint;
typedef unsigned char   uchar;
typedef unsigned short  ushort;
typedef unsigned long   ulong;

//#define DEBUG

#define scale 1
#define inrange8(t) ((t)<0) ? 0 :(((t)>255) ? 255:(t))
#define clamp(t)    ((t)<0) ? 0 :(((t)>255*scale) ? 255*scale : (t))


typedef struct {
   float r, g, b;
} RGB;

float ColourDistance(RGB e1, RGB e2)
{
  float r,g,b;
  float rmean;
  
  e1.r/=scale;
  e1.g/=scale;    
  e1.b/=scale;    
  
  e2.r/=scale;
  e2.g/=scale;    
  e2.b/=scale;    

  rmean = ( (int)e1.r + (int)e2.r ) / 2 ;
  r = ((int)e1.r - (int)e2.r);
  g = ((int)e1.g - (int)e2.g);
  b = ((int)e1.b - (int)e2.b);
//  return r*r+g*g+b*b;
  return ((((512+rmean)*r*r)/256) + 4*g*g + (((767-rmean)*b*b)/256));
}

// Just one function for everything

void main(int argc, char **argv)
{

// Vars

 FILE *file,*CHR,*CLR;
 int bc,bp,i,j,x,y,c,p,k,MAXX,MAXY;
 uint n,total=0,done=0,size;
 char *name;
 short image[512+2][512+2][3],header[18],palette[16][3];

// TMS9918 RGB palette - approximated 50Hz PAL values
 uint pal[16][3]= {
{ 0,0,0},                 // 0 Transparent
{ 0,0,0},                 // 1 Black           0    0    0
{ 33,200,66},             // 2 Medium green   33  200   66
{ 94,220,120},            // 3 Light green    94  220  120
{ 84,85,237},             // 4 Dark blue      84   85  237
{ 125,118,252},           // 5 Light blue    125  118  252
{ 212,82,77},             // 6 Dark red      212   82   77
{ 66,235,245},            // 7 Cyan           66  235  245
{ 252,85,84},             // 8 Medium red    252   85   84
{ 255,121,120},           // 9 Light red     255  121  120
{ 212,193,84},            // A Dark yellow   212  193   84
{ 230,206,128},           // B Light yellow  230  206  128
{ 33,176,59},             // C Dark green     33  176   59
{ 201,91,186},            // D Magenta       201   91  186
{ 204,204,204},           // E Gray          204  204  204
{ 255,255,255}            // F White         255  255  255
};
// Scale palette

 for (i=0;i<16;i++)
     for (k=0;k<3;k++)
        palette[i][k] = scale*pal[i][k];

// Get time

 clock();

// Application prompt

 printf("TMSopt v.0.1 - TGA 24bpp to TMS9918 converter.\nCoded by Eduardo A. Robsy Petrus & Arturo Ragozini 2007.\n\n");
 printf("Credits to Rafael Jannone for his Floyd-Steinberg implementation.\n \n");


// Guess the name of the image I used for testing
#ifdef DEBUG
argc = 2;
argv[1] = malloc(20);
argv[1][0] = 'l';
argv[1][1] = 'e';
argv[1][2] = 'n';
argv[1][3] = 'n';
argv[1][4] = 'a';
argv[1][5] = '_';
argv[1][6] = '.';
argv[1][7] = 't';
argv[1][8] = 'g';
argv[1][9] = 'a';
argv[1][10] = 0;
#endif

// Test if only one command-line parameter is available

 if (argc==1)
 {
  printf("Syntax: TMSopt [file.tga]\n");
  return;
 }

// Open source image (TGA, 24-bit, uncompressed)

 if ((file=fopen(argv[1],"rb"))==NULL)
 {
  printf("cannot open %s file!\n",argv[1]);
  return;
 }

// Read TGA header

 for (i=0;i<18;i++) header[i]=fgetc(file);

// Check header info

 for (i=0,n=0;i<12;i++) n+=header[i];

// I deleted the check on n, was it important ?
 if ((header[2]!=2)||(header[17])||(header[16]!=24))
 {
  printf("Unsupported file format!\n");
  return;
 }

// Calculate size

 MAXX=header[12]|header[13]<<8;
 MAXY=header[14]|header[15]<<8;

 size=MAXX*MAXY;
 printf("MAXX: %d  MAXYY:%d  Size: %d", MAXX, MAXY,size);


// Check size limits

 if ((!MAXX)||(MAXX>512)||(!MAXY)||(MAXY>512))
 {
  printf("Unsupported size!");
  return;
 }

// Load image data

 for (y=MAXY-1;y>=0;y--)
  for (x=0;x<MAXX;x++)
   for (k=0;k<3;k++)
    image[x+1][y+1][2-k]=((short)fgetc(file))*scale;        // Scale image

 for (x=0;x<MAXX;x++)
    for (k=0;k<3;k++)
        image[x][0][k] = image[x][1][k];

 for (y=0;y<MAXY;y++)
    for (k=0;k<3;k++)
        image[0][y][k] = image[1][0][k];



//printf("%d" , MAXX );
//printf("%d" , MAXY );
//printf("%d", (int) lrint( (double)245/32) );
// Close file

 fclose(file);

// Information

 printf("Converting %s (%i,%i) to TMS9918 format ",argv[1],MAXX,MAXY);
 printf("in (%i,%i) screen 2 tiles...    ",((MAXX+7)>>3),((MAXY+7)>>3));



// Conversion done

 printf("\b\b\bOk   \n");


// Create TMS output files (CHR, CLR)

 argv[1][strlen(argv[1])-3]='C';
 argv[1][strlen(argv[1])-2]='H';
 argv[1][strlen(argv[1])-1]='R';
 CHR=fopen(argv[1],"wb");


 fputc(0xFE,CHR);    // Binary data
 fputc(0x00,CHR);    // Start at 0000h
 fputc(0x00,CHR);
 fputc(0xFF,CHR);    // Stop at 17FFh  D3FFh or 54271
 fputc(0xD3,CHR);
 fputc(0x00,CHR);    // Run
 fputc(0x00,CHR);

   // Save best pattern and colour combination
   // NOTE1:
   // THIS PART CAN BE LARGELY CUTTED AND OPTIMIZED REUSING
   // RESULTS FROM THE PREVIOUS LOOP, BUT WHO CARES?
   // NOTE2:
   // This code can be used for conversion without dithering

 for (y=0;y<MAXY;y++)
    for (x=0;x<MAXX;x++)
     //   for (j=0;j<8;j++)
        {
            uchar c1,c2;
            uint bs = INT_MAX;
            uchar bp = 0, bc = 0;
            uint cp, n1, n2, n3;
	    uint aaa;
	    uchar bbb;
	    uchar a1,a2,a3;

      //      uint yy = 1+((y<<3)|j);


          //  printf("%d", (int) lrint( (double)245/32) );


n1 = ( (int) lrint( (double)(image[1+x][1+y][1])/32-1 )  ) * 32;
n2 = ( (int) lrint( (double)(image[1+x][1+y][0])/32-1 )  ) * 4;
n3 = ( (int) lrint( (double)(image[1+x][1+y][2])/64-1 )  );


a1  = ( image[1+x][1+y][1]   * 8) / 256;
a2  = ( image[1+x][1+y][0]   * 8) / 256;
a3  = ( image[1+x][1+y][2]   * 4) / 256;

//   byte eightBitColor = (red << 5) | (green << 2) | blue;


if ((int)n1 <1 ) n1=0;
if ((int)n2 <1 ) n2=0;
if ((int)n3 <1 ) n3=0;






aaa = n1 + n2 + n3;

bbb = (a1 <<5 ) | (a2 << 2) | a3; 

if (x==1) 
printf( "a:%d b:%d c:%d aaa: %d --> %d %d %d "
		,image[1+x][1+y][0]
		,image[1+x][1+y][1]
	        ,image[1+x][1+y][2]
		,aaa
		,n1
		,n2
		,n3
		);

//if (y==7) printf("/n");

	    fputc( aaa
                    ,CHR);       // Scale to char




         // fputc(bp,CHR);
        }


 fclose(CHR);
printf("END CHR");


// Generate new name

 name = malloc(0x100);
 argv[1][strlen(argv[1])-4]=0;
 strcpy(name,argv[1]);
 strcat(name,"_tms.tga");

// Save file header

 file=fopen(name,"wb");

 for (i=0;i<18;i++) fputc(header[i],file);

// Save image data

 for (y=MAXY-1;y>=0;y--)
 {
 for (x=0;x<MAXX;x++)
  {
//    image[1+x][1+y][printf("%d", (int) lrint( (double)245/32) );

printf (" %d ", image[1+x][1+y][0] );
    fputc(   (int) lrint( (double)(image[1+x][1+y][0])/32)  
	   + (int) lrint( (double)(image[1+x][1+y][1])/4)     
	
		    ,file);       // Scale to char
  }
 }
// Close file

 fclose(file);

// Prompt elapsed time

 printf("%.2f million combinations analysed in %.2f seconds.\n",total/1e6,(float)clock()/(float)CLOCKS_PER_SEC);
 printf("Note: the .CLR and .CHR files have correct headers only for 256x192 images. \n");

}


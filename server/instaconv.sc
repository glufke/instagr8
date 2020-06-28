#!/bin/bash

#---------------------
# FILE..: instaconv.sc
# DESC..: This file receives the URL of the JPG img to be converted to MSX.
# PARAM.: 1 - The SESSION Id
#         2 - The URL of the image.
#         3 - Number indicating the version of the MSX (1 or 2)
# CONFIG: Edit the SRVPATH if for any reason you server needs a special path.
#         Otherwise, keep it "" 
#         IMGPATH is the name of folder to keep the JPG and converted files.

#Parameters
SES=$1
IMG=$2
VER=$3

#If the version was ommited, consider MSX1.
if [ -z "$VER"];
then
VER=1
fi

#Config
SRVPATH=""
IMGPATH="${SRVPATH}img/" 

#Check the parameters
if [[ -z "$SES" ]] &&
   [[ -z "$IMG" ]] 
then
  echo "This program requires 2 parameters:"
  echo " - SESSION_ID --> A number"
  echo " - IMAGE_URL  --> The URL to be converted (For now, a jpg image)"
  echo " - VERSION    --> (Optional). This may be 1 or 2. (for msx1 or msx2)"
  exit 0
fi

#Deletes old files, if found.
find $IMGPATH -mmin +2 -type f -exec rm -fv {} \;

#Set Image names/path
INPFILE="${IMGPATH}in${SES}.jpg"
OUTFILE="${IMGPATH}out${SES}.tga"
TMPFILE="${IMGPATH}tmp${SES}.png"
TMPFILE2="${IMGPATH}tmpi${SES}.png"
TXTIMGFILE="${IMGPATH}txt${SES}.bmp"
TXTFILE="${IMGPATH}txt${SES}.txt" 
rm $OUTFILE

#Download the Image URL
wget  "$IMG" -O $INPFILE 
chmod 666 $INPFILE

# ------- MSX 1 conversion
if [[ "$VER" = "1" ]]; 
then
#convert the image to MSX size
convert $INPFILE -orient BottomLeft -resize 256x192 -background black -gravity center -extent 256x192 -roll +32+0 $TMPFILE
chmod 666 $TMPFILE

#create TEXT to img
convert -size 64x140 -background white -font small5.ttf -pointsize 10 caption:"$(cat $TXTFILE)" +dither -monochrome  $TXTIMGFILE
chmod 666 $TXTIMGFILE

#add instragr8 logo and msx logo (64x192 pixels)
composite instagr81l.bmp $TMPFILE $TMPFILE2
chmod 666 $TMPFILE2

composite -geometry +0+25 $TXTIMGFILE $TMPFILE2 $OUTFILE

#convert the imge to Screen2 Tiles and Color. (thanks https://www.msx.org/downloads/related/graphics/screen-2-converter )
./sc2 $OUTFILE
fi

# ------- MSX 2 conversion
if [[ "$VER" = "2" ]];
then
#convert the image to MSX size
convert $INPFILE -orient BottomLeft -resize 256x212 -background black -gravity center -extent 256x212 -roll +22+0 $TMPFILE
chmod 666 $TMPFILE

#create TEXT to img
convert -size 44x160 -background white -font Courier -pointsize 10 caption:"$(cat $TXTFILE)" $TXTIMGFILE
chmod 666 $TXTIMGFILE 

#add instragr8 logo and msx logo (64x192 pixels)
composite instagr82l.bmp $TMPFILE $TMPFILE2
chmod 666 $TMPFILE2

composite -geometry +0+25 $TXTIMGFILE $TMPFILE2 $OUTFILE
#convert the imge to Screen8. (Just 1 file)
./sc8 $OUTFILE 
fi

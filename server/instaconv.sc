#!/bin/bash

#---------------------
# FILE..: instaconv.sc (instagr8)
# DESC..: This file receives the URL of the JPG img to be converted to MSX.
# PARAM.: 1 - The SESSION Id
#         2 - The URL of the image.
# CONFIG: Edit the SRVPATH if for any reason you server needs a special path.
#         Otherwise, keep it "" 
#         IMGPATH is the name of folder to keep the JPG and converted files.

#Parameters
SES=$1
IMG=$2

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
  exit 0
fi

#Set Image names/path
INPFILE="${IMGPATH}in${SES}.jpg"
OUTFILE="${IMGPATH}out${SES}.tga"
TMPFILE="${IMGPATH}tmp${SES}.jpg"
rm $OUTFILE

#Download the Image URL
wget  "$IMG" -O $INPFILE 

#convert the image to MSX size
convert $INPFILE -orient BottomLeft -resize 256x192 -background black -gravity center -extent 256x192 -roll +32+0 $TMPFILE

#add instragr8 logo and msx logo (64x192 pixels)
composite instagr8l.bmp $TMPFILE $OUTFILE

#convert the imge to Screen2 Tiles and Color. (thanks https://www.msx.org/downloads/related/graphics/screen-2-converter )
./sc2 $OUTFILE 

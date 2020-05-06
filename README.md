# instagr8 for MSX1

Watch the Youtube video: [Link](https://www.youtube.com/watch?v=L-G7gzgepdA)
Oficial msx.org topic: [Link](https://www.msx.org/forum/msx-talk/software/msx-instagr8-instagram-for-msx1)

### INSTRUCTIONS
* Download the file INSTAGR8.BAS and save in a floppy disk or SD card for your MSX.
* In BASIC, execute with:
LOAD "INSTAGR8.BAS",R

### WHAT IS NEEDED:
You need only the cartridge GR8NET. It doesn't matter what SETMAP it is configured.
The GR8NET has many features, but only 2 are being used in this project:
- BLOAD "HTTP"
- OPEN "HTTP"
Its needed also a server as a bridge (you can use mine: 54.201.73.96). You will understand why this is needed.

### HOW IT WORKS
MSX computers run at 3.56mhz and have about 25KB of memory available in Basic. Its virtually impossible for the MSX to perform the followint tasks:
- connect into HTTPS (Instagram API)
- retrieve huge JSON files and parse the JSON
- download huge images from the Instagram API
- convert it to 16 color, low resolution
That's why a server is needed in the middle, to make all the heavy work.

### HOW IT WORKS (Detailed explanation)
1. BASIC generates a random number (5 digits) to be used as "session". Example 1234
2. BASIC Input what to show. It can be an INSTAGRAM USERNAME, or an INSTAGRAM TAG, starting with #.
3. BASIC open a HTTP request to the "Simplification Layer", I will call it SERVER. The HTTP request has the following structure:
`http://SERVER/instagr8/msxinsta.php?t=t&u=glufke&n=1&s=1234`
Being:

| Parameter | Description |
| ------ | ------ |
|u | the search string, i.e. msx
|s | the session ID, i.e. 1234
|t | this is the TYPE being "u" for username and "t" for tag (or hashtag)
|n | this is the number of the image being retrieved. (from 1 to ...)
|v | this is the version of the MSX (1 or 2). It's an optional parameter.

4. SERVER captures the request (I'm using Apache, but you can user NGINX or any other)
5. SERVER Apache calls the PHP script "msxinsta.php" with all the parameters.
6. PHP open one of these 2 URL's that belongs to Instagram
`https://www.instagram.com/someuser?__a=1`
`https://www.instagram.com/explore/tags/tagblablabla?__a=1`
7. PHP saves in a variable the JSON file retruned.
8. PHP parses the JSON file in one ARRAY.
9. PHP retrieves some basic information from the ARRAY returned, like name of the account. (It can bring many different information, like the number of followers, etc, but we don't have "space" in the MSX screen to show all these infos)
10. PHP loops into the ARRAY an retrieve the exact post we want. (this is passed by the parameter N). In other words, MSX just say "I want the Nth post of this user or tag".
11. PHP finds the Nth post and saves the "Description" and URL of the JPG image.
12. PHP calls a SHELL script to make the conversion
13. SHELL calls WGET to save URL with the JPG. (Saves with the name in1234.jpg, being 1234 the session number created in MSX)
14. SHELL calls IMAGEMAGICK to resize to 192x192 pixels.
15. SHELL calls IMAGEMAGICK to add the "Instagr8" logo and MSX logo (it's a 64x192 image) and saves it in a TGA image format)
16. SHELL calls the conversion C executable. (sc2 or sc8) previosuly compiled. This script is found [here](https://www.msx.org/downloads/related/graphics/screen-2-converter). (Thanks ARTRAG and Jannone)
17. For MSX1, the sc2.c creates 2 files. out1234.CHR and out1234.CLR. (one for pattern table &H0000 of VRAM and one for color table &H2000 of VRAM).
    For MSX2, the sc8.c creates just 1 file. out1234.CHR. (Starting at &H0000 to &HD3FF).
18. Back to PHP, truncates the "Description" of the image to 128 characters. (We don't have more space in the MSX screen. About 16 lines x 8 chars)
19. PHP sends back to MSX Basic the Description. (While the 2 files, CHR and CLR, are saved in the server)
20. BASIC retrieves the Description Content.
21. BASIC uses BLOAD "out1234.CHR" (and "out1234.CLR" for screen 2) to load into GR8NET internal memory. 
22. BASIC calls GR8NET CALL NETBTOV(0,0) to copy from internal GR8NET ram to VRAM.
23. BASIC loops the Description and prints the Description 8 chars at the time.

### SOURCES
The initial files are just a "proof of concept". Many things will be improved in the next days. Feel free to change it or send suggestions.

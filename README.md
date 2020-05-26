# instagr8 for MSX1 and MSX2

Watch:
* Youtube video1: [Link](https://www.youtube.com/watch?v=L-G7gzgepdA)
* Youtube video2: [Link](https://youtu.be/487cka18JU4)

Oficial msx.org topic: [Link](https://www.msx.org/forum/msx-talk/software/msx-instagr8-instagram-for-msx1)

### INSTRUCTIONS
* Download the file INSTAGR8.BAS and save in a floppy disk or SD card for your MSX.
* You can download the last version directly in MSX using [HGET](https://www.konamiman.com/msx/networking/hget.com) with this command in MSX DOS:

`hget glufke.ddns.net:8080/INSTAGR8.BAS`
* In BASIC, execute with:
LOAD "INSTAGR8.BAS",R

Type the instagram account or a tag (starting with #)
Press right to go to next picture.
Press ESC to choose another account or tag
Press 1 to switch to MSX1 - Screen2.
Press 2 to switch to MSX2 - Screen8. (only if you have an MSX2, otherwise it will be ignored)

### WHAT IS NEEDED:
You need only the cartridge GR8NET. It doesn't matter what SETMAP it is configured.
The GR8NET has many features, but only 2 are being used in this project:
- BLOAD "HTTP"
- OPEN "HTTP"
Its needed also a server as a bridge (you can use mine: 54.201.73.96). You will understand why this is needed.

### HOW IT WORKS
MSX computers run at 3.56mhz and have about 25KB of memory available in Basic. Its virtually impossible for the MSX to perform the following tasks:
- connect into HTTPS (Instagram API)
- retrieve huge JSON files and parse the JSON
- download huge images from the Instagram API
- convert it to 16 colors (SCREEN 2) or 256 colors (SCREEN 8) low resolution
That's why a server is needed in the middle, to make all the heavy work.

### HOW IT WORKS (Detailed explanation)
1. BASIC generates a random number (5 digits) to be used as "session". Example 12345
2. BASIC loads the content of http://glufke.ddns.net:8080/instagr8.url . This file contains the URL of the current server. This way, in the case I change the server URL, it is automatic captured by the clients. Works like a DNS for the server.
3. BASIC ask the user what to show. It can be an INSTAGRAM USERNAME, or an INSTAGRAM TAG, starting with #.
4. BASIC open a HTTP request to the "Simplification Layer", I will call it SERVER. The HTTP request has the following structure:
`http://SERVER/instagr8/msxinsta.php?t=t&u=glufke&n=1&s=12345&v=2`
Being:

| Parameter | Description |
| ------ | ------ |
|u | the search string, i.e. msx
|s | the session ID, i.e. 12345
|t | this is the TYPE being "u" for username and "t" for tag (or hashtag)
|n | this is the number of the image being retrieved. (from 1 to ...)
|v | this is the version of the MSX (1 or 2). It's an optional parameter.

5. SERVER captures the request (I'm using Apache, but you can user NGINX or any other)
6. SERVER Apache calls the PHP script "msxinsta.php" with all the parameters.
7. PHP open one of these 2 URL's that belongs to Instagram
`https://www.instagram.com/someuser?__a=1`
`https://www.instagram.com/explore/tags/tagblablabla?__a=1`
8. PHP saves in a variable the JSON file retruned.
9. PHP parses the JSON file in one ARRAY.
10. PHP retrieves some basic information from the ARRAY returned, like name of the account. (It can bring many different information, like the number of followers, etc, but we don't have "space" in the MSX screen to show all these infos)
11. PHP loops into the ARRAY an retrieve the exact post we want. (this is passed by the parameter N). In other words, MSX just say "I want the Nth post of this user or tag".
12. PHP finds the Nth post and saves the "Description" and URL of the JPG image.
13. PHP calls a SHELL script to make the conversion to SCREEN2 or SCREEN8.
14. SHELL calls WGET to save URL with the JPG. (Saves with the name in12345.jpg, being 12345 the session number created in MSX)
15. SHELL calls IMAGEMAGICK to resize to 192x192 pixels (MSX1) or 256x212 (MSX2)
16. SHELL calls IMAGEMAGICK to add the "Instagr8" logo and MSX logo (it's a 64x192 image or 48x212) and saves it in a TGA image format)
17. SHELL calls the conversion C executable. (sc2 or sc8) previosuly compiled. This script is found [here](https://www.msx.org/downloads/related/graphics/screen-2-converter). (Thanks ARTRAG and Jannone)
18. For MSX1, the sc2.c creates 2 files. out12345.CHR and out12345.CLR. (one for pattern table &H0000 of VRAM and one for color table &H2000 of VRAM).
    For MSX2, the sc8.c creates just 1 file. out12345.CHR. (Starting at &H0000 to &HD3FF).
19. Back to PHP, truncates the "Description" of the image to 128 characters. (We don't have more space in the MSX screen. About 16 lines x 8 chars)
20. PHP sends back to MSX Basic the Description. (While the 2 files, CHR and CLR, are saved in the server)
21. BASIC retrieves the Description Content.
22. BASIC uses BLOAD "out1234.CHR" (and "out1234.CLR" for screen 2) to load into GR8NET internal memory. 
23. BASIC calls GR8NET CALL NETBTOV(0,0) to copy from internal GR8NET ram to VRAM.
24. BASIC loops the Description and prints the Description 8 chars at the time.

### SOURCES
The initial files are just a "proof of concept". Many things will be improved in the next days. Feel free to change it or send suggestions.

# Spritesheet separator

Separates spritesheets into images based on transparency

---
Before (17,000 x 9,000 spritesheet):

![before](https://raw.githubusercontent.com/GiantBooley/spritesheet-separator/refs/heads/main/before.png)
After (30 seconds c++ version 3 hours python version):

![after](https://raw.githubusercontent.com/GiantBooley/spritesheet-separator/refs/heads/main/afterdasd.png)
---
# Tutorial for python version (slow)
Step 1: Install Python\
Step 2: Open command prompt and type `cd C:\the\folder\with\the\spritesheet`\
Step 3: Type `pip install pillow`\
Step 4: Copy separator.py to the folder with the spritesheet and edit the settings in the file to what you need\
Step 5: In command prompt, type `python separator.py`\

# tutorial for c++ version (500x faster)
Step 1: Install g++ (https://github.com/niXman/mingw-builds-binaries/releases) (i used x86_64-14.2.0-release-posix-seh-ucrt-rt_v12-rev0.7z)\
step 2: Go to the folder with the atlaser.cpp file\
Step 3: Download stb_image.h and stb_image_write.h and put them both in include/stb folder.\
Step 4: Go command prompt and type `g++ atlaser.cpp -Iinclude -o atlaser.exe` (for windows)\
Step 5: Type `./atlaser` and it should show instructions on how to do it and stuff

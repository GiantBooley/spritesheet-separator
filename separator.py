#spritesheet separator v1 by giantbooley

from PIL import Image
import os

#settings start
spritesheetFileName = "spritesheet.png"
name = "Mushroom"
alphaThreshold = 60 # pixels with alpha below this number will be deleted (0-255)
floodFillMaxSteps = 10000 # max number of steps the flood fill algorithm uses
imageNumberStartFromOne = True #if true then the numbers on image filenames start from 1 instead of 0
#settings end

im = Image.open(spritesheetFileName)
width, height = im.size
alphaMask = Image.new("RGBA", (width, height));
px = im.load()
apx = alphaMask.load()

if not os.path.exists("output/" + name):
    os.makedirs("output/" + name)

# create alpha mask
for x in range(width):
    for y in range(height):
        if px[x, y][3] < alphaThreshold:
            apx[x, y] = (0, 0, 0, 255)
        else:
            apx[x, y] = (255, 0, 0, 255)
            
            
k = 1 if imageNumberStartFromOne else 0

for x in range(width):
    for y in range(height):
        if apx[x, y] == (255, 0, 0, 255):
            left = x
            right = x
            top = y
            bottom = y
            checkedPixels = Image.new("RGBA", (width, height), (0, 0, 0, 255))
            cpx = checkedPixels.load()
            cpx[x, y]
            floodPixelList = [(x, y)]
            for i in range(floodFillMaxSteps): #steps
                for j in range(len(floodPixelList) - 1, -1, -1): # for each flood pixel
                    if floodPixelList[j][0] > 0 and apx[floodPixelList[j][0] - 1, floodPixelList[j][1]] == (255, 0, 0, 255) and cpx[floodPixelList[j][0] - 1, floodPixelList[j][1]] == (0, 0, 0, 255):
                        floodPixelList.append((floodPixelList[j][0] - 1, floodPixelList[j][1]))
                        cpx[floodPixelList[j][0] - 1, floodPixelList[j][1]] = (255, 0, 0, 255)
                        apx[floodPixelList[j][0] - 1, floodPixelList[j][1]] = (0, 0, 0, 255)
                        
                    if floodPixelList[j][0] < width - 1 and apx[floodPixelList[j][0] + 1, floodPixelList[j][1]] == (255, 0, 0, 255) and cpx[floodPixelList[j][0] + 1, floodPixelList[j][1]] == (0, 0, 0, 255):
                        floodPixelList.append((floodPixelList[j][0] + 1, floodPixelList[j][1]))
                        cpx[floodPixelList[j][0] + 1, floodPixelList[j][1]] = (255, 0, 0, 255)
                        apx[floodPixelList[j][0] + 1, floodPixelList[j][1]] = (0, 0, 0, 255)
                        
                    if floodPixelList[j][1] > 0 and apx[floodPixelList[j][0], floodPixelList[j][1] - 1] == (255, 0, 0, 255) and cpx[floodPixelList[j][0], floodPixelList[j][1] - 1] == (0, 0, 0, 255):
                        floodPixelList.append((floodPixelList[j][0], floodPixelList[j][1] - 1))
                        cpx[floodPixelList[j][0], floodPixelList[j][1] - 1] = (255, 0, 0, 255)
                        apx[floodPixelList[j][0], floodPixelList[j][1] - 1] = (0, 0, 0, 255)
                        
                    if floodPixelList[j][1] < height - 1 and apx[floodPixelList[j][0], floodPixelList[j][1] + 1] == (255, 0, 0, 255) and cpx[floodPixelList[j][0], floodPixelList[j][1] + 1] == (0, 0, 0, 255):
                        floodPixelList.append((floodPixelList[j][0], floodPixelList[j][1] + 1))
                        cpx[floodPixelList[j][0], floodPixelList[j][1] + 1] = (255, 0, 0, 255)
                        apx[floodPixelList[j][0], floodPixelList[j][1] + 1] = (0, 0, 0, 255)
                    left = min([left, floodPixelList[j][0]])
                    right = max([right, floodPixelList[j][0]])
                    top = min([top, floodPixelList[j][1]])
                    bottom = max([bottom, floodPixelList[j][1]])
                    del floodPixelList[j]
            croppedObject = im.copy()
            copx = croppedObject.load()
            for xa in range(width):
                for ya in range(height):
                    if cpx[xa, ya] == (0, 0, 0, 255):
                        copx[xa, ya] = (0, 0, 0, 0)
            if left == right or top == bottom:
                continue
            croppedObject.crop((left, top, right, bottom)).save("output/" + name + "/" + name + str(k) + ".png")
            print("Saved \"" + "output/" + name + "/" + name + str(k) + ".png\"")
            k += 1

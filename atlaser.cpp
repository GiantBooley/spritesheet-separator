//spritesheet separator v5 by giantbooley

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>
#include <chrono>
using namespace std;
//from time import process_time


string secondsToTime(int seconds) {
    int hours = seconds / 3600;
    int minutes = (seconds / 60) % 60;
	seconds = seconds % 60;
    string out = "";
    if (hours > 0) out += to_string(hours) + "h ";
    if (minutes > 0) out += to_string(minutes) + "m ";
    out += to_string(seconds) + "s ";
    return out;
}
struct Vec2 {
	int x, y;
};

int main(int argc, char* argv[]) { // spritesheet, name, startnum, makedivisible, edgeMargin, alphathreshold, minsize
	if (argc != 8) {
		cout << "usage: atlaser [spritesheet file] [output image names] [start number] [makedivisiblebynumber] [edge margin] [alpha threshold] [min size]" << endl;
		cout << "if make divisible by number isnt 1 then it expands the image canvas so the width and height are divisible by that" << endl;
		cout << "alpha threshold is 0-255, any alpha below that is ignored" << endl;
		cout << "example: atlaser rockspritesheet.png Rock 1 4 2 25 10" << endl;
		return 0;
	}
	//settings start
	string spritesheetFileName = argv[1];
	string name = argv[2];
	int imageStartNumber = stoi(argv[3]); //image start number
	int makeDivisibleByNumber = stoi(argv[4]); // if true resize to size divisible by 4
	int edgeMargin = stoi(argv[5]); //pixel gap inbetween the image and edge of the image
	unsigned char alphaThreshold = (unsigned char)min(max(stoi(argv[6]), 0), 255); // pixels with alpha below this number will be deleted (0-255)
	int minWidthHeight = stoi(argv[7]); // minimum number for width and height for island to be valid
	//settings end

	int width, height, colorChannels;
	unsigned char* data = stbi_load(spritesheetFileName.c_str(), &width, &height, &colorChannels, 0);
	if (!data) {
		cerr << "ERROR: failed to load " << spritesheetFileName << endl;
		return 1;
	}

	string pathName = "./output/" + name;
	filesystem::path path{pathName.c_str()};
	if (!filesystem::is_directory(path)) {
		filesystem::create_directories(pathName.c_str());
	}

	unsigned char* currentIslandMask = (unsigned char*)malloc(sizeof(unsigned int) * width * height);
	for (int i = 0; i < width * height; i++) {
		currentIslandMask[i] = static_cast<unsigned char>(0);
	}
	int k = imageStartNumber;
	Vec2 checkOffsets[4] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
	
	auto start = chrono::high_resolution_clock::now();
	
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (data[(y * width + x) * 4 + 3] > alphaThreshold) { // check if has alpha at x y
				//cout << "starting island (" << x << ", " << y << ")" << endl;
				int left = x;
				int right = x;
				int top = y;
				int bottom = y;
				vector<Vec2> floodPixelList = {{x, y}}; // pixels on the edge of flood fill
				while (floodPixelList.size() > 0U) {
					for (int j = floodPixelList.size() - 1; j >= 0; j--) {// for each flood pixel
						int ecks = floodPixelList.at(j).x;
						int why = floodPixelList.at(j).y;
						floodPixelList.erase(floodPixelList.begin() + j);
						for (Vec2 offset : checkOffsets) {
							int newX = ecks + offset.x;
							int newY = why + offset.y;
							if (ecks > 0 && ecks < width - 1 && why > 0 && why < height - 1 &&
								data[(newY * width + newX) * 4 + 3] > alphaThreshold && // check if new position has alpha
								currentIslandMask[newY * width + newX] == static_cast<unsigned char>(0) // check if the pixel hasnt been added yet
							) { // check if flood pixel x isnt 0 and the pixel to the lfet of it has alpha and the pixel ot the left of it hasnt been checked
								floodPixelList.push_back({newX, newY});
								currentIslandMask[newY * width + newX] = static_cast<unsigned char>(255);
							}
						}

						left = min(left, ecks);
						right = max(right, ecks);
						top = min(top, why);
						bottom = max(bottom, why);
					}
				}
				if (right - left + 1 < minWidthHeight || bottom - top + 1 < minWidthHeight || left == right || top == bottom) {
					cout << "island is too small skipping (" << (right - left + 1) << "x" << (bottom - top + 1) << ")" << endl;
					continue;
				}
				//get size
				int finalLeft = left - edgeMargin;
				int finalRight = right + edgeMargin;
				int finalTop = top - edgeMargin;
				int finalBottom = bottom + edgeMargin;
				if (makeDivisibleByNumber > 1) {
					int finalWidth = finalRight - finalLeft + 1;
					int finalHeight = finalBottom - finalTop + 1;
					int horizontalAmountToAdd = makeDivisibleByNumber - (finalWidth % makeDivisibleByNumber);
					int verticalAmountToAdd = makeDivisibleByNumber - (finalHeight % makeDivisibleByNumber);
					int leftAmountToAdd = horizontalAmountToAdd / 2;
					int topAmoundToAdd = verticalAmountToAdd / 2;
					finalLeft -= leftAmountToAdd;
					finalRight += horizontalAmountToAdd - leftAmountToAdd;
					finalTop -= topAmoundToAdd;
					finalBottom += verticalAmountToAdd - topAmoundToAdd;
				}
				//copy object and remove island
				unsigned char* objectPixels = (unsigned char*)malloc(sizeof(unsigned char) * (finalRight - finalLeft + 1) * (finalBottom - finalTop + 1) * 4);
				for (int ya = 0; ya <= finalBottom - finalTop; ya++) {
					for (int xa = 0; xa <= finalRight - finalLeft; xa++) {
						bool isInsideData = xa + finalLeft >= 0 && xa + finalLeft < width && ya + finalTop >= 0 && ya + finalTop < height;
						unsigned char* objectPixel = objectPixels + (ya * (finalRight - finalLeft + 1) + xa) * 4;
						unsigned char* dataPixel = isInsideData ? (data + ((ya + finalTop) * width + xa + finalLeft) * 4) : nullptr;
						unsigned char* currentIslandMaskPixel = isInsideData ? (currentIslandMask + (ya + finalTop) * width + xa + finalLeft) : nullptr;

						objectPixel[0] = isInsideData ? dataPixel[0] : static_cast<unsigned char>(0);
						objectPixel[1] = isInsideData ? dataPixel[1] : static_cast<unsigned char>(0);
						objectPixel[2] = isInsideData ? dataPixel[2] : static_cast<unsigned char>(0);
						if (!isInsideData || currentIslandMask[(ya + finalTop) * width + xa + finalLeft] == static_cast<unsigned char>(0)) {
							objectPixel[3] = static_cast<unsigned char>(0);
						} else {
							objectPixel[3] = dataPixel[3];
						}
						if (isInsideData && *currentIslandMaskPixel == static_cast<unsigned char>(255)) {
							dataPixel[3] = static_cast<unsigned char>(0);
							*currentIslandMaskPixel = static_cast<unsigned char>(0);
						}
					}
				}

				string savedFileName = "output/" + name + "/" + name + to_string(k) + ".png";
				stbi_write_png(savedFileName.c_str(), finalRight - finalLeft + 1, finalBottom - finalTop + 1, 4, objectPixels, (finalRight - finalLeft + 1) * 4);
				free(objectPixels);
				float percent = (float)(y * width + x) / (float)(width * height) * 100.f;
				cout << "Saved \"" << savedFileName << "\" (" << (int)percent << "%)" << endl;
				k++;
			}
		}
	}
	auto stop = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
	cout << "finished separating in " << ((float)duration.count() / 1000.f) << " seconds" << endl;
	free(data);
	free(currentIslandMask);
	return 0;
}

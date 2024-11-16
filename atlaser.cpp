//spritesheet separator v7 by giantbooley

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>
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

	if (string(argv[1]) == "separate") {
		//settings start
		string spritesheetFileName = argv[2];
		string name = argv[3];
		int imageStartNumber = stoi(argv[4]); //image start number
		int makeDivisibleByNumber = stoi(argv[5]); // if true resize to size divisible by 4
		int edgeMargin = stoi(argv[6]); //pixel gap inbetween the image and edge of the image
		unsigned char alphaThreshold = (unsigned char)min(max(stoi(argv[7]), 0), 255); // pixels with alpha below this number will be deleted (0-255)
		int minWidthHeight = stoi(argv[8]); // minimum number for width and height for island to be valid
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
		int imageIndex = 1; //for printing progress
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
					//finishMultiplier = (width * height) / (y * width + x)
					//secondsRemaining = process_time() / imageIndex * (imageIndex * finishMultiplier - imageIndex)
					cout << "Saved \"" << savedFileName << "\"" << endl;
					//print("Saved \"" + "output/" + name + "/" + name + str(k) + ".png\" (" + str(imageIndex) + "/" + str(round(finishMultiplier * imageIndex)) + ", " + secondsToTime(secondsRemaining) + " remaining, " + secondsToTime(process_time()) + " elapsed)");
					//imageIndex += 1
					k++;
				}
			}
		}
		free(data);
		free(currentIslandMask);
	} else if (string(argv[1]) == "image") {
		string inputFolderName = argv[2];
		string outputFolderName = argv[3];
		unsigned char alphaThreshold = (unsigned char)min(max(stoi(argv[4]), 0), 255);
		int makeDivisibleByNumber = stoi(argv[5]);
		int edgeMargin = stoi(argv[6]);
		bool cropToObject = string(argv[7]) == "true";

		string inputPathName = "./" + inputFolderName;
		filesystem::path inputPath{inputPathName.c_str()};
		if (!filesystem::is_directory(inputPath)) {
			cout << "error: input path doesnt exist" << endl;
			return 1;
		}

		string outputPathName = "./" + outputPathName;
		filesystem::path outputPath{outputFolderName.c_str()};
		if (!filesystem::is_directory(outputPath)) {
			filesystem::create_directories(outputPathName.c_str());
		}

		for (const auto & entry : filesystem::directory_iterator(inputFolderName)) {
			int width, height, colorChannels;
			string path = entry.path().u8string();
			string fileName = path.substr(inputFolderName.length() + 1);
			unsigned char* data = stbi_load(path.c_str(), &width, &height, &colorChannels, 0);
			if (!data) {
				cerr << "ERROR: failed to load " << entry.path() << endl;
				continue;
			}
			unsigned char* alphaImage = new unsigned char[width * height * 4];
			for (int i = 0; i < width * height; i++) {
				for (int j = 0; j < 4; j++) {
					alphaImage[i * 4 + j] = (j < colorChannels) ? data[i * colorChannels + j] : static_cast<unsigned char>(255);
				}
				if (alphaImage[i * 4 + 3] <= alphaThreshold) alphaImage[i * 4 + 3] = 0;
			}
			// find bounds of object
			int topMost = 0;
			int bottomMost = height - 1;
			int leftMost = 0;
			int rightMost = width - 1;
			if (cropToObject) {
				for (int y = 0; y < height; y++) {
					for (int x = 0; x < width; x++) {
						if (alphaImage[(y * width + x) * 4 + 3] > 0) {
							topMost = y;
							goto bottommost;
						}
					}
				}
				bottommost:
				for (int y = height - 1; y >= 0; y--) {
					for (int x = 0; x < width; x++) {
						if (alphaImage[(y * width + x) * 4 + 3] > 0) {
							bottomMost = y;
							goto leftmost;
						}
					}
				}
				leftmost:
				for (int x = 0; x < width; x++) {
					for (int y = 0; y < height; y++) {
						if (alphaImage[(y * width + x) * 4 + 3] > 0) {
							leftMost = x;
							goto rightmost;
						}
					}
				}
				rightmost:
				for (int x = width - 1; x >= 0; x--) {
					for (int y = 0; y < height; y++) {
						if (alphaImage[(y * width + x) * 4 + 3] > 0) {
							rightMost = x;
							goto finishmosts;
						}
					}
				}
			}
			finishmosts:

			int finalLeft = leftMost - edgeMargin;
			int finalRight = rightMost + edgeMargin;
			int finalTop = topMost - edgeMargin;
			int finalBottom = bottomMost + edgeMargin;
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
			int finalWidth = finalRight - finalLeft + 1;
			int finalHeight = finalBottom - finalTop + 1;
			unsigned char* newPixels = new unsigned char[finalWidth * finalHeight * 4];

			for (int y = finalTop; y <= finalBottom; y++) {
				for (int x = finalLeft; x <= finalRight; x++) {
					unsigned char* pixel = &newPixels[((y - finalTop) * finalWidth + (x - finalLeft)) * 4];
					bool isInside = x >= 0 && y >= 0 && x < width && y < height;
					if (isInside) {
						unsigned char* regularPixel = &alphaImage[(y * width + x) * 4];
						pixel[0] = regularPixel[0];
						pixel[1] = regularPixel[1];
						pixel[2] = regularPixel[2];
						pixel[3] = regularPixel[3];
					} else {
						pixel[0] = 0;
						pixel[1] = 0;
						pixel[2] = 0;
						pixel[3] = 0;
					}
				}
			}

			string savedFileName = "./" + outputFolderName + "/" + fileName;
			cout << "Saved \"" << savedFileName << "\"" << endl;
			stbi_write_png(savedFileName.c_str(), finalWidth, finalHeight, 4, newPixels, finalWidth * 4);
			free(data);
			free(alphaImage);
			free(newPixels);
		}
	} else {
		cout << "webp and avif and some other formats dont work so you can convert them with imagemagick to png or jpeg" << endl;
		cout << "FOR SPRITESHEET SEPARATION===============" << endl;
		cout << "usage: atlaser separate [spritesheet file] [output image names] [start number] [makedivisiblebynumber] [edge margin] [alpha threshold] [min size]" << endl;
		cout << "if make divisible by number isnt 1 then it expands the image canvas so the width and height are divisible by that" << endl;
		cout << "alpha threshold is 0-255, any at or alpha below that is ignored" << endl;
		cout << "example: atlaser separate rockspritesheet.png Rock 1 4 2 25 10" << endl;
		cout << endl;
		cout << "FOR SINGLE IMAGES===============" << endl;
		cout << "usage: atlaser image [input folder] [output folder] [alpha threshold] [makedivisiblebynumber] [edge margin] [crop to object true/false]" << endl;
		cout << "this processes every image in input folder and saves an image with the same name into output folder" << endl;
		cout << "alpha threshold is 0-255, any alpha at or below that is ignored" << endl;
		cout << "if make divisible by number isnt 1 then it expands the image canvas so the width and height are divisible by that" << endl;
		cout << "if crop to object is enabled then it removes the edges of the image that have no alpha" << endl;
		cout << "example: atlaser image 50 input output 4 2 true" << endl;
		cout << endl;
	}
	return 0;
}

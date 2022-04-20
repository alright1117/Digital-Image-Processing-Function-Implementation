#ifndef SRC_RGBCOMPRESSOR_H_
#define SRC_RGBCOMPRESSOR_H_

# include "opencv2/highgui/highgui.hpp"
# include "opencv2/imgproc/imgproc.hpp"
# include <iostream>
# include <string>
# include <fstream>
# include <exception>
# include <cmath>
# include "huffman.h"

class rgbCompressor {
	Huffman h;

	cv::Mat bgrImage;//Original image converted to monochromatic

	cv::Mat Y;
	cv::Mat Cb;
	cv::Mat Cr;

	cv::Mat dctY;
	cv::Mat dctCb;
	cv::Mat dctCr;

	int height, h2m8;
	int width, w2m8;

	float quality;
	float quantFactor;

	int* zigZagMask; // A mask to zig-zag search

	std::string* shiftedBit;//Mask for shift bits in string
	unsigned int numberBitsShifted;

	cv::Mat luminance;//Quantization Matrix for luminance
	cv::Mat chrominance;

	std::priority_queue<node> frequenceTable;//Frequence table for Huffman Algorithm

	std::ofstream outfile; //To write in .compactado
	std::ifstream infile;  //To read from .compactado

	std::string codifiedImage;  //Image after huffman codification
	std::string fileName;		//Name of the file .compactado

	std::vector<float> codeTable;

public:
	rgbCompressor();
	virtual ~rgbCompressor();

	void quantLumMat();
	void quantChromMat();

	void setParamCompression(char* imageName);
	void setParamDecompression(char* imageName);

	void bgrToYCbCr();
	void yCbCrToBgr();

	void forwardDCT();	// Forward transformation - DCT
	void inverseDCT();	// Inverse transformation - iDCT

	void displayImage(std::string imageName, cv::Mat image);// Display all images

	void compress(char* imageName);//Execute Huffman to create a table of symbols and codify the image to a file
	void decompress(char* imageName);

	void searchFrequenceTable();// Search the frequency table and set it in huffman's codification.
	void readCodeTable();		// Read from .compactado the frequency table and set it in huffman's codification.

	void readCodifiedImage();	// Read the image from .compactado.
};

#endif /* SRC_RGBCOMPRESSOR_H_ */


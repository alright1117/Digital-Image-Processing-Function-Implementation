#include "rgbCompressor.h"

rgbCompressor::rgbCompressor() {
	//Mask to shiftting a bit in string
	static std::string bitsMask[10] = { "", "0", "00", "000", "0000", "00000", "000000", "0000000", "00000000" };
	shiftedBit = bitsMask;

	static int zzMask[64] = {
			  0,  1,  8, 16,  9,  2, 3,  10,
			  17, 24, 32, 25, 18, 11,  4,  5,
			  12, 19, 26, 33, 40, 48, 41, 34,
			  27, 20, 13,  6,  7, 14, 21, 28,
			  35, 42, 49, 56, 57, 50, 43, 36,
			  29, 22, 15, 23, 30, 37, 44, 51,
			  58, 59, 52, 45, 38, 31, 39, 46,
			  53, 60, 61, 54, 47, 55, 62, 63 };
	zigZagMask = zzMask;

	//Create the luminance matrix for quantization
	luminance = (cv::Mat_<float>(8, 8) <<
		16, 11, 10, 16, 24, 40, 51, 61,
		12, 12, 14, 19, 26, 58, 60, 55,
		14, 13, 16, 24, 40, 57, 69, 56,
		14, 17, 22, 29, 51, 87, 80, 62,
		18, 22, 37, 56, 68, 109, 103, 77,
		24, 35, 55, 64, 81, 104, 113, 92,
		49, 64, 78, 87, 103, 121, 120, 101,
		72, 92, 95, 98, 112, 100, 103, 99);

	chrominance = (cv::Mat_<float>(8, 8) <<
		17, 18, 24, 47, 99, 99, 99, 99,
		18, 21, 26, 66, 99, 99, 99, 99,
		24, 26, 56, 99, 99, 99, 99, 99,
		47, 66, 99, 99, 99, 99, 99, 99,
		99, 99, 99, 99, 99, 99, 99, 99,
		99, 99, 99, 99, 99, 99, 99, 99,
		99, 99, 99, 99, 99, 99, 99, 99,
		99, 99, 99, 99, 99, 99, 99, 99);

	quality = 50; // 50 ~ 100
	quantFactor = 64.f;

	quantLumMat();

	width = 0;
	height = 0;
	w2m8 = 0;
	h2m8 = 0;
	numberBitsShifted = 0;
}

void rgbCompressor::quantLumMat() {
	quality = quality / 100;

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			luminance.at<float>(i, j) = std::ceil(luminance.at<float>(i, j) * quality);

		}
	}
}

void rgbCompressor::quantChromMat() {
	quality = quality / 100;

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			chrominance.at<float>(i, j) = std::ceil(luminance.at<float>(i, j) * quality);

		}
	}
}

void rgbCompressor::setParamCompression(char* imageName) {
	bgrImage = cv::imread(imageName);

	//Check if image exist
	if (!bgrImage.data) {
		std::cout << "\033[1;31mCan't load the image. Please insert the image address.\033[0m\n" << std::endl;
		throw std::exception();
	}


	// Modify the image names to .compactado
	fileName.clear();
	fileName.append(imageName);
	fileName.insert(7, "result/");
	fileName.erase(fileName.end() - 4, fileName.end());
	fileName += ".compactado"; //sufix

	//Open output streams to .compactado
	outfile.open(fileName.c_str());

	// Transform the image resolutions to a resolution with size multiple of 8 by 8
	// Necessary to broke the image in 8 by 8 blocks in DCT step
	height = bgrImage.cols;
	width = bgrImage.rows;

	h2m8 = (height % 8) ? (h2m8 = 8 - height % 8) : 0;
	w2m8 = (width % 8) ? (w2m8 = 8 - width % 8) : 0;

	if (h2m8 != 0) {
		cv::Mat hBlock = bgrImage(cv::Rect(0, height - h2m8, width, h2m8));
		cv::vconcat(bgrImage, hBlock, bgrImage);
		height += h2m8;
	}

	if (w2m8 != 0) {
		cv::Mat wBlock = bgrImage(cv::Rect(width - w2m8, 0, w2m8, height));
		cv::hconcat(bgrImage, wBlock, bgrImage);
		width += w2m8;
	}

	// Write the image's size in .compactado
	outfile << height << " " << width << std::endl;
	outfile << h2m8 << " " << w2m8 << std::endl;

}

void rgbCompressor::bgrToYCbCr() {

	Y = cv::Mat_<float>(height, width);
	Cb = cv::Mat_<float>(height / 2, width / 2);
	Cr = cv::Mat_<float>(height / 2, width / 2);

	for (int i = 0; i < bgrImage.rows; i++) {
		for (int j = 0; j < bgrImage.cols; j++) {
			//get bgr pixel 
			cv::Vec3b bgrPixel = bgrImage.at<cv::Vec3b>(i, j);

			float B = bgrPixel[0];
			float G = bgrPixel[1];
			float R = bgrPixel[2];

			//actual conversion from BGR to YCrCb
			float delta = 0.5f;
			float yPixel = 0.299f * R + 0.587f * G + 0.114f * B;
			Y.at<float>(i, j) = yPixel;

			if ((i % 2 == 0) && (j % 2 == 0)) {
				float CbPixel = (B - yPixel) * 0.564f + delta;
				float CrPixel = (R - yPixel) * 0.713f + delta;
				Cb.at<float>(i / 2, j / 2) = CbPixel;
				Cr.at<float>(i / 2, j / 2) = CrPixel;
			}

		}
	}
}

void rgbCompressor::yCbCrToBgr() {

	bgrImage.create(height, width, CV_32FC3);

	float CbPixel;
	float CrPixel;

	for (int i = 0; i < bgrImage.rows; i++) {
		for (int j = 0; j < bgrImage.cols; j++) {

			//actual conversion from BGR to YCrCb
			float delta = 0.5f;
			float yPixel = Y.at<float>(i, j);
			
			if ((i % 2 == 0) && (j % 2 == 0)) {
				CbPixel = Cb.at<float>(i / 2, j / 2);
				CrPixel = Cr.at<float>(i / 2, j / 2);
			}
			
			float B = (CbPixel - delta) / 0.564f + yPixel;
			float R = (CrPixel - delta) / 0.713f + yPixel;
			float G = 1.f / 0.587f * (yPixel - 0.299f * R - 0.114f * B);
			cv::Vec3f bgrPixel(B, G, R);

			bgrImage.at<cv::Vec3f>(i, j) = bgrPixel;
			
		}
	}

	bgrImage.convertTo(bgrImage, CV_8UC3);
}

// Forward transformation - DCT
void rgbCompressor::forwardDCT() {

	dctY = cv::Mat_<uchar>(height, width);
	dctCb = cv::Mat_<uchar>(height / 2, width / 2);
	dctCr = cv::Mat_<uchar>(height / 2, width / 2);

	for (int i = 0; i < height; i += 8) {
		for (int j = 0; j < width; j += 8) {
			// Get a block 8x8 for each position on image
			cv::Mat block8 = Y(cv::Rect(j, i, 8, 8));

			// Convert block from 8 bits to 64 bits
			block8.convertTo(block8, CV_32FC1);

			// DCT
			cv::dct(block8, block8);

			// Quantization step
			cv::divide(block8, luminance, block8);

			// Adding factor to the block
			cv::add(block8, quantFactor, block8);

			// Converting it back to unsigned char
			block8.convertTo(block8, CV_8UC1);

			cv::Mat blockC = cv::Mat_<uchar>(8, 8);

			for (int k = 0; k < (int)8 * 8; k++)
				blockC.at<uchar>(k) = block8.at<uchar>(zigZagMask[k]);
			//std::cout << "Copyto\n";
			blockC.copyTo(dctY(cv::Rect(j, i, 8, 8)));


		}
	}

	for (int i = 0; i < height / 2; i += 8) {
		for (int j = 0; j < width / 2; j += 8) {
			// Get a block 8x8 for each position on image
			cv::Mat blockCb = Cb(cv::Rect(j, i, 8, 8));
			cv::Mat blockCr = Cr(cv::Rect(j, i, 8, 8));

			// DCT
			cv::dct(blockCb, blockCb);
			cv::dct(blockCr, blockCr);

			// Quantization step
			cv::divide(blockCb, chrominance, blockCb);
			cv::divide(blockCr, chrominance, blockCr);

			// Adding to the block
			cv::add(blockCb, quantFactor, blockCb);
			cv::add(blockCr, quantFactor, blockCr);

			// Converting it back to unsigned char
			blockCb.convertTo(blockCb, CV_8UC1);
			blockCr.convertTo(blockCr, CV_8UC1);

			cv::Mat zzBlockCb = cv::Mat_<uchar>(8, 8);
			cv::Mat zzBlockCr = cv::Mat_<uchar>(8, 8);

			for (int k = 0; k < (int)8 * 8; k++) {
				zzBlockCb.at<uchar>(k) = blockCb.at<uchar>(zigZagMask[k]);
				zzBlockCr.at<uchar>(k) = blockCr.at<uchar>(zigZagMask[k]);
			}

			//std::cout << "Copyto\n";
			zzBlockCb.copyTo(dctCb(cv::Rect(j, i, 8, 8)));
			zzBlockCr.copyTo(dctCr(cv::Rect(j, i, 8, 8)));
		}
	}
}



void rgbCompressor::inverseDCT() {
	
	Y = cv::Mat_<float>(height, width);
	Cb = cv::Mat_<float>(height / 2, width / 2);
	Cr = cv::Mat_<float>(height / 2, width / 2);

	for (int i = 0; i < height; i += 8) {
		for (int j = 0; j < width; j += 8) {
			// Get a block 8x8 for each position on image
			cv::Mat block = dctY(cv::Rect(j, i, 8, 8));
			block.convertTo(block, CV_32FC1);

			// Subtracting the block by factor
			cv::subtract(block, quantFactor, block);

			cv::Mat block8 = cv::Mat::zeros(8, 8, CV_32FC1);
			for (int k = 0; k < (int)8 * 8; k++)
				block8.at<float>(zigZagMask[k]) = block.at<float>(k);

			// Quantization step
			cv::multiply(block8, luminance, block8);

			// Inverse DCT
			cv::idct(block8, block8);

			// Copying the block back to the new dct image
			//std::cout << "Copyto\n";
			block8.copyTo(Y(cv::Rect(j, i, 8, 8)));
		}
	}
	
	for (int i = 0; i < height / 2; i += 8) {
		for (int j = 0; j < width / 2; j += 8) {
			// Get a block 8x8 for each position on image
			cv::Mat blockCb = dctCb(cv::Rect(j, i, 8, 8));
			cv::Mat blockCr = dctCr(cv::Rect(j, i, 8, 8));
			blockCb.convertTo(blockCb, CV_32FC1);
			blockCr.convertTo(blockCr, CV_32FC1);

			// Subtracting the block by 128
			cv::subtract(blockCb, quantFactor, blockCb);
			cv::subtract(blockCr, quantFactor, blockCr);

			cv::Mat izblockCb = cv::Mat::zeros(8, 8, CV_32FC1);
			cv::Mat izblockCr = cv::Mat::zeros(8, 8, CV_32FC1);

			for (int k = 0; k < (int)8 * 8; k++) {
				izblockCb.at<float>(zigZagMask[k]) = blockCb.at<float>(k);
				izblockCr.at<float>(zigZagMask[k]) = blockCr.at<float>(k);
			}

			// Quantization step
			cv::multiply(izblockCb, chrominance, izblockCb);
			cv::multiply(izblockCr, chrominance, izblockCr);

			// Inverse DCT
			cv::idct(izblockCb, izblockCb);
			cv::idct(izblockCr, izblockCr);

			// Copying the block back to the new dct image
			//std::cout << "Copyto\n";
			izblockCb.copyTo(Cb(cv::Rect(j, i, 8, 8)));
			izblockCr.copyTo(Cr(cv::Rect(j, i, 8, 8)));
		}
	}

}


// Display all images
void rgbCompressor::displayImage(std::string imageName, cv::Mat image) {
	/// Display
	cv::imshow(imageName, image);
	cv::waitKey(0);
}



// Compute the histogram matrix for dct image in planes
void rgbCompressor::searchFrequenceTable() {
	std::vector<float> f(256, 0.0);
	
	for (int i = 0; i < height; i ++) {
		for (int j = 0; j < width; j ++) {
			f[dctY.at<uchar>(i, j)] += 1;

			if ((i % 2 == 0) && (j % 2 == 0)) {
				f[dctCb.at<uchar>(i / 2, j / 2)] += 1;
				f[dctCr.at<uchar>(i / 2, j / 2)] += 1;
			}
		}
	}

	h.setFrequenceTable(f);
}



//Compressor step
void rgbCompressor::compress(char* imageName) {
	//Just for information
	std::cout << "\033[0;32mCompressing...\033[0m\n";

	std::cout << "\033[0;32mSetting Parameter...\033[0m\n";
	setParamCompression(imageName);
	bgrToYCbCr();

	std::cout << "\033[0;32mTransforming Image...\033[0m\n";
	forwardDCT();

	std::cout << "\033[0;32mSearching Frequency Table...\033[0m\n";
	searchFrequenceTable();

	std::cout << "\033[0;32mComputing Symbol Table...\033[0m\n";
	codeTable = h.getHuffmanCode();

	//Write the Frequency Table in the file
	std::cout << "\033[0;32mPrinting Frequency Table...\033[0m\n";
	for (unsigned short i = 0; i < codeTable.size(); i++) {
		if (codeTable.at(i) == 0) continue;
		outfile << i << " " << codeTable.at(i) << std::endl;
	}

	//transform image Mat to vector
	std::vector<int> inputFile;
	inputFile.assign(dctY.datastart, dctY.dataend);
	for (int i = 0; i < height / 2; i++) {
		for (int j = 0; j < width / 2; j++) {
			inputFile.insert(inputFile.end(), (int)dctCb.at<uchar>(i, j));
		}
	}

	for (int i = 0; i < height / 2; i++) {
		for (int j = 0; j < width / 2; j++) {
			inputFile.insert(inputFile.end(), (int)dctCr.at<uchar>(i, j));
		}
	}
	//Codify image dct based on table of code from Huffman
	std::cout << "\033[0;32mEncoding Image with Huffman...\033[0m\n";
	codifiedImage = h.encode(inputFile);

	//Add 0's in the end of the file to complete a byte
	numberBitsShifted = 8 - codifiedImage.length() % 8;
	codifiedImage += shiftedBit[numberBitsShifted];


	//End of the code table and the number of bits 0 shifted in the end of the file
	outfile << "#" << numberBitsShifted << std::endl;

	std::cout << "\033[0;32mPrinting Image...\033[0m\n";
	size_t imSize = codifiedImage.size();
	for (size_t i = 0; i < imSize; i += 8) {
		outfile << (int)std::strtol(codifiedImage.substr(i, 8).c_str(), 0, 2) << std::endl;
	}
	codifiedImage.clear();

	//Close the file .compactado
	outfile.close();

	//Just for information
	if (!codifiedImage.length())
		std::cout << "\033[0;32mCompressing Successful!\033[0m\n";
	else
		std::cout << "\033[0;31mCompressing Failed!\033[0m\n";

}









void rgbCompressor::setParamDecompression(char* imageName) {
	fileName.clear();
	fileName.append(imageName);

	if (fileName.substr(fileName.length() - 11, fileName.length()) != ".compactado") {
		std::cout << "\033[0;31mCan't load the image. Please insert the image address.\033[0m\n" << std::endl;
		throw std::exception();
	}
	fileName.erase(fileName.length() - 11, fileName.length());
	fileName += ".saida.png";

	infile.open(imageName);
	if (!infile.is_open()) {
		std::cout << "\033[0;31mCan't load the image. Please insert the image address.\033[0m\n" << std::endl;
		throw std::exception();
	}

	std::string size;//Get the original size of the image
	infile >> size;
	height = std::atoi(size.c_str());
	infile >> size;
	width = std::atoi(size.c_str());

	infile >> size;
	h2m8 = std::atoi(size.c_str());
	infile >> size;
	w2m8 = std::atoi(size.c_str());
}


// Read the symbol table
void rgbCompressor::readCodeTable() {

	std::string ind, frequency;
	infile >> ind;

	while (ind[0] != '#') {
		infile >> frequency;
		h.setFrequenceTable(std::atoi(ind.c_str()), std::strtof(frequency.c_str(), 0));
		infile >> ind;
	}
	ind.erase(0, 1);//remove '#'
	numberBitsShifted = std::atol(ind.c_str());
}



void rgbCompressor::readCodifiedImage() {
	int c;
	int mask = 128;//1000 0000
	//codifiedImage.clear();

	int count = 0;

	//infile >> std::noskipws;
	//infile >> c;

	while (infile >> c) {
		for (int i = 0; i < 8; i++) {

			if ((c << i) & mask) {
				codifiedImage.push_back('1');
			}
			else {
				codifiedImage.push_back('0');
			}
			count++;
		}
	}
	codifiedImage.erase(codifiedImage.length() - numberBitsShifted, numberBitsShifted);//remove the exceed bits
}



//Uncompress step
void rgbCompressor::decompress(char* fileName) {
	
	//Just for information
	std::cout << "\033[0;32mUncompressing...\033[0m\n";

	std::cout << "\033[0;32mSetting Parameters...\033[0m\n";
	setParamDecompression(fileName);

	std::cout << "\033[0;32mReading Symbol Table ...\033[0m\n";
	readCodeTable();

	std::cout << "\033[0;32mReading Codified Image ...\033[0m\n";
	readCodifiedImage();

	std::cout << "\033[0;32mDecoding Image with Huffman...\033[0m\n";
	std::vector<int> dctFile = h.decode(codifiedImage);

	dctY = cv::Mat::zeros(height, width, CV_8UC1);
	dctCb = cv::Mat::zeros(height / 2, width / 2, CV_8UC1);
	dctCr = cv::Mat::zeros(height / 2, width / 2, CV_8UC1);

	std::cout << "\033[0;32mTransforming Image...\033[0m\n";
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			dctY.at<uchar>(i, j) = (uchar)dctFile.at(i * dctY.step + j);
		}
	}

	for (int i = 0; i < height / 2; i++) {
		for (int j = 0; j < width / 2; j++) {
			dctCb.at<uchar>(i, j) = (uchar)dctFile.at(height * width + i * dctCb.step + j);
		}
	}

	for (int i = 0; i < height / 2; i++) {
		for (int j = 0; j < width / 2; j++) {
			dctCr.at<uchar>(i, j) = (uchar)dctFile.at(height * width * 5 / 4 + i * dctCr.step + j);
		}
	}

	std::cout << "\033[0;32mProcessing Image...\033[0m\n";
	inverseDCT();

	yCbCrToBgr();
	if (w2m8 != 0)
		bgrImage = bgrImage.colRange(0, width - w2m8);

	if (h2m8 != 0)
		bgrImage = bgrImage.rowRange(0, height - h2m8);

	cv::imwrite(this->fileName, bgrImage);
	std::cout << "\033[0;32mDone!\033[0m\n";
}




rgbCompressor::~rgbCompressor() {}
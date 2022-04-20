#include <string>
#include <iostream>
#include <opencv2/core/core.hpp>
#include"dft.h"
#include"utils.h"
using namespace cv;

int main(int argc, const char** argv)
{
	const char* refPath = "data/Noisy.raw";
	Mat img = read_raw_img(refPath);
	
	Mat planes[] = { Mat_<float>(img), Mat::zeros(img.size(), CV_32F) };
	Mat complexImg;

	merge(planes, 2, complexImg); 

	DFT_trans(img, complexImg);

	std::cout << "complex" << complexImg.rows << " " << complexImg.cols << std::endl;																										  // compute log(1 + sqrt(Re(DFT(img))**2 + Im(DFT(img))**2))
	split(complexImg, planes); 
	magnitude(planes[0], planes[1], planes[0]);
	Mat mag = planes[0];
	mag += Scalar::all(1);
	log(mag, mag);
	normalize(mag, mag, 0, 1, NORM_MINMAX);
	mag *= 255;
	Mat mag8UC;
	mag.convertTo(mag8UC, CV_8UC1);

	//const char* filename = "result/Noisy.txt";
	//writeMatToFile(planes[0], filename);
	Mat centMat = visualizedMat(complexImg);

	// Inverse DFT
	Mat invPlanes[] = { Mat_<float>(img), Mat::zeros(img.size(), CV_32F) };
	Mat invComplexImg;
	merge(planes, 2, invComplexImg);
	IDFT_trans(complexImg, invComplexImg);
	split(invComplexImg, invPlanes);
	Mat test = invPlanes[0];
	normalize(test, test, 0, 1, NORM_MINMAX);

	imwrite("result/DFT/NoisyMag.png", mag8UC);
	imwrite("result/DFT/NoisyMagC.png", centMat);

	imshow("spectrum magnitude", mag8UC);
	imshow("cent spectrum magnitude", centMat);
	imshow("Inverse img", test);

	waitKey();
	system("pause");
	return 0;
}
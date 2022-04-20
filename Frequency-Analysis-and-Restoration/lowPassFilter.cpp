#include <string>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <math.h> 
#include"dft.h"
#include"utils.h"


using namespace cv;


cv::Mat addGaussianNoise(const cv::Mat_<float>& src, int mean, int variance) {

	Mat_<float> noiseImage;
	src.copyTo(noiseImage);
	Mat noise(src.size(), src.type());
	cv::randn(noise, mean, variance); //mean and variance
	noiseImage += noise;
	Mat finalImage;
	noiseImage.convertTo(finalImage, CV_8UC1);

	return finalImage.clone();
}

cv::Mat addUniformNoise(const cv::Mat_<float>& src, int min, int max) {

	Mat_<float> noiseImage;
	src.copyTo(noiseImage);
	Mat noise(src.size(), src.type());
	cv::randu(noise, min, max); //mean and variance
	noiseImage += noise;
	Mat finalImage;
	noiseImage.convertTo(finalImage, CV_8UC1);

	return finalImage.clone();
}

float distance(int m, int n, int x, int y)
{
	float dist = sqrt(pow(m - x, 2) + pow(n - y, 2));
	return dist;
}

Mat ideaLowPassFilter(Mat complexImg)
{
	Mat filterImg = complexImg.clone();

	int m = complexImg.rows / 2;
	int n = complexImg.cols / 2;

	int r = m / 2;

	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			float dist = distance(0, 0, i, j);
			if (dist > r)
			{
				filterImg.at<Vec2f>(i, j)[0] = 0.0;
				filterImg.at<Vec2f>(i, j)[1] = 0.0;
			}
		}
	}

	for (int i = m; i < 2 * m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			float dist = distance(2 * m, 0, i, j);
			if (dist > r)
			{
				filterImg.at<Vec2f>(i, j)[0] = 0.0;
				filterImg.at<Vec2f>(i, j)[1] = 0.0;
			}
		}
	}

	for (int i = 0; i < m; i++)
	{
		for (int j = n; j < 2 * n; j++)
		{
			float dist = distance(0, 2 * n, i, j);
			if (dist > r)
			{
				filterImg.at<Vec2f>(i, j)[0] = 0.0;
				filterImg.at<Vec2f>(i, j)[1] = 0.0;
			}
		}
	}

	for (int i = m; i < 2 * m; i++)
	{
		for (int j = n; j < 2 * n; j++)
		{
			float dist = distance(2 * m, 2 * n, i, j);
			if (dist > r)
			{
				filterImg.at<Vec2f>(i, j)[0] = 0.0;
				filterImg.at<Vec2f>(i, j)[1] = 0.0;
			}
		}
	}

	return filterImg.clone();
}

Mat gaussianLowPassFilter(Mat complexImg, int kernelSize, float stdDev)
{
	Mat kernel = gaussKernel(kernelSize, stdDev);
	Mat paddedKernel = Mat::zeros(complexImg.size(), CV_32F);

	for (int i = 0; i < kernel.rows; i++)
	{
		for (int j = 0; j < kernel.cols; j++)
		{
			paddedKernel.at<float>(i, j) = kernel.at<float>(i, j);
		}
	}

	Mat temp[] = { Mat_<float>(paddedKernel), Mat::zeros(paddedKernel.size(), CV_32F) };
	Mat complexKernel;
	merge(temp, 2, complexKernel);
	DFT_trans(paddedKernel, complexKernel);

	Mat filterImg = complexImg.clone();

	for (int i = 0; i < filterImg.rows; i++)
	{
		for (int j = 0; j < filterImg.cols; j++)
		{
			filterImg.at<Vec2f>(i, j)[0] = complexImg.at<Vec2f>(i, j)[0] * complexKernel.at<Vec2f>(i, j)[0] - \
										   complexImg.at<Vec2f>(i, j)[1] * complexKernel.at<Vec2f>(i, j)[1];

			filterImg.at<Vec2f>(i, j)[1] = complexImg.at<Vec2f>(i, j)[1] * complexKernel.at<Vec2f>(i, j)[0] + \
										   complexImg.at<Vec2f>(i, j)[0] * complexKernel.at<Vec2f>(i, j)[1];
		}

	}

	return filterImg.clone();
}

int main(int argc, const char** argv)
{
	const char* refPath = "data/baboon.raw";
	Mat img = read_raw_img(refPath);
	
	// add noise
	Mat noiseImg = addUniformNoise(img, -30, 30);
	//Mat noiseImg = addGaussianNoise(img, 0, 30);

	Mat planes[] = { Mat_<float>(noiseImg), Mat::zeros(noiseImg.size(), CV_32F) };
	Mat complexImg;

	merge(planes, 2, complexImg);
	DFT_trans(noiseImg, complexImg);

	Mat filterComplexImg = ideaLowPassFilter(complexImg);

	Mat mag = visualizedMat(complexImg);
	Mat filterMag = visualizedMat(filterComplexImg);

	double min, max;
	minMaxIdx(mag, &min, &max);
	mag = mag / max * 255;
	filterMag = filterMag / max * 255;

	Mat mag8UC; Mat filterMag8UC;
	mag.convertTo(mag8UC, CV_8UC1); filterMag.convertTo(filterMag8UC, CV_8UC1);

	// Inverse DFT
	Mat invPlanes[] = { Mat_<float>(img), Mat::zeros(img.size(), CV_32F) };
	Mat invComplexImg;
	merge(invPlanes, 2, invComplexImg);
	IDFT_trans(filterComplexImg, invComplexImg);
	split(invComplexImg, invPlanes);
	Mat finalImg = invPlanes[0];
	Mat finalImg8UC;
	finalImg.convertTo(finalImg8UC, CV_8UC1);

	// save Image
	imwrite("result/lowpass/uniformNoise/idealLowPass/baboon/30_noise.png", noiseImg);
	imwrite("result/lowpass/uniformNoise/idealLowPass/baboon/30_mag.png", mag8UC);
	imwrite("result/lowpass/uniformNoise/idealLowPass/baboon/30_filterMag.png", filterMag8UC);
	imwrite("result/lowpass/uniformNoise/idealLowPass/baboon/30_filterImg.png", finalImg8UC);


	// show Image
	imshow("Original image", img);
	imshow("Noise image", noiseImg);
	imshow("Noise spectrum magnitude", mag8UC);
	imshow("After denoise", filterMag8UC);
	imshow("Inverse img", finalImg8UC);

	waitKey();
	system("pause");

	return 0;
}
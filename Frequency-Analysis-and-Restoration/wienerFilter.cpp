#include <string>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <math.h> 
#include"dft.h"
#include"utils.h"

using namespace cv;


float distance(int m, int n, int x, int y)
{
	float dist = sqrt(pow(m - x, 2) + pow(n - y, 2));
	return dist;
}


Mat wienierFilter(Mat complexImg)
{
	Mat resultImg = complexImg.clone();

	int m = complexImg.rows / 2;
	int n = complexImg.cols / 2;

	int r = m / 2;

	int s = 1;
	int k = 2;
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			float dist = distance(0, 0, i, j);
			if (dist > s)
			{
				resultImg.at<Vec2f>(i, j)[0] /= dist / (sqrt(dist) + k);
				resultImg.at<Vec2f>(i, j)[1] /= dist / (sqrt(dist) + k);
			}
			else
			{
				resultImg.at<Vec2f>(i, j)[0] /= s;
				resultImg.at<Vec2f>(i, j)[1] /= s;
			}
		}
	}

	for (int i = m; i < 2 * m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			float dist = distance(2 * m, 0, i, j);
			if (dist > s)
			{
				resultImg.at<Vec2f>(i, j)[0] /= dist / (sqrt(dist) + k);
				resultImg.at<Vec2f>(i, j)[1] /= dist / (sqrt(dist) + k);
			}
			else
			{
				resultImg.at<Vec2f>(i, j)[0] /= s;
				resultImg.at<Vec2f>(i, j)[1] /= s;
			}
		}
	}

	for (int i = 0; i < m; i++)
	{
		for (int j = n; j < 2 * n; j++)
		{
			float dist = distance(0, 2 * n, i, j);
			if (dist > s)
			{
				resultImg.at<Vec2f>(i, j)[0] /= dist / (sqrt(dist) + k);
				resultImg.at<Vec2f>(i, j)[1] /= dist / (sqrt(dist) + k);
			}
			else
			{
				resultImg.at<Vec2f>(i, j)[0] /= s;
				resultImg.at<Vec2f>(i, j)[1] /= s;
			}
		}
	}

	for (int i = m; i < 2 * m; i++)
	{
		for (int j = n; j < 2 * n; j++)
		{
			float dist = distance(2 * m, 2 * n, i, j);
			if (dist > s)
			{
				resultImg.at<Vec2f>(i, j)[0] /= dist / (sqrt(dist) + k);
				resultImg.at<Vec2f>(i, j)[1] /= dist / (sqrt(dist) + k);
			}
			else
			{
				resultImg.at<Vec2f>(i, j)[0] /= s;
				resultImg.at<Vec2f>(i, j)[1] /= s;
			}
		}
	}

	return resultImg.clone();
}

int main(int argc, const char** argv)
{
	const char* refPath = "data/Noisy.raw";
	Mat img = read_raw_img(refPath);

	Mat planes[] = { Mat_<float>(img), Mat::zeros(img.size(), CV_32F) };
	Mat complexImg;

	merge(planes, 2, complexImg);
	DFT_trans(img, complexImg);

	Mat filterComplexImg = wienierFilter(complexImg);

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

	// save image
	imwrite("result/denoise/wiener/mag_k2.png", mag8UC);
	imwrite("result/denoise/wiener/filterMag_k2.png", filterMag8UC);
	imwrite("result/denoise/wiener/filterImg_k2.png", finalImg8UC);

	imshow("Original image", img);
	imshow("Noise spectrum magnitude", mag8UC);
	imshow("After denoise", filterMag8UC);
	imshow("Inverse img", finalImg8UC);

	waitKey();
	system("pause");
	return 0;
}
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <math.h>
#include "utils.h"

#define PI 3.1415926

using namespace cv;

void DCT_trans(Mat img, Mat out)
{
	int N = img.rows;
	Mat imageRow(img.size(), CV_32FC1);

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			float tmp = 0.0;
			float Ci;
			if (j == 0)
			{
				Ci = sqrt(0.5);
			}
			else
			{
				Ci = 1;
			}

			for (int k = 0; k < N; k++)
			{
				float theta = j * PI * (2 * k + 1) / (2 * N);
				tmp += img.at<float>(i, k) * cos(theta);
			}

			imageRow.at<float>(i, j) = sqrt(2.0) / sqrt(N) * Ci * tmp;
		}
	}

	for (int j = 0; j < N; j++)
	{
		for (int i = 0; i < N; i++)
		{
			float tmp = 0.0;
			float Ci;
			if (i == 0)
			{
				Ci = sqrt(0.5);
			}
			else
			{
				Ci = 1;
			}

			for (int k = 0; k < N; k++)
			{
				float theta = i * PI * (2 * k + 1) / (2 * N);
				tmp += imageRow.at<float>(k, j) * cos(theta);
			}

			out.at<float>(i, j) = sqrt(2.0) / sqrt(N) * Ci * tmp;
		}
	}
}

void IDCT_trans(Mat img, Mat out)
{
	int N = img.rows;
	Mat imageRow(img.size(), CV_32FC1);

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			float tmp = 0.0;
			float Ci;
			if (i == 0)
			{
				Ci = sqrt(0.5);
			}
			else
			{
				Ci = 1;
			}

			for (int k = 0; k < N; k++)
			{
				float theta = k * PI * (2 * j + 1) / (2 * N);
				tmp += img.at<float>(i, k) * cos(theta);
			}

			imageRow.at<float>(i, j) = sqrt(2.0) / sqrt(N) * Ci * tmp;
		}
	}

	for (int j = 0; j < N; j++)
	{
		for (int i = 0; i < N; i++)
		{
			float tmp = 0.0;
			float Ci;
			if (j == 0)
			{
				Ci = sqrt(0.5);
			}
			else
			{
				Ci = 1;
			}

			for (int k = 0; k < N; k++)
			{
				float theta = k * PI * (2 * i + 1) / (2 * N);
				tmp += imageRow.at<float>(k, j) * cos(theta);
			}

			out.at<float>(i, j) = sqrt(2.0) / sqrt(N) * Ci * tmp;
		}
	}
}

float distance(int m, int n, int x, int y)
{
	float dist = sqrt(pow(m - x, 2) + pow(n - y, 2));
	return dist;
}

void lowPassFilter(Mat DCTimage, Mat filterImg)
{
	int m = DCTimage.rows;
	int n = DCTimage.cols;

	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			float dist = distance(0, 0, i, j);
			if (dist <= 1)
			{
				filterImg.at<float>(i, j) = DCTimage.at<float>(i, j) / 1.2;
			}
			else
			{
				filterImg.at<float>(i, j) = DCTimage.at<float>(i, j) / sqrt(dist) * 2;
			}
		}
	}
}

int main(int argc, const char** argv)
{
	const char* refPath = "data/Noisy.raw";
	Mat img = read_raw_img(refPath);

	Mat floatImage;
	img.convertTo(floatImage, CV_32FC1);

	Mat DCTimage(img.size(), CV_32FC1);
	Mat filterImage(img.size(), CV_32FC1);
	Mat finalImage(img.size(), CV_32FC1);

	DCT_trans(floatImage, DCTimage);
	lowPassFilter(DCTimage, filterImage);
	IDCT_trans(filterImage, finalImage);

	Mat mag = DCTimage.clone();
	mag = abs(mag);
	log(mag, mag);
	normalize(mag, mag, 0, 1, NORM_MINMAX);
	mag *= 255;

	Mat magfilter = filterImage.clone();

	magfilter = abs(magfilter);
	log(magfilter, magfilter);
	normalize(magfilter, magfilter, 0, 1, NORM_MINMAX);
	magfilter *= 255;

	Mat mag8UC; Mat filterMag8UC;
	mag.convertTo(mag8UC, CV_8UC1); magfilter.convertTo(filterMag8UC, CV_8UC1);

	Mat finalImg8UC;
	finalImage.convertTo(finalImg8UC, CV_8UC1);

	imshow("Img", img);
	imshow("Mag", mag8UC);

	imshow("Magfilter", filterMag8UC);
	imshow("final img", finalImg8UC);

	// save Image
	imwrite("result/DCT/mag.png", mag8UC);
	imwrite("result/DCT/filterMag.png", filterMag8UC);
	imwrite("result/DCT/filterImg.png", finalImg8UC);

	waitKey();
	system("pause");
	return 0;
}
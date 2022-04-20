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

Mat ideaHighPassFilter(Mat complexImg)
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
			if (dist < r)
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
			if (dist < r)
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
			if (dist < r)
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
			if (dist < r)
			{
				filterImg.at<Vec2f>(i, j)[0] = 0.0;
				filterImg.at<Vec2f>(i, j)[1] = 0.0;
			}
		}
	}

	return filterImg.clone();
}

Mat butterWorthFilter(Mat complexImg, float D, int p)
{
	int m = complexImg.rows / 2;
	int n = complexImg.cols / 2;

	Mat filterImg = complexImg.clone();

	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			float dist = distance(0, 0, i, j);

			float Hreal = 1 - 1 / (1 + pow(dist / D, 2 * p));
			float Himage = 1 - 1 / (1 + pow(dist / D, 2 * p));
			filterImg.at<Vec2f>(i, j)[0] = Hreal * complexImg.at<Vec2f>(i, j)[0];
			filterImg.at<Vec2f>(i, j)[1] = Himage * complexImg.at<Vec2f>(i, j)[1];
		}
	}

	for (int i = m; i < 2 * m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			float dist = distance(2 * m, 0, i, j);

			float Hreal = 1 - 1 / (1 + pow(dist / D, 2 * p));
			float Himage = 1 - 1 / (1 + pow(dist / D, 2 * p));
			filterImg.at<Vec2f>(i, j)[0] = Hreal * complexImg.at<Vec2f>(i, j)[0];
			filterImg.at<Vec2f>(i, j)[1] = Himage * complexImg.at<Vec2f>(i, j)[1];
		}
	}

	for (int i = 0; i < m; i++)
	{
		for (int j = n; j < 2 * n; j++)
		{
			float dist = distance(0, 2 * n, i, j);

			float Hreal = 1 - 1 / (1 + pow(dist / D, 2 * p));
			float Himage = 1 - 1 / (1 + pow(dist / D, 2 * p));
			filterImg.at<Vec2f>(i, j)[0] = Hreal * complexImg.at<Vec2f>(i, j)[0];
			filterImg.at<Vec2f>(i, j)[1] = Himage * complexImg.at<Vec2f>(i, j)[1];
		}
	}

	for (int i = m; i < 2 * m; i++)
	{
		for (int j = n; j < 2 * n; j++)
		{
			float dist = distance(2 * m, 2 * n, i, j);

			float Hreal = 1 - 1 / (1 + pow(dist / D, 2 * p));
			float Himage = 1 - 1 / (1 + pow(dist / D, 2 * p));
			filterImg.at<Vec2f>(i, j)[0] = Hreal * complexImg.at<Vec2f>(i, j)[0];
			filterImg.at<Vec2f>(i, j)[1] = Himage * complexImg.at<Vec2f>(i, j)[1];
		}
	}

	return filterImg.clone();
}

int main(int argc, const char** argv)
{
	const char* refPath = "data/Noisy.raw";
	Mat img = read_raw_img(refPath);

	Mat planes[] = { Mat_<float>(img), Mat::zeros(img.size(), CV_32F) };
	Mat complexImg;

	merge(planes, 2, complexImg);
	DFT_trans(img, complexImg);

	Mat filterComplexImg = butterWorthFilter(complexImg, 100, 2);

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
	Mat finalImg = abs(invPlanes[0]);
	normalize(finalImg, finalImg, 0, 1, NORM_MINMAX);
	finalImg *= 255;

	Mat finalImg8UC;
	finalImg.convertTo(finalImg8UC, CV_8UC1);

	// save Image
	imwrite("result/highpass/butterworth/Noisy/mag_d100_r2.png", mag8UC);
	imwrite("result/highpass/butterworth/Noisy/filterMag_d100_r2.png", filterMag8UC);
	imwrite("result/highpass/butterworth/Noisy/filterImg_d100_r2.png", finalImg8UC);
	
	// show Image
	imshow("Original image", img);
	imshow("Noise spectrum magnitude", mag8UC);
	imshow("After filter", filterMag8UC);
	imshow("Inverse img", finalImg8UC);

	waitKey();
	system("pause");
	return 0;
}
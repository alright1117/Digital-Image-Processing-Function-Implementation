#include <opencv2/core/core.hpp>
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;

cv::Mat read_raw_img(const char* refPath)
{
	int height = 512;
	int width = 512;

	FILE* fp = NULL;
	fp = fopen(refPath, "rb+");
	uchar* data = (uchar*)malloc(width * height * sizeof(uchar));
	fread(data, sizeof(uchar), width * height, fp);
	cv::Mat img;
	int buflen = width * height;
	img.create(height, width, CV_8UC1);
	memcpy(img.data, data, buflen * sizeof(uchar));

	return img.clone();
}

void writeMatToFile(cv::Mat& m, const char* filename)
{
    ofstream fout(filename);

    if (!fout)
    {
        cout << "File Not Opened" << endl;  return;
    }

    for (int i = 0; i < m.rows; i++)
    {
        for (int j = 0; j < m.cols; j++)
        {
            fout << m.at<float>(i, j) << "\t";
        }
        fout << endl;
    }

    fout.close();
}

cv::Mat visualizedMat(Mat complexImg)
{
	Mat planes[] = { Mat_<float>(complexImg), Mat::zeros(complexImg.size(), CV_32F)};

	split(complexImg, planes);
	magnitude(planes[0], planes[1], planes[0]);
	Mat mag = planes[0];

	mag += Scalar::all(1);
	log(mag, mag);
	mag = mag(Rect(0, 0, mag.cols & (-2), mag.rows & (-2)));

	// centerized
	int cx = mag.cols / 2;
	int cy = mag.rows / 2;

	std::cout << cx << " " << cy << std::endl;
	Mat tmp;
	Mat q0(mag, Rect(0, 0, cx, cy));
	Mat q1(mag, Rect(cx, 0, cx, cy));
	Mat q2(mag, Rect(0, cy, cx, cy));
	Mat q3(mag, Rect(cx, cy, cx, cy));

	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);

	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);

	/*
	normalize(mag, mag, 0, 1, NORM_MINMAX);
	mag *= 255;
	Mat result;
	mag.convertTo(result, CV_8UC1);
	*/
	return mag.clone();
}

cv::Mat gaussKernel(int kernelSize, float stdDev)
{
	float sum_term{ 0 };
	cv::Mat kernel = cv::Mat::zeros(kernelSize, kernelSize, CV_32F);


	// Create kernel
	for (int i = -kernelSize / 2; i <= kernelSize / 2; i++)
	{
		for (int j = -kernelSize / 2; j <= kernelSize / 2; j++)
		{
			kernel.at<float>(i + kernelSize / 2, j + kernelSize / 2) = std::exp(-(pow(i, 2) + pow(j, 2)) / (2 * pow(stdDev, 2)));
			sum_term += kernel.at<float>(i + kernelSize / 2, j + kernelSize / 2);
		}
	}

	kernel /= sum_term;

	return kernel.clone();
}

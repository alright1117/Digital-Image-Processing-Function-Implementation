#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>

using namespace cv;
using namespace std; 

cv::Mat read_raw_img(const char *refPath) {
	int height = 512;
	int width = 512;

	FILE* fp = NULL;
	fp = fopen(refPath, "rb+");
	uchar* data = (uchar*)malloc(width * height * sizeof(uchar));
	fread(data, sizeof(uchar), width* height, fp);
	cv::Mat img;
	int buflen = width * height;
	img.create(height, width, CV_8UC1);
	memcpy(img.data, data, buflen * sizeof(uchar));

	return img;
}

void gammaCorrection(const Mat& src, Mat& dst, const float gamma)
{
    float invGamma = 1 / gamma;

    Mat table(1, 256, CV_8U);
    uchar* p = table.ptr();
    for (int i = 0; i < 256; ++i) {
        p[i] = (uchar)(pow(i / 255.0, invGamma) * 255);
    }

    LUT(src, table, dst);
}

//function to take histogram of input image
void imageHistogram(Mat image, int histogram[]) {
    // initialize all intensity values to 0
    for (int i = 0; i < 256; i++) {
        histogram[i] = 0;
    }
    // calculate the number of pixels for each intensity value
    for (int y = 0; y < image.rows; y++) {
        for (int x = 0; x < image.cols; x++) {
            histogram[(int)image.at<uchar>(y, x)]++;
        }
    }
}
//function to take calculate cumulative histogram of input image
void cumulativeImageHistogram(int histogram[], int cumulativeHistogram[]) {
    cumulativeHistogram[0] = histogram[0];
    for (int i = 1; i < 256; i++) {
        cumulativeHistogram[i] = histogram[i] + cumulativeHistogram[i - 1];
    }
}
//function to display histogram
void histogramDisplay(int histogram[], const char* name) {
    int newHistogram[256];
    for (int i = 0; i < 256; i++) {
        newHistogram[i] = histogram[i];
    }
    //histogram size
    int histogramWidth = 640;
    int histogramHeight = 500;
    //creating "bins" for the range of 256 intensity values
    int binWidth = cvRound((double)histogramWidth / 256);
    Mat histogramImage(histogramHeight, histogramWidth, CV_8UC1, Scalar(255, 255, 255));
    //finding maximum intensity level in the histogram
    int maximumIntensity = newHistogram[0];
    for (int i = 1; i < 256; i++) {
        if (maximumIntensity < newHistogram[i]) {
            maximumIntensity = newHistogram[i];
        }
    }
    //normalizing histogram in terms of rows (y)
    for (int i = 0; i < 256; i++) {
        newHistogram[i] = ((double)newHistogram[i] / maximumIntensity) * histogramImage.rows;
    }
    //drawing the intensity level - line
    for (int i = 0; i < 256; i++) {
        line(histogramImage, Point(binWidth * (i), histogramHeight), Point(binWidth * (i), histogramHeight - newHistogram[i]), Scalar(0, 0, 0), 1, 8, 0);
    }
    // display
    namedWindow(name, CV_WINDOW_AUTOSIZE);
    imshow(name, histogramImage);
}

int main() {
    const char *refPath = "data/peppers.raw";
    Mat image = read_raw_img(refPath);
    Mat gammaImg;
    gammaCorrection(image, gammaImg, 0.3);

    //Call function to create histogram
    int histogram[256];
    imageHistogram(gammaImg, histogram);
    //Get the image size
    int size = gammaImg.rows * gammaImg.cols;
    float alpha = 255.0 / size;
    //Probability distribution for intensity levels
    float PRk[256];
    for (int i = 0; i < 256; i++) {
        PRk[i] = (double)histogram[i] / size;
    }
    //Call function to create cumulative histogram
    int cumulativeHistogram[256];
    cumulativeImageHistogram(histogram, cumulativeHistogram);
    //Scaling operation
    int Sk[256];
    for (int i = 0; i < 256; i++) {
        Sk[i] = cvRound((double)cumulativeHistogram[i] * alpha);
    }
    //Initializing equalized histogram
    float PSk[256];
    for (int i = 0; i < 256; i++) {
        PSk[i] = 0;
    }
    //Mapping operation
    for (int i = 0; i < 256; i++) {
        PSk[Sk[i]] += PRk[i];
    }
    //Rounding to get final values
    int finalValues[256];
    for (int i = 0; i < 256; i++) {
        finalValues[i] = cvRound(PSk[i] * 255);
    }
    //Creating equalized image
    Mat finalImage = gammaImg.clone();

    for (int y = 0; y < gammaImg.rows; y++) {
        for (int x = 0; x < gammaImg.cols; x++) {
            finalImage.at<uchar>(y, x) = saturate_cast<uchar>(Sk[gammaImg.at<uchar>(y, x)]);
        }
    }
    //Displaying source image
    namedWindow("Original Image");
    imshow("Original Image", image);
    //Displaying source image histogram
    histogramDisplay(histogram, "Original Histogram");

    namedWindow("Gamma Image");
    imshow("Gamma Image", gammaImg);

    //Displaying histogram equalized image
    namedWindow("Equilized Image");
    imshow("Equilized Image", finalImage);
    //Displaying equalized image histogram
    histogramDisplay(finalValues, "Equilized Histogram");
    //Saving image as a file
    imwrite("result/historgramEqualization/peppersGamma.png", gammaImg);
    imwrite("result/historgramEqualization/peppers.png", finalImage);
    waitKey(0);
    return 0;
}
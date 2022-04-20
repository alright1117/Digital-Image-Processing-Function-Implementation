#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>

using namespace cv;
using namespace std;

cv::Mat read_raw_img(const char* refPath) {
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

void historgramMatching(Mat image, Mat refImage, Mat resultImage) {
    int size = image.rows * image.cols;
    int refSize = refImage.rows * refImage.cols;
    float alpha = 255.0 / size;
    float refAlpha = 255.0 / refSize;

    int histogram[256];
    int refHistogram[256];
    imageHistogram(image, histogram);
    imageHistogram(refImage, refHistogram);

    int cumulativeHistogram[256];
    int refCumulativeHistogram[256];
    cumulativeImageHistogram(histogram, cumulativeHistogram);
    cumulativeImageHistogram(refHistogram, refCumulativeHistogram);

    Mat table(1, 256, CV_8U);
    uchar* p = table.ptr();
    for (int i = 0; i < 256; i++) {
        p[i] = (uchar)cvRound((double)cumulativeHistogram[i] * alpha);
    }

    int refSk[256];
    for (int i = 0; i < 256; i++) {
        refSk[i] = (uchar)cvRound((double)refCumulativeHistogram[i] * refAlpha);
    }
    Mat refInverseTable(1, 256, CV_8U);
    uchar* refp = refInverseTable.ptr();
    int tmp;
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            if (refSk[j] == i) {
                refp[i] = (uchar)j;
                tmp = j;              
                break;
            }
            else {
                refp[i] = (uchar)tmp;
            }
        }
    }

    
    Mat tmpImage = resultImage.clone();
    LUT(image, table, tmpImage);
    LUT(tmpImage, refInverseTable, resultImage);
}

int main() {
    const char* imgPath = "data/F16.raw";
    const char* refPath = "data/F16.raw";

    Mat image = read_raw_img(imgPath);
    Mat refImage = read_raw_img(refPath);

    Mat gammaImg;
    gammaCorrection(image, gammaImg, 0.3);

    Mat resultImage = image.clone();

    historgramMatching(gammaImg, refImage, resultImage);
    //Displaying source image
    namedWindow("Original Image");
    imshow("Original Image", image);

    namedWindow("Gamma Image");
    imshow("Gamma Image", gammaImg);
    //Displaying histogram equalized image
    namedWindow("Equilized Image");
    imshow("Equilized Image", resultImage);

    imwrite("result/historgramMatching/F16Gamma.png", gammaImg);
    imwrite("result/historgramMatching/F16.png", resultImage);
    //Saving image as a file
    //imwrite("EqualizedImage.jpg", finalImage);
    waitKey();
    return 0;
}
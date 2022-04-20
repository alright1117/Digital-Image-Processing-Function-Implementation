#include <iostream>
#include <string>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

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

Mat paddingImage(Mat image, int paddingSize) {
    Mat paddedImage;
    paddedImage.create(image.rows + 2 * paddingSize, image.cols + 2 * paddingSize, image.type());
    paddedImage.setTo(cv::Scalar::all(0));
    image.copyTo(paddedImage(Rect(paddingSize, paddingSize, image.cols, image.rows)));

    return paddedImage;
}

//function to take calculate cumulative histogram of input image
void cumulativeImageHistogram(int histogram[], int cumulativeHistogram[]) {
    cumulativeHistogram[0] = histogram[0];
    for (int i = 1; i < 256; i++) {
        cumulativeHistogram[i] = histogram[i] + cumulativeHistogram[i - 1];
    }
}


void localHistorgramEqualization(Mat resultImage, Mat paddedImage, int windowSize) {
    int histogram[256];
    int size = windowSize * windowSize;
    int cumulativeHistogram[256];
    int Sk[256];
    float alpha = 255.0 / size;

    for (int i = 0; i < 256; i++) {
        histogram[i] = 0;
    }

    for (int y = 0; y < paddedImage.rows - (2 * (windowSize / 2)); y++) {
        for (int x = 0; x < paddedImage.cols - (2 * (windowSize / 2)); x++) {
            for (int wy = 0; wy < windowSize; wy++) {
                for (int wx = 0; wx < windowSize; wx++) {
                    histogram[(int)paddedImage.at<uchar>(y + wy, x + wx)]++;
                }
            }
            
            cumulativeImageHistogram(histogram, cumulativeHistogram);
            for (int i = 0; i < 256; i++) {
                Sk[i] = cvRound((double)cumulativeHistogram[i] * alpha);
            }

            resultImage.at<uchar>(y, x) = saturate_cast<uchar>(Sk[paddedImage.at<uchar>(y + windowSize / 2, x + windowSize / 2)]);
            
            memset(histogram, 0, sizeof(histogram));
        }
    }
}
int main() {
    const char* refPath = "data/peppers.raw";
    int windowSize = 101;
    int paddingSize = windowSize / 2;
    Mat image = read_raw_img(refPath);
    Mat gammaImg;
    gammaCorrection(image, gammaImg, 0.3);

    Mat paddedImage = paddingImage(gammaImg, paddingSize);
    Mat resultImage = image.clone();

    localHistorgramEqualization(resultImage, paddedImage, windowSize);

    namedWindow("Original Image");
    imshow("Original Image", gammaImg);
    //Displaying histogram equalized image
    namedWindow("Equilized Image");
    imshow("Equilized Image", resultImage);
    imwrite("result/localHistorgramEqualization/peppersGamma.png", gammaImg);
    imwrite("result/localHistorgramEqualization/peppers.png", resultImage);
    waitKey(0);

    return 0;
}
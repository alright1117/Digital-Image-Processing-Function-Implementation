#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core.hpp"

#include <iostream>
#include <cmath>

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

    return img.clone();
}

Mat paddingImage(Mat image, int paddingSize)
{
    Mat paddedImage;
    paddedImage.create(image.rows + 2 * paddingSize, image.cols + 2 * paddingSize, image.type());
    paddedImage.setTo(cv::Scalar::all(0));
    image.copyTo(paddedImage(Rect(paddingSize, paddingSize, image.cols, image.rows)));

    return paddedImage.clone();
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

cv::Mat_<float> spatialConvolution(const cv::Mat_<float>& src, const cv::Mat_<float>& kernel)
{
    int y = src.rows - 2 * (kernel.rows - 2);
    int x = src.cols - 2 * (kernel.cols - 2);

    Mat dst(y, x, src.type());

    for (int i = 0; i < y; i++)
    {
        for (int j = 0; j < x; j++)
        {
            float tmp = 0.0f;
            for (int k = 0; k < kernel.rows; k++)
            {
                for (int l = 0; l < kernel.cols; l++)
                {
                    tmp += src.at<float>(i + k, j + l) * kernel.at<float>(k, l);
                }
            }
            dst.at<float>(i, j) = saturate_cast<float>(tmp);
        }
    }
    return dst.clone();
}

double SpaceFactor(int x1, int y1, int x2, int y2, double sigmaC) {
    double absX = pow(abs(x1 - x2), 2);
    double absY = pow(abs(y1 - y2), 2);

    return exp(-(absX + absY) / (2 * pow(sigmaC, 2)));
}

double ColorFactor(int x, int y, double sigmaS) {
    double distance = abs(x - y) / sigmaS;
    return exp(-0.5 * pow(distance, 2));
}

Mat BilateralFilter(const cv::Mat src, int kernelSize, double sigmaC, double sigmaS) {
    int len; //must be odd number
    len = kernelSize / 2;

    Mat dst(src.rows, src.cols, src.type());

    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            double sum = 0;
            double n = 0;
            for (int k = i - len; k <= i + len; k++) {
                for (int l = j - len; l <= j + len; l++) {
                    if (k < 0 || l < 0 || k >= src.rows || l >= src.cols)
                        continue;
                    sum += src.at<uchar>(k, l) * SpaceFactor(i, j, k, l, sigmaC) * ColorFactor(src.at<uchar>(i, j), src.at<uchar>(k, l), sigmaS);
                    n += SpaceFactor(i, j, k, l, sigmaC) * ColorFactor(src.at<uchar>(i, j), src.at<uchar>(k, l), sigmaS);
                }
            }
            double value = sum / n;
            dst.at<uchar>(i, j) = saturate_cast<uchar>(value);
        }
    }
    return dst.clone();
}

int main() {
    cout << "Applying Bilateral Filter." << endl;

    int kernelSize;
    cout << "Please set filter size:" << endl;
    scanf("%d", &kernelSize);

    double sigmaC, sigmaS;
    cout << "Please set sigmaC:" << endl;
    scanf("%lf", &sigmaC);
    cout << "Please set sigmaS:" << endl;
    scanf("%lf", &sigmaS);

    string path;
    cout << "Please input image path:" << endl;
    cin >> path;
    const char* refPath = path.c_str();

    Mat image = read_raw_img(refPath);

    cout << "Bluring image? yes:1, no:0." << endl;
    string userInput;
    cin >> userInput;

    Mat blurImage;
    if (userInput == "1")
    {
        int blurKernelSize;
        cout << "Please set blur filter size:" << endl;
        scanf("%d", &blurKernelSize);
        int paddingSize = kernelSize / 2;
        Mat paddedImage = paddingImage(image, paddingSize);
        Mat floatImage;
        paddedImage.convertTo(floatImage, CV_32FC1);
        float stdDev = 1; // gaussian filter std
        Mat kernel = gaussKernel(kernelSize, stdDev);
        Mat blurImageFloat = spatialConvolution(floatImage, kernel);
        blurImageFloat.convertTo(blurImage, CV_8UC1);

    }
    else
    {
        blurImage = image;
    }

    Mat finalImage = BilateralFilter(blurImage, kernelSize, sigmaC, sigmaS);

    //Displaying source image
    namedWindow("Original Image");
    imshow("Original Image", image);

    if (userInput == "1")
    {
        namedWindow("Blur Image");
        imshow("Blur Image", blurImage);
    }
    //Displaying histogram equalized image
    namedWindow("Final Image");
    imshow("Final Image", finalImage);
    imwrite("result/bilateralFilter/result.png", blurImage);

    waitKey();
    return 0;
}
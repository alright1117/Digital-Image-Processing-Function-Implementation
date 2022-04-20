#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>
#include <cmath>
#include <iomanip>

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

cv::Mat getKernel(int kernel) {

    if (kernel == 1)
    {
        float kernelWeight[9] = { -1, 0, -1, 0, 6, 0, -1, 0, -1 };
        Mat kernel = cv::Mat(3, 3, CV_32F, kernelWeight);
        return kernel.clone();
    }
    else
    {
        float kernelWeight[9] = {1, 0, 4, 2, 5, 2, 1, 0, 4};
        Mat kernel = cv::Mat(3, 3, CV_32F, kernelWeight);
        for (int i = 0; i < kernel.rows; i++)
        {
            for (int j = 0; j < kernel.cols; j++)
            {
               kernel.at<float>(i, j) /= 25;
            }
        }

        return kernel.clone();
    }
}

cv::Mat_<float> spatialConvolution(const cv::Mat_<float>& src, const cv::Mat_<float>& kernel)
{
    int y = src.rows - (2 * (kernel.rows / 2));
    int x = src.cols - (2 * (kernel.cols / 2));
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

int main() {

    int kernelSize = 3;
    int paddingSize = kernelSize / 2;

    string path;
    cout << "Please input image path:" << endl;
    cin >> path;
    const char* refPath = path.c_str();

    Mat image = read_raw_img(refPath);
    Mat paddedImage = paddingImage(image, paddingSize);
    Mat floatImage;
    paddedImage.convertTo(floatImage, CV_32FC1);

    int mode;
    cout << "Please choose the filter type:" << endl;
    scanf("%d", &mode);

    Mat kernel = getKernel(mode);

    cout << "The kernel is:   " << endl;
    for (int i = 0; i < kernel.rows; i++)
    {
        cout << "    ";
        for (int j = 0; j < kernel.cols; j++)
        {
            cout << kernel.at<float>(i, j) << ", ";
        }
        cout << endl;
    }

    Mat finalImageFloat = spatialConvolution(floatImage, kernel);
    Mat finalImage;
    finalImageFloat.convertTo(finalImage, CV_8UC1);

    //Displaying source image
    namedWindow("Original Image");
    imshow("Original Image", image);

    //Displaying histogram equalized image
    namedWindow("Final Image");
    imshow("Final Image", finalImage);
    imwrite("result/convolution2/result.png", finalImage);

    waitKey();
    return 0;

}
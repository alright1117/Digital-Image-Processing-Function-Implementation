#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <vector>
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

cv::Mat_<float> medianFilter(const cv::Mat_<float>& src, int kernelSize)
{
    int y = src.rows - (2 * (kernelSize / 2));
    int x = src.cols - (2 * (kernelSize / 2));

    Mat dst(y, x, src.type());

    for (int i = 0; i < y; i++)
    {
        const int dataSize = kernelSize * kernelSize;
        for (int j = 0; j < x; j++)
        {
            vector<float> data;
            for (int k = 0; k < kernelSize; k++)
            {
                for (int l = 0; l < kernelSize; l++)
                {
                    float tmp = src.at<float>(i + k, j + l);
                    data.push_back(tmp);
                }
            }

            std::sort(data.begin(), data.end());

            //cv::sort(filter, sortArr, CV_SORT_EVERY_ROW + CV_SORT_ASCENDING);

            if (kernelSize % 2 == 0)
            {
                int idx1 = dataSize / 2;
                int idx2 = dataSize / 2 + 1;
                float temp = (data[idx1] + data[idx2]) / 2;
                dst.at<float>(i, j) = saturate_cast<float>(temp);
            }
            else 
            {
                int idx = dataSize / 2 + 1;
                float temp = data[idx];
                
                dst.at<float>(i, j) = saturate_cast<float>(temp);
            }
        }
    }
    return dst.clone();
}

int main() {

    int kernelSize;
    cout << "Please set filter size:" << endl;
    scanf("%d", &kernelSize);
    int paddingSize = kernelSize / 2;

    string path;
    cout << "Please input image path:" << endl;
    cin >> path;
    const char* refPath = path.c_str();

    Mat image = read_raw_img(refPath);
    Mat paddedImage = paddingImage(image, paddingSize);
    Mat floatImage;
    paddedImage.convertTo(floatImage, CV_32FC1);

    Mat finalImageFloat = medianFilter(floatImage, kernelSize);
    Mat finalImage;
    finalImageFloat.convertTo(finalImage, CV_8UC1);

    //Displaying source image
    namedWindow("Original Image");
    imshow("Original Image", image);

    //Displaying histogram equalized image
    namedWindow("Final Image");
    imshow("Final Image", finalImage);
    imwrite("result/medianFilter/result.png", finalImage);

    waitKey();
    return 0;
}

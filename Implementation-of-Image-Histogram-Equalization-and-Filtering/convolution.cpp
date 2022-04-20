#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>
#include <cmath>
#include <iomanip>
#define PI 3.1415

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
// compute sigma^2 that 'fit' the kernel half width 
float compute_squared_variance(int half_width, float epsilon = 0.001)
{
    assert(0 < epsilon && epsilon < 1); // small value required
    return -(half_width + 1.0) * (half_width + 1.0) / 2.0 / std::log(epsilon);
}

float gaussian_exp(float y, float x, float sigma2)
{
    assert(0 < sigma2);
    return std::exp(-(x * x + y * y) / (2 * sigma2));
}

// create a Gaussian kernel of size 2*half_width+1 x 2*half_width+1
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

// create a Averaging filter
cv::Mat averagingKernel(int kernelSize)
{
    Mat kernel(kernelSize, kernelSize, CV_32F, Scalar(1 / pow((double)kernelSize, 2)));
    return kernel.clone();
}

//
cv::Mat laplacianKernel(int kernelSize) {

    if (kernelSize == 3)
    {
        float kernelWeight[9] = { -1, -1, -1, -1, 8, -1, -1, -1, -1 };
        Mat kernel = cv::Mat(3, 3, CV_32F, kernelWeight);
        return kernel.clone();
    }
    else if (kernelSize == 5)
    {
        float kernelWeight[25] = { 0, 0, -1, 0, 0, 0, -1, -2, -1, 0, -1, -2, 16, -2, -1, 0, -1, -2, -1, 0, 0, 0, -1, 0 ,0};
        Mat kernel = cv::Mat(5, 5, CV_32F, kernelWeight);
        return kernel.clone();
    }
    else if (kernelSize == 7)
    {
        float kernelWeight[49] = { 0, 0, 0, -1, 0, 0, 0,
                                   0, 0, 0, -2, 0, 0, 0,
                                   0, 0, -2, -4, -2, 0, 0,
                                  -1, -2, -4, 36, -4, -2, -1,
                                   0, 0, -2, -4, -2, 0, 0,
                                   0, 0, 0, -2, 0, 0, 0,
                                   0, 0, 0, -1, 0, 0, 0 };
        Mat kernel = cv::Mat(7, 7, CV_32F, kernelWeight);
        return kernel.clone();
    }
    else
    {
        cout << "error: kernel size is not supported" << endl;
        exit(0);
    }
}

cv::Mat sobelKernel(int kernelSize, int r) {

    if (kernelSize == 3)
    {
        float kernelWeight[9] = { -1, 0, 1, -2, 0, 2, -1, 0, 1 };
        Mat kernelX = cv::Mat(3, 3, CV_32F, kernelWeight);
        Mat kernelY;
        if (r == 0)
        {
            return kernelX.clone();
        }
        else
        {
            Mat kernelY;
            flip(kernelX.t(), kernelY, 0);
            return kernelY.clone();
        }
    }
    else if (kernelSize == 5)
    {
        float kernelWeight[25] = { 2, 1, 0, -1, -2, 2, 1, 0, -1, -2, 4, 2, 0, -2, -4, 2, 1, 0, -1, -2, 2, 1, 0, -1, -2 };
        Mat kernelX = cv::Mat(5, 5, CV_32F, kernelWeight);
        Mat kernelY;
        if (r == 0)
        {
            return kernelX.clone();
        }
        else
        {
            Mat kernelY;
            flip(kernelX.t(), kernelY, 0);
            return kernelY.clone();
        }
    }
    else if (kernelSize == 7)
    {
        float kernelWeight[49] = { 4, 2, 1, 0, -1, -2, -4,
                                   4, 2, 1, 0, -1, -2, -4,
                                   4, 2, 1, 0, -1, -2, -4,
                                   8, 4, 2, 0, -2, -4, -8,
                                   4, 2, 1, 0, -1, -2, -4,
                                   4, 2, 1, 0, -1, -2, -4,
                                   4, 2, 1, 0, -1, -2, -4, };
        Mat kernelX = cv::Mat(7, 7, CV_32F, kernelWeight);
        Mat kernelY;
        if (r == 0)
        {
            return kernelX.clone();
        }
        else
        {
            Mat kernelY;
            flip(kernelX.t(), kernelY, 0);
            return kernelY.clone();
        }
    }
    else
    {
        cout << "error: kernel size is not supported" << endl;
        exit(0);
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

    int mode;
    cout << "Please choose the filter type:" <<endl;
    cout << "(1) Gaussian" << endl;
    cout << "(2) Averaging" << endl;
    cout << "(3) Unsharp mask" << endl;
    cout << "(4) Laplacian" << endl;
    cout << "(5) Sobel" << endl;
    scanf("%d", &mode);

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

    Mat kernel;
    if (mode == 1 || mode == 3) 
    {
        float stdDev = 2; // gaussian filter std
        kernel = gaussKernel(kernelSize, stdDev);
    }

    else if (mode == 2)
    {
        kernel = averagingKernel(kernelSize);
    }
    
    else if (mode == 4)
    {
        kernel = laplacianKernel(kernelSize);
    }
    else if (mode == 5)
    {
        cout << "Please set the direction of sobel filter (x:0, y:1):" << endl;
        int r;
        scanf("%d", &r);
        kernel = sobelKernel(kernelSize, r);
    }
    else
    {
        cout << "error: input wrong index" << endl;
        exit(0);
    }

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
    if (mode == 3)
    {   
        float factor;
        cout << "Please set factor:" << endl;
        scanf("%f", &factor);

        Mat floatRawImage;
        image.convertTo(floatRawImage, CV_32FC1);
        for (int y = 0; y < finalImageFloat.rows; y++)
        {
            for (int x = 0; x < finalImageFloat.cols; x++)
            {
                finalImageFloat.at<float>(y, x) = floatRawImage.at<float>(y, x) + factor * (floatRawImage.at<float>(y, x) - finalImageFloat.at<float>(y, x));
            }
        }
    }

    Mat finalImage;
    finalImageFloat.convertTo(finalImage, CV_8UC1);

    //Displaying source image
    namedWindow("Original Image");
    imshow("Original Image", image);

    //Displaying histogram equalized image
    namedWindow("Final Image");
    imshow("Final Image", finalImage);
    imwrite("result/convolution/result.png", finalImage);

    waitKey();
    return 0;
        
}

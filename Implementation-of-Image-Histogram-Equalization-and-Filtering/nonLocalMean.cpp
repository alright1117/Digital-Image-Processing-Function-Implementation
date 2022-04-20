#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>
#include <cmath>
#include <iomanip>
#include <time.h>

#define MAX(i,j) ( (i)<(j) ? (j):(i) )
#define MIN(i,j) ( (i)<(j) ? (i):(j) )

#define fTiny 0.00000001f

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

cv::Mat addGaussianNoise(const cv::Mat_<float>& src) {

    Mat_<float> noiseImage;
    src.copyTo(noiseImage);
    Mat noise(src.size(), src.type());
    cv::randn(noise, 0, 20); //mean and variance
    noiseImage += noise;
    Mat finalImage;
    noiseImage.convertTo(finalImage, CV_8UC1);

    return finalImage.clone();
}

cv::Mat_<float> nonLocalMeanFilter(int iDWin,                       // Half size of patch
                                   int iDBloc,                      // Half size of research window
                                   float h, 
                                   const cv::Mat& src){     // Input

    // length of each channel
    int iWidth = src.cols;
    int iHeight = src.rows;
    Mat dst(iWidth, iHeight, src.type());

    // padding image 
    Mat paddedImage = paddingImage(src, iDWin);
    Mat floatImage;
    paddedImage.convertTo(floatImage, CV_32FC1);

    // PROCESS STARTS
    // for each pixel (x,y)
    for (int y = 0; y < iHeight; y++)
    {
        for (int x = 0; x < iWidth; x++)
        {
            int y1 = y + iDWin;
            int x1 = x + iDWin;
            // research zone depending on the boundary and the size of the window
            int imin = MAX(y1 - iDBloc, iDWin + 1);
            int imax = MIN(y1 + iDBloc, iHeight + iDWin - 1);

            int jmin = MAX(x1 - iDBloc, iDWin + 1);
            int jmax = MIN(x1 + iDBloc, iWidth + iDWin - 1);



            // maximum of weights. Used for reference patch
            float fMaxWeight = 0.0f;
            // sum of weights
            float fTotalWeight = 0.0f;
            float average = 0.0f;
            for (int i = imin; i <= imax; i++)
            {
                for (int j = jmin; j <= jmax; j++)
                {
                    if (j != x1 || i != y1)
                    {
                        float fDif = 0.0;
                        for (int s = -iDWin; s <= iDWin; s++)
                            for (int r = -iDWin; r <= iDWin; r++)
                            {
                                float dif = (floatImage.at<float>(y1 + s, x1 + r) - floatImage.at<float>(i + s, j + r));
                                fDif += (dif * dif);
                            }

                        float fWeight = expf(-(fDif / h / h));
                        if (fWeight > fMaxWeight) fMaxWeight = fWeight;

                        fTotalWeight += fWeight;
                        average += fWeight * floatImage.at<float>(i, j);
                    }
                }
            }
            // current patch with fMaxWeight
            average += fMaxWeight * floatImage.at<float>(y1, x1);
            fTotalWeight += fMaxWeight;
            // normalize average value when fTotalweight is not near zero

            if (fTotalWeight > fTiny)
            {
                dst.at<uchar>(y, x) = saturate_cast<uchar>(average / fTotalWeight);
            }
            else
            {
                dst.at<uchar>(y, x) = src.at<uchar>(y, x);
            }
        }
    }
    return dst.clone();
}


int main() {
    clock_t tStart = clock();
    cout << "Applying Non local mean Filter." << endl;

    string path;
    cout << "Please input image path:" << endl;
    cin >> path;
    const char* refPath = path.c_str();
    Mat image = read_raw_img(refPath);

    Mat floatImage;
    image.convertTo(floatImage, CV_32FC1);

   // Mat noiseImage = addGaussianNoise(floatImage);

    
    Mat finalImageFloat = nonLocalMeanFilter(7, 17, 250.0f, image);
    printf("Time taken: %.2fs\n", (double)(clock() - tStart) / CLOCKS_PER_SEC);

    Mat finalImage;
    finalImageFloat.convertTo(finalImage, CV_8UC1);
    
    //Displaying source image
    namedWindow("Original Image");
    imshow("Original Image", image);

    //Displaying noise image
    //namedWindow("Noise Image");
    //imshow("Noise Image", noiseImage);
    //imwrite("result/nonLocalMean/resultnoise.png", noiseImage);

    //Displaying Final image
    namedWindow("Final Image");
    imshow("Final Image", finalImage);
    imwrite("result/nonLocalMean/result.png", finalImage);

    waitKey();
    return 0;
}
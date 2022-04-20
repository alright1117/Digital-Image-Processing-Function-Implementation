# include <iostream>
# include <cmath>
# include "compressor.h"
# include "rgbcompressor.h"

int main(int argc, char** argv){

	int gray = 0; // 1: gray, 0: rgb

	char path[] = "images/lena_color_512.tif";
	char codePath[] = "images/result/lena_color_512.compactado";
	char finalPath[] = "images/result/lena_color_512.saida.png";

	if (gray == 1)
	{
		Compressor c;
		c.compress(path);

		Compressor d;
		d.decompress(codePath);

		cv::Mat img = cv::imread(path, cv::IMREAD_GRAYSCALE);
		cv::Mat compressImg = cv::imread(finalPath, cv::IMREAD_GRAYSCALE);
		img.convertTo(img, CV_32FC1);
		compressImg.convertTo(compressImg, CV_32FC1);

		float MSE = 0.f;
		for (int i = 0; i < img.rows; i++) {
			for (int j = 0; j < img.cols; j++) {
				MSE += pow((img.at<float>(i, j) - compressImg.at<float>(i, j)), 2);
			}
		}

		MSE = MSE / (img.rows * img.cols);
		float RMSE = sqrt(MSE);
		float PSNR = 20 * log10f(255 / RMSE);
		std::cout << "RMSE: " << RMSE << " PSNR: " << PSNR << std::endl;
	}
	else
	{
		rgbCompressor c;
		c.compress(path);

		rgbCompressor d;
		d.decompress(codePath);

		cv::Mat img = cv::imread(path);
		cv::Mat compressImg = cv::imread(finalPath);
		img.convertTo(img, CV_32FC3);
		compressImg.convertTo(compressImg, CV_32FC3);

		float MSE = 0.f;
		for (int i = 0; i < img.rows; i++) {
			for (int j = 0; j < img.cols; j++) {
				cv::Vec3f imgPixel = img.at<cv::Vec3f>(i, j);
				cv::Vec3f imgCPixel = compressImg.at<cv::Vec3f>(i, j);

				MSE += pow((imgPixel[0] - imgCPixel[0]), 2) + pow((imgPixel[1] - imgCPixel[1]), 2) + pow((imgPixel[2] - imgCPixel[2]), 2);
			}
		}

		MSE = MSE / (img.rows * img.cols * img.channels());

		float RMSE = sqrt(MSE);
		float PSNR = 20 * log10f(255 / RMSE);
		std::cout << "RMSE: " << RMSE << " PSNR: " << PSNR << std::endl;
	}
	



	return 0;
}





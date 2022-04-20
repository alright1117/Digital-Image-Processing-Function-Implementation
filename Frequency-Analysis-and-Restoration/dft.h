#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <time.h>
#include <Windows.h>
#include <winbase.h>

#define _CRT_SECURE_NO_WARNINGS
#define intsize sizeof(int)
#define complexsize sizeof(complex)
#define PI 3.1415926

using namespace cv;

int* a, * b;
int nLen, init_nLen, mLen, init_mLen, N, M;

typedef struct{
	float real;
	float image;
}complex;

complex* A, * A_In, * W;

complex Add(complex, complex);
complex Sub(complex, complex);
complex Mul(complex, complex);
int calculate_M(int);
void reverse(int, int);

void fft(int fft_nLen, int fft_M)
{
	int i;
	int lev, dist, p, t;
	complex B;

	W = (complex*)malloc(complexsize * fft_nLen / 2);

	for (lev = 1; lev <= fft_M; lev++)
	{
		dist = (int)pow(2, lev - 1);
		for (t = 0; t < dist; t++)
		{
			p = t * (int)pow(2, fft_M - lev);
			W[p].real = (float)cos(2 * PI * p / fft_nLen);
			W[p].image = (float)(-1 * sin(2 * PI * p / fft_nLen));
			for (i = t; i < fft_nLen; i = i + (int)pow(2, lev))
			{
				B = Add(A[i], Mul(A[i + dist], W[p]));
				A[i + dist] = Sub(A[i], Mul(A[i + dist], W[p]));
				A[i].real = B.real;
				A[i].image = B.image;
			}
		}
	}

	free(W);
}

void ifft(int fft_nLen, int fft_M)
{
	int i;
	int lev, dist, p, t;
	complex B;

	W = (complex*)malloc(complexsize * fft_nLen / 2);

	for (lev = 1; lev <= fft_M; lev++)
	{
		dist = (int)pow(2, lev - 1);
		for (t = 0; t < dist; t++)
		{
			p = t * (int)pow(2, fft_M - lev);
			W[p].real = (float)cos(2 * PI * p / fft_nLen);
			W[p].image = (float)sin(2 * PI * p / fft_nLen);
			for (i = t; i < fft_nLen; i = i + (int)pow(2, lev))
			{
				B = Add(A[i], Mul(A[i + dist], W[p]));
				A[i + dist] = Sub(A[i], Mul(A[i + dist], W[p]));
				A[i].real = B.real;
				A[i].image = B.image;
			}
		}
	}

	free(W);
}

int calculate_M(int len)
{
	int i;
	int k;

	i = 0;
	k = 1;
	while (k < len)
	{
		k = k * 2;
		i++;
	}

	return i;
}


void reverse(int len, int M)
{
	int i, j;

	a = (int*)malloc(intsize * M);
	b = (int*)malloc(intsize * len);

	for (i = 0; i < M; i++)
	{
		a[i] = 0;
	}

	b[0] = 0;
	for (i = 1; i < len; i++)
	{
		j = 0;
		while (a[j] != 0)
		{
			a[j] = 0;
			j++;
		}

		a[j] = 1;
		b[i] = 0;
		for (j = 0; j < M; j++)
		{
			b[i] = b[i] + a[j] * (int)pow(2, M - 1 - j);
		}
	}
}


complex Add(complex c1, complex c2)
{
	complex c;
	c.real = c1.real + c2.real;
	c.image = c1.image + c2.image;
	return c;
}


complex Sub(complex c1, complex c2)
{
	complex c;
	c.real = c1.real - c2.real;
	c.image = c1.image - c2.image;
	return c;
}


complex Mul(complex c1, complex c2)
{
	complex c;
	c.real = c1.real * c2.real - c1.image * c2.image;
	c.image = c1.real * c2.image + c2.real * c1.image;
	return c;
}


Mat DFT_trans(Mat img, Mat out)
{
	DWORD start_time = GetTickCount();
	std::cout << "raw:" << img.rows << " " << img.cols;

	init_mLen = img.rows;
	init_nLen = img.cols;
	M = calculate_M(init_mLen);
	N = calculate_M(init_nLen);
	mLen = (int)pow(2, M);
	nLen = (int)pow(2, N);

	A_In = (complex*)malloc(complexsize * nLen * mLen);

	img.convertTo(img, CV_32FC1);

	/*影象矩陣元素儲存到complex結構體中*/
	for (int i = 0; i < init_mLen; i++)
	{
		//uchar *data = img.ptr<uchar>(i);
		for (int j = 0; j < init_nLen; j++)
		{
			//A_In[i*nLen + j].real = (int)data[j];
			A_In[i * nLen + j].real = (float)img.at<float>(i, j);
			//std::cout << A_In[i*nLen + j].real<< std::endl;
			A_In[i * nLen + j].image = 0.0;
		}
	}


	for (int i = 0; i < mLen; i++)
	{
		for (int j = init_nLen; j < nLen; j++)
		{
			A_In[i * nLen + j].real = 0.0;
			A_In[i * nLen + j].image = 0.0;
		}
	}


	for (int i = init_mLen; i < mLen; i++)
	{
		for (int j = 0; j < init_nLen; j++)
		{
			A_In[i * nLen + j].real = 0.0;
			A_In[i * nLen + j].image = 0.0;
		}
	}

	A = (complex*)malloc(complexsize * nLen);
	reverse(nLen, N);
	for (int i = 0; i < mLen; i++)
	{
		for (int j = 0; j < nLen; j++)
		{
			A[j].real = A_In[i * nLen + b[j]].real;
			A[j].image = A_In[i * nLen + b[j]].image;
		}

		fft(nLen, N);
		for (int j = 0; j < nLen; j++)
		{
			A_In[i * nLen + j].real = A[j].real;
			A_In[i * nLen + j].image = A[j].image;
		}
	}
	free(A);

	A = (complex*)malloc(complexsize * mLen);
	reverse(mLen, M);
	for (int i = 0; i < nLen; i++)
	{
		for (int j = 0; j < mLen; j++)
		{
			A[j].real = A_In[b[j] * nLen + i].real;
			A[j].image = A_In[b[j] * nLen + i].image;
		}


		fft(mLen, M);
		for (int j = 0; j < mLen; j++)
		{
			A_In[j * nLen + i].real = A[j].real;
			A_In[j * nLen + i].image = A[j].image;
		}
	}

	free(A);

	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			out.at<Vec2f>(i, j)[0] = (float)A_In[i * nLen + j].real;
			//std::cout << A_In[i*nLen + j].real << std::endl;
			out.at<Vec2f>(i, j)[1] = (float)A_In[i * nLen + j].image;
		}

	}

	free(A_In);

	return out;
}

Mat IDFT_trans(Mat img, Mat out)
{
	DWORD start_time = GetTickCount();
	cv::resize(img, img, Size(img.cols, img.cols));
	std::cout << "raw:" << img.rows << " " << img.cols;

	init_mLen = img.rows;
	init_nLen = img.cols;
	M = calculate_M(init_mLen);
	N = calculate_M(init_nLen);
	mLen = (int)pow(2, M);
	nLen = (int)pow(2, N);

	A_In = (complex*)malloc(complexsize * nLen * mLen);

	/*影象矩陣元素儲存到complex結構體中*/
	for (int i = 0; i < init_mLen; i++)
	{
		//uchar *data = img.ptr<uchar>(i);
		for (int j = 0; j < init_nLen; j++)
		{
			//A_In[i*nLen + j].real = (int)data[j];
			A_In[i * nLen + j].real = img.at<Vec2f>(i, j)[0];
			//std::cout << A_In[i*nLen + j].real<< std::endl;
			A_In[i * nLen + j].image = img.at<Vec2f>(i, j)[1];
		}
	}


	for (int i = 0; i < mLen; i++)
	{
		for (int j = init_nLen; j < nLen; j++)
		{
			A_In[i * nLen + j].real = 0.0;
			A_In[i * nLen + j].image = 0.0;
		}
	}


	for (int i = init_mLen; i < mLen; i++)
	{
		for (int j = 0; j < init_nLen; j++)
		{
			A_In[i * nLen + j].real = 0.0;
			A_In[i * nLen + j].image = 0.0;
		}
	}

	A = (complex*)malloc(complexsize * nLen);
	reverse(nLen, N);
	for (int i = 0; i < mLen; i++)
	{
		for (int j = 0; j < nLen; j++)
		{
			A[j].real = A_In[i * nLen + b[j]].real;
			A[j].image = A_In[i * nLen + b[j]].image;
		}

		ifft(nLen, N);
		for (int j = 0; j < nLen; j++)
		{
			A_In[i * nLen + j].real = A[j].real;
			A_In[i * nLen + j].image = A[j].image;
		}
	}
	free(A);

	A = (complex*)malloc(complexsize * mLen);
	reverse(mLen, M);
	for (int i = 0; i < nLen; i++)
	{
		for (int j = 0; j < mLen; j++)
		{
			A[j].real = A_In[b[j] * nLen + i].real;
			A[j].image = A_In[b[j] * nLen + i].image;
		}


		ifft(mLen, M);
		for (int j = 0; j < mLen; j++)
		{
			A_In[j * nLen + i].real = A[j].real;
			A_In[j * nLen + i].image = A[j].image;
		}
	}

	free(A);

	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			out.at<Vec2f>(i, j)[0] = (float)A_In[i * nLen + j].real / mLen / nLen;
			out.at<Vec2f>(i, j)[1] = (float)A_In[i * nLen + j].image / mLen / nLen;
		}

	}

	free(A_In);

	return out.clone();
}

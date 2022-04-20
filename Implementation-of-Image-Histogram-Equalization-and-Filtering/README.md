# Implementation Image Histogram Equalization and Filtering

## Overview 

This is an assignment of digital image processing and computer vision course, the requirements of this assignment are (a) Histogram equalization: Give three test images with RAW format, use the gamma correction to decrease/increase the brightness of the images. Based on these degraded images, implement histogram equalization, local histogram equalization, and histogram matching, (b) Adopt the gaussian, averaging, unsharp mask, laplacian, and sobel filters to check the visualized results, (c) Give two special kernels below, discuss the properties and the potential problems, (d) select one of the “statistic-order filters” to filter the noisy image and show the visualized results, (e) implement Bilateral filter with/without Gaussian Smoothing Kernel for the “Noisy.raw” and verify the visualization effect, (f) implement the naive NLM to denoise the Noisy.raw with kernel sizes of 5x5, 7x7, and (g) accelerate the NLM filter.

### Contents:

- [Overview](#overview)
- [Prerequisites](#Prerequisites)
- [Folder Structure](#FolderStructure)

---
### Prerequisites:

- OpenCV 4.5.4

[[back]](#contents)
<br/>

---

### Folder Structure

>```data/``` &nbsp; - &nbsp; examples image <br/>```result/``` &nbsp; - &nbsp; the result of histogram equalization and filtering <br/>

>```bilteralFilter.cpp``` &nbsp; - &nbsp; implementation of bilateral filter<br/>
>```convolution.cpp``` &nbsp; - &nbsp; implementation of gaussian, averaging, unsharp mask, laplacian and sobel filtering. <br/>
>```convolution-2.cpp``` &nbsp; - &nbsp; an example of convolution issues <br/>```histogramEqualization.cpp``` &nbsp; - &nbsp; implementation of histogram equalization <br/>```localHistogramEqualizating.cpp``` &nbsp; - &nbsp; implementation of  local histogram equalization <br/>```histogramMatching.cpp``` &nbsp; - &nbsp; implementation of histogram matching <br/>```medianFilter.cpp``` &nbsp; - &nbsp; implementation of median filtering <br/>```nonLocalMean.cpp``` &nbsp; - &nbsp; implementation of non local mean filtering <br/>```hist.ipynb``` &nbsp; - &nbsp; comparing the histogram of image before and after equalization <br/>

[[back]](#contents)
<br/>

---




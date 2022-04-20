# Frequency Analysis and Restoration

## Overview 

This is an assignment of digital image processing and computer vision course, the requirements of this assignment are (a) Fourier transform: Give test images with RAW format, transform the images to the frequency domain using DFT and show the raw and centralized frequency response, then compare the spectrum histogram with different images. (b) Adopt ideal and gaussian low pass filter on noise images with gaussian and uniform noise.  (c) Adopt ideal and Butterworth high pass filter on noise images, compare the results with Laplacian filter in spatial domain. (d) Give the Noise, adopt inverse filter, wiener filter, BM3D, and guided filter to remove the noise as clean as possible. (e) Adopt DCT as the image restoration domain, design a DCT-based denoiser for Noisy image.

### Contents:

- [Overview](#overview)
- [Prerequisites](#Prerequisites)
- [Folder Structure](#Folder Structure)

---
### Prerequisites:

- OpenCV 4.5.4

[[back]](#contents)
<br/>

---

### Folder Structure

>```data/``` &nbsp; - &nbsp; test images <br/>```result/``` &nbsp; - &nbsp; the result of frequency transform and filtering <br/>```BM3D/``` &nbsp; - &nbsp; BM3D algorithm implemented by matlab <br/>

>```DFT.cpp``` &nbsp; - &nbsp; implementation of fast Fourier transform<br/>
>```DCT.cpp``` &nbsp; - &nbsp; implementation of discrete cosine transform<br/>
>```guidedFilter.cpp``` &nbsp; - &nbsp; implementation of guided filter<br/>```highPassFilter.cpp``` &nbsp; - &nbsp; implementation of ideal high pass filter and Butterworth filter <br/>```lowPassFilter.cpp``` &nbsp; - &nbsp; implementation of ideal low pass filter and Gaussian filter  <br/>```inverseFilter.cpp``` &nbsp; - &nbsp; implementation of inverse filter<br/>```wienerFilter.cpp``` &nbsp; - &nbsp; implementation of wiener filter<br/>```plot.ipynb``` &nbsp; - &nbsp; comparing the frequency spectrum of three test images <br/>

[[back]](#contents)
<br/>

---




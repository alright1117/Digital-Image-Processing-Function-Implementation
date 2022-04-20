# JPEG Compression

## Overview 

This is an assignment of digital image processing and computer vision course, the requirements of this assignment are (a) Implement the grey level JPEG compression algorithm with your own encode and decoder, which the bitstream can be unrecognizable by standard image viewers. Please calculate RMSE and PSNR between the decompressed and original images, and show two examples in the report. (b) Implement the color level JPEG compression with “4:2:0” color sampling. In this term, the bitstream can be unrecognizable by OS as well. (c) Design a mechanism called “Quality factor” that can control the output quality by changing the quantization table dynamically. (d) Implement the full-functional JPEG compression with the standard bitstream, where your encoder can generate the bitstream that the OS can be recognized. (e) The Huffman table can be embedded into the Header of the encoded bitstream (file-stream) to achieve a better compression ratio. Please implement this and the encoded bitstream should be able to be recognizable by OS.

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

>```images/``` &nbsp; - &nbsp; test images <br/>```MATLAB-Jpeg-encoder/``` &nbsp; - &nbsp; standard JPEG compression implemented by MATLab <br/>

>```main.cpp``` &nbsp; - &nbsp; the implementation of my own compression encoder and decoder<br/>
>```compressor.cpp``` &nbsp; - &nbsp; compressor<br/>
>```rgbCompressor.cpp``` &nbsp; - &nbsp; RGB compressor<br/>```huffman.cpp``` &nbsp; - &nbsp; Huffman encoding  <br/>

[[back]](#contents)
<br/>

---




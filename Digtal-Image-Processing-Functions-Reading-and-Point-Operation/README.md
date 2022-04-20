# Digital Image Processing Functions: Reading and Point Operation

## Overview 

This is an assignment of digital image processing and computer vision course, the requirements of this assignment are (a) reading the images correctly and display them on the screen, and draw the centered 10x10 pixels values for each image, (b) implementing and comparing the log-transform, gamma-transform, and image negative, and (c) realizing and comparing down- and up-sampling on these images with bilinear and nearest-neighbor interpolation.

### Contents:

- [Overview](#overview)
- [Prerequisites](#Prerequisites)
- [Folder Structure](#FolderStructure)
- [Implementation](#Implementation)

---
### Prerequisites:

- Python 3.6
- numpy 1.20.1
- opencv-python 4.4.0.46

[[back]](#contents)
<br/>

---

### Folder Structure

>```data/``` &nbsp; - &nbsp; examples image <br/>

>```ehance_img.py``` &nbsp; - &nbsp; enhancement toolkits<br/>
>```read_crop_img.py``` &nbsp; - &nbsp; reading image and center crop <br/>
>```resize_img.py``` &nbsp; - &nbsp; up- and down-sampling with bilinear and nearest-neighbor interpolation <br/>

[[back]](#contents)
<br/>

---
### Implementation

#### (a) Reading images and center crop

To finish the requirement, use the following command:

```bash
python read_crop_img.py --img_path data/baboon.bmp --save_path read_crop_result/baboon.png
```

Optional parameters (and default values):

>```--img_path```: **```data/baboon.bmp```** &nbsp; - &nbsp; the path of input image<br/>
>```--save_path```: **```read_crop_result/baboon.png```** &nbsp; - &nbsp; result path <br/>

[[back]](#contents)
<br/>

---

#### (b) Image Enhancement Toolkit

To finish the requirement, use the following command:

```bash
python enhance_img.py --img_path data/baboon.bmp --save_path read_crop_result/baboon.png
```

Optional parameters (and default values):

>```--img_path```: **```data/baboon.bmp```** &nbsp; - &nbsp; the path of input image<br/>
>```--save_path```: **```enhance_img_result/baboon.png```** &nbsp; - &nbsp; result path <br/>

[[back]](#contents)
<br/>

---

#### (c) up- and down-sampling by bilinear and nearest-neighbor interpolation

To finish the requirement, use the following command:

```bash
python resize_img.py --img_path data/lena.raw --save_path resize_result
```

Optional parameters (and default values):

>```--img_path```: **```data/lena.raw```** &nbsp; - &nbsp; the path of input image<br/>
>```--save_path```: **```resize_result```** &nbsp; - &nbsp; result folder path <br/>

[[back]](#contents)
<br/>

---

#### 


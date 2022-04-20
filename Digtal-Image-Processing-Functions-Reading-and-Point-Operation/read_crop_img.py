import numpy as np
import os
import argparse
import cv2
import matplotlib.pyplot as plt

def read_img(path):
    if path.split('.')[-1] == 'raw':
        with open(path, 'rb') as fd:
            rows = 512
            cols = 512
            f = np.fromfile(fd, dtype=np.uint8,count=rows*cols)
            img = f.reshape((rows, cols))
    else:
        img = cv2.imread(path, cv2.IMREAD_GRAYSCALE)
    
    return img

def center_crop(img, size):
    img_h, img_w = img.shape
    c_h, c_w = img_h // 2, img_w // 2
    crop_img = img[c_h-size:c_h+size, c_w-size:c_w+size]
    return crop_img

def main(args):

    img_name = args.img_path.split('/')[-1]
    img = read_img(args.img_path)

    fig = plt.figure()
    plt.subplot(211)
    plt.title(img_name, fontsize=20)
    plt.imshow(img, cmap='gray')
    plt.axis('off')
    plt.subplot(212)
    plt.imshow(center_crop(img, 10), cmap='gray')
    plt.axis('off')
    plt.show()

    fig.savefig(args.save_path)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--img_path", type = str, default = 'data/baboon.bmp')
    parser.add_argument("--save_path", type = str, default = 'read_crop_result/baboon.png')
    args = parser.parse_args()
    main(args)

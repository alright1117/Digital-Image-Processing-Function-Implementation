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

def enhancement(img, method):
    if method == 'log':
        img = (np.log(img + 1) / (np.log(1 + np.max(img)))) * 255
        enhance_img = np.array(img, dtype=np.uint8)
    elif method == 'gamma':
        enhance_img = gamma_correlation(img, gamma=2.2)
    elif method == 'negative':
        enhance_img = 255 - img
    return enhance_img
    
def gamma_correlation(img, gamma):
    invGamma = 1.0 / gamma
    img_gamma = (img / 255) ** invGamma * 255
    return img_gamma

def main(args):

    img_name = args.img_path.split('/')[-1]
    img = read_img(args.img_path)

    fig = plt.figure(figsize=(24, 12))
    plt.subplot(411)
    plt.title(img_name, fontsize=20)
    plt.imshow(img, cmap='gray')
    plt.axis('off')

    plt.subplot(412)
    log_img = enhancement(img, method='log')
    plt.imshow(log_img, cmap='gray')
    plt.axis('off')

    plt.subplot(413)
    gamma_img = enhancement(img, method='gamma')
    plt.imshow(gamma_img, cmap='gray')
    plt.axis('off')

    plt.subplot(414)
    neg_img = enhancement(img, method='negative')
    plt.imshow(neg_img, cmap='gray')
    plt.axis('off')

    fig.savefig(args.save_path)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--img_path", type = str, default = 'data/baboon.bmp')
    parser.add_argument("--save_path", type = str, default = 'enhance_img_result/baboon.png')
    args = parser.parse_args()
    main(args)

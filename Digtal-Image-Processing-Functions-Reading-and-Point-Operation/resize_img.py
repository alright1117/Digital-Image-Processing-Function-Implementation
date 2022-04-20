import numpy as np
import math
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

def bilinear_resize(image, new_h, new_w):

  img_h, img_w = image.shape

  new_img = np.empty([new_h, new_w])

  x_ratio = float(img_w - 1) / (new_w - 1) if new_w > 1 else 0
  y_ratio = float(img_h - 1) / (new_h - 1) if new_h > 1 else 0

  for i in range(new_h):
    for j in range(new_w):

      x_l, y_l = math.floor(x_ratio * j), math.floor(y_ratio * i)
      x_h, y_h = math.ceil(x_ratio * j), math.ceil(y_ratio * i)

      x_weight = (x_ratio * j) - x_l
      y_weight = (y_ratio * i) - y_l

      a = image[y_l, x_l]
      b = image[y_l, x_h]
      c = image[y_h, x_l]
      d = image[y_h, x_h]

      pixel = a * (1 - x_weight) * (1 - y_weight) \
              + b * x_weight * (1 - y_weight) + \
              c * y_weight * (1 - x_weight) + \
              d * x_weight * y_weight

      new_img[i][j] = pixel

  return new_img

def nearest_neighbor_resize(img, new_h, new_w):

    img_h, img_w = img.shape

    new_img = np.empty([new_h, new_w])

    x_ratio = float(img_w - 1) / (new_w - 1) if new_w > 1 else 0
    y_ratio = float(img_h - 1) / (new_h - 1) if new_h > 1 else 0

    for i in range(new_h):
        for j in range(new_w):
            p_x = int(j * x_ratio)
            p_y = int(i * y_ratio)

            new_img[i, j] = img[p_y, p_x]

    return new_img

def main(args):

    img_name = args.img_path.split('/')[-1]
    img = read_img(args.img_path)

    # 512x512 -> 128x128
    b_img128 = bilinear_resize(img, 128, 128)
    n_img128 = nearest_neighbor_resize(img, 128, 128)
    cv2.imwrite(os.path.join(args.save_path, '512_128.png'), cv2.hconcat([b_img128, n_img128]))

    # 512x512 -> 32x32
    b_img32 = bilinear_resize(img, 32, 32)
    n_img32 = nearest_neighbor_resize(img, 32, 32)
    cv2.imwrite(os.path.join(args.save_path, '512_32.png'), cv2.hconcat([b_img32, n_img32]))

    # 32x32 -> 512x512
    b_img512 = bilinear_resize(b_img32, 512, 512)
    n_img512 = nearest_neighbor_resize(n_img32, 512, 512)
    cv2.imwrite(os.path.join(args.save_path, '32_512.png'), cv2.hconcat([b_img512, n_img512]))

    # 512x512 -> 1024x512
    b_img1024 = bilinear_resize(img, 512, 1024)
    n_img1024 = nearest_neighbor_resize(img, 512, 1024)
    cv2.imwrite(os.path.join(args.save_path, '512_1024.png'), cv2.hconcat([b_img1024, n_img1024]))

    # 128x128 -> 256x512
    b_img256 = bilinear_resize(b_img128, 512, 256)
    n_img256 = nearest_neighbor_resize(n_img128, 512, 256)
    cv2.imwrite(os.path.join(args.save_path, '128_256.png'), cv2.hconcat([b_img256, n_img256]))

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--img_path", type = str, default = 'data/lena.raw')
    parser.add_argument("--save_path", type = str, default = 'resize_result')
    args = parser.parse_args()
    main(args)
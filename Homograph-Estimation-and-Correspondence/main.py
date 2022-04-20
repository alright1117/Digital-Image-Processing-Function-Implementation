import cv2
import argparse
import matplotlib.pyplot as plt
import os

from utils import detect, matchKeyPoint, drawMatches
from RANSAC import RANSAC, fastRANSAC
from sift import computeKeypointsAndDescriptors

def main(args):

    img_left = cv2.imread(args.img1_path)
    img_right = cv2.imread(args.img2_path)

    if not os.path.exists(args.save_path):
        os.makedirs(args.save_path)

    print("Detect keypoints...")
    if args.handcraft:
        kps_l, features_l = computeKeypointsAndDescriptors(cv2.cvtColor(img_left, cv2.COLOR_BGR2GRAY))
        kps_r, features_r = computeKeypointsAndDescriptors(cv2.cvtColor(img_right, cv2.COLOR_BGR2GRAY))
    else:
        kps_l, features_l = detect(img_left, args.method)
        kps_r, features_r = detect(img_right, args.method)

    print("Matching points...")
    (matches_pos, MatchesDist) = matchKeyPoint(kps_l, kps_r, features_l, features_r, args.ratio)
    vis = drawMatches([img_left, img_right], matches_pos)
    cv2.imwrite(os.path.join(args.save_path, "base.png"), vis)

    print("Finding homography...")
    print("RANSAC:")
    # RANSAC result
    (HomoMat, matches_pos_ransac) = RANSAC(matches_pos)
    vis = drawMatches([img_left, img_right], matches_pos_ransac)
    cv2.imwrite(os.path.join(args.save_path, "RANSAC.png"), vis)

    print("fastRANSAC:")
    # My RANSAC result
    (HomoMat, matches_pos_ransac) = fastRANSAC(matches_pos, MatchesDist)
    vis = drawMatches([img_left, img_right], matches_pos_ransac)
    cv2.imwrite(os.path.join(args.save_path, "fastRANSAC.png"), vis)

if __name__ == "__main__":

    parser = argparse.ArgumentParser()

    parser.add_argument("--img1_path", type = str, default = "scene/book1.jpg")
    parser.add_argument("--img2_path", type = str, default = "scene/scene.jpg")
    parser.add_argument("--method", type = str, default = 'sift')
    
    parser.set_defaults(handcraft=False)
    parser.add_argument('--handcraft', dest='handcraft', action='store_true')
    
    parser.add_argument("--ratio", type = float, default = 0.75)
    parser.add_argument("--save_path", type = str, default = "result/book1/sift")

    args = parser.parse_args()

    main(args)
import cv2
import os
import argparse
import matplotlib.pyplot as plt
import numpy as np
import random
import scipy.stats as stats
from utils import detect, matchKeyPoint

def solve_homography(P, m):
    """
    Solve homography matrix 
    Args:
        P:  Coordinates of the points in the original plane,
        m:  Coordinates of the points in the target plane
    Returns:
        H: Homography matrix 
    """
    A = []  
    for r in range(len(P)): 
        #print(m[r, 0])
        A.append([-P[r,0], -P[r,1], -1, 0, 0, 0, P[r,0]*m[r,0], P[r,1]*m[r,0], m[r,0]])
        A.append([0, 0, 0, -P[r,0], -P[r,1], -1, P[r,0]*m[r,1], P[r,1]*m[r,1], m[r,1]])

    u, s, vt = np.linalg.svd(A) # Solve s ystem of linear equations Ah = 0 using SVD
    # pick H from last line of vt  
    H = np.reshape(vt[8], (3,3))
    # normalization, let H[2,2] equals to 1
    H = (1/H.item(8)) * H

    return H

def RANSAC(matches_pos):
    '''
        Fit the best homography model with RANSAC algorithm - noBlending、linearBlending、linearBlendingWithConstant
    '''
    dstPoints = [] # i.e. left image(destination image)
    srcPoints = [] # i.e. right image(source image) 
    for dstPoint, srcPoint in matches_pos:
        dstPoints.append(list(dstPoint)) 
        srcPoints.append(list(srcPoint))
    dstPoints = np.array(dstPoints)
    srcPoints = np.array(srcPoints)
    
    # RANSAC algorithm, selecting the best fit homography
    NumSample = len(matches_pos)
    threshold = 5.0  
    NumIter = 200
    NumRamdomSubSample = 4
    MaxInlier = 0
    NumInlier_list = []

    for run in range(NumIter):
        SubSampleIdx = random.sample(range(NumSample), NumRamdomSubSample) # get the Index of ramdom sampling
        H = solve_homography(srcPoints[SubSampleIdx], dstPoints[SubSampleIdx])
        
        # find the best Homography have the the maximum number of inlier
        NumInlier = 0 
        Inlier_pos = []
        for i in range(NumSample):
            if i not in SubSampleIdx:
                concateCoor = np.hstack((srcPoints[i], [1])) # add z-axis as 1
                dstCoor = H @ concateCoor.T # calculate the coordination after transform to destination img 
                if dstCoor[2] <= 1e-8: # avoid divide zero number, or too small number cause overflow
                    continue
                dstCoor = dstCoor / dstCoor[2]
                if (np.linalg.norm(dstCoor[:2] - dstPoints[i]) < threshold):
                    NumInlier = NumInlier + 1
                    Inlier_pos.append(i)

        if (MaxInlier < NumInlier):
            MaxInlier = NumInlier

        NumInlier_list.append(MaxInlier)

    return NumInlier_list

def calProbability(MatchesDist):
    MatchesDist = np.array(MatchesDist)

    m, std = np.mean(np.array(MatchesDist)), np.std(np.array(MatchesDist))
    distribution = stats.norm(m, std)

    p = distribution.pdf(MatchesDist)
    p /= np.sum(p)
    return p

def fastRANSAC(matches_pos, MatchesDist):
    '''
        Fit the homography model with fast RANSAC algorithm
    '''
    dstPoints = [] # i.e. left image(destination image)
    srcPoints = [] # i.e. right image(source image) 
    for dstPoint, srcPoint in matches_pos:
        dstPoints.append(list(dstPoint)) 
        srcPoints.append(list(srcPoint))
    dstPoints = np.array(dstPoints)
    srcPoints = np.array(srcPoints)
    
    p = calProbability(MatchesDist)

    # Fast RANSAC algorithm, selecting the best fit homography
    NumSample = len(matches_pos)
    threshold = 5.0  
    NumIter = 200
    NumRamdomSubSample = 4
    MaxInlier = 0
    NumInlier_list = []

    for run in range(NumIter):
        SubSampleIdx = np.random.choice(range(NumSample), size=NumRamdomSubSample, replace=False, p=p)
        H = solve_homography(srcPoints[SubSampleIdx], dstPoints[SubSampleIdx])
        
        # find the best Homography have the the maximum number of inlier
        NumInlier = 0 
        Inlier_pos = []
        for i in range(NumSample):
            if i not in SubSampleIdx:
                concateCoor = np.hstack((srcPoints[i], [1])) # add z-axis as 1
                dstCoor = H @ concateCoor.T # calculate the coordination after transform to destination img 
                if dstCoor[2] <= 1e-8: # avoid divide zero number, or too small number cause overflow
                    continue
                dstCoor = dstCoor / dstCoor[2]
                if (np.linalg.norm(dstCoor[:2] - dstPoints[i]) < threshold):
                    NumInlier = NumInlier + 1
                    Inlier_pos.append(i)

        if (MaxInlier < NumInlier):
            MaxInlier = NumInlier

        NumInlier_list.append(MaxInlier)
    
    return NumInlier_list


def main(args):

    img_left = cv2.imread(args.img1_path)
    img_right = cv2.imread(args.img2_path)

    if not os.path.exists(args.save_path):
        os.makedirs(args.save_path)

    print("Detect keypoints...")
    kps_l, features_l = detect(img_left, args.method)
    kps_r, features_r = detect(img_right, args.method)

    (matches_pos, MatchesDist) = matchKeyPoint(kps_l, kps_r, features_l, features_r, args.ratio)
    base_count = np.zeros(200)
    fast_count = np.zeros(200)
    iters = 200

    for i in range(iters):
        NumInlier_list_base = RANSAC(matches_pos)
        NumInlier_list_fast = fastRANSAC(matches_pos, MatchesDist)

        base_count += np.array(NumInlier_list_base)
        fast_count += np.array(NumInlier_list_fast)

    base_count /= iters
    fast_count /= iters

    # plot the distribution of matching scores
    fig = plt.figure(figsize= (8, 6))
    ax = plt.gca()
    ax.hist(MatchesDist)

    ax.spines['right'].set_visible(False)
    ax.spines['top'].set_visible(False)

    ax.spines['bottom'].set_color('dimgray')
    ax.spines['left'].set_color('dimgray')
    fig.savefig(os.path.join(args.save_path, "hist.png"))

    # plot the comparison of RANSACE and fast version
    fig = plt.figure(figsize= (8, 6))
    ax = plt.gca()
    ax.plot(base_count, label="base")
    ax.plot(fast_count, label="fast")
    plt.xlabel("Iteration", fontsize=20)
    plt.ylabel("Inlier Number", fontsize=20)

    ax.spines['right'].set_visible(False)
    ax.spines['top'].set_visible(False)

    ax.spines['bottom'].set_color('dimgray')
    ax.spines['left'].set_color('dimgray')
    ax.legend(prop={'size': 14})
    ax.grid()
    fig.savefig(os.path.join(args.save_path, "curve.png"))

if __name__ == "__main__":

    parser = argparse.ArgumentParser()

    parser.add_argument("--img1_path", type = str, default = "scene/book1.jpg")
    parser.add_argument("--img2_path", type = str, default = "scene/scene.jpg")
    parser.add_argument("--method", type = str, default = 'sift')
    parser.add_argument("--ratio", type = float, default = 0.75)
    parser.add_argument("--save_path", type = str, default = "result/book1/sift")

    args = parser.parse_args()

    main(args)
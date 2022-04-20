import numpy as np
import random
import scipy.stats as stats

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
    NumIter = 1000
    NumRamdomSubSample = 4
    MaxInlier = 0
    Best_iter = None
    Best_H = None
    BestInlier_pos = []

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
            Best_iter = run + 1
            MaxInlier = NumInlier
            Best_H = H
            Inlier_pos.extend(SubSampleIdx)
            BestInlier_pos = Inlier_pos

    print("The Number of Maximum Inlier: ", MaxInlier, " Interation: ", Best_iter)
    
    Best_Inlier_pos = [matches_pos[i] for i in BestInlier_pos]
    
    return Best_H, Best_Inlier_pos

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
    NumIter = 1000
    NumRamdomSubSample = 4
    MaxInlier = 0
    Best_H = None
    Best_iter = None
    BestInlier_pos = []
    
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
            Best_iter = run + 1
            MaxInlier = NumInlier
            Best_H = H
            Inlier_pos.extend(SubSampleIdx)
            BestInlier_pos = Inlier_pos

    print("The Number of Maximum Inlier: ", MaxInlier, " Interation: ", Best_iter)
    
    Best_Inlier_pos = [matches_pos[i] for i in BestInlier_pos]
    
    return Best_H, Best_Inlier_pos
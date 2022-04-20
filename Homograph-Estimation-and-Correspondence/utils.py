import cv2
import numpy as np
import matplotlib.pyplot as plt

def detect(img, mode="sift"):
    '''
    The Detector and Descriptor
    '''
    # SIFT detector and descriptor
    if mode == "sift":
        detector = cv2.xfeatures2d.SIFT_create()
    else:
        detector = cv2.xfeatures2d.SURF_create()

    kps, features = detector.detectAndCompute(img,None)
    
    return kps, features

def matchKeyPoint(kps_l, kps_r, features_l, features_r, ratio):
    '''
        Match the Keypoints beteewn two image
    '''
    Match_idxAndDist = [] # [min corresponding index, min distance, seccond min corresponding index, second min distance]
    for i in range(len(features_l)):
        min_IdxDis = [-1, np.inf]  # record the min corresponding index, min distance
        secMin_IdxDis = [-1 ,np.inf]  # record the second corresponding min index, min distance
        for j in range(len(features_r)):
            dist = np.linalg.norm(features_l[i] - features_r[j])
            if (min_IdxDis[1] > dist):
                secMin_IdxDis = np.copy(min_IdxDis)
                min_IdxDis = [j , dist]
            elif (secMin_IdxDis[1] > dist and secMin_IdxDis[1] != min_IdxDis[1]):
                secMin_IdxDis = [j, dist]
        
        Match_idxAndDist.append([min_IdxDis[0], min_IdxDis[1], secMin_IdxDis[0], secMin_IdxDis[1]])

    # ratio test as per Lowe's paper
    # reject the point if ||f1 - f2 || / || f1 - f2' || >= ratio, that represent it's ambiguous point
    goodMatches = []
    goodMatchesDist = []
    for i in range(len(Match_idxAndDist)):
        if (Match_idxAndDist[i][1] <= Match_idxAndDist[i][3] * ratio):
            goodMatches.append((i, Match_idxAndDist[i][0]))
            goodMatchesDist.append(Match_idxAndDist[i][1])

    goodMatches_pos = []
    for (idx, correspondingIdx) in goodMatches:
        psA = (int(kps_l[idx].pt[0]), int(kps_l[idx].pt[1]))
        psB = (int(kps_r[correspondingIdx].pt[0]), int(kps_r[correspondingIdx].pt[1]))
        goodMatches_pos.append([psA, psB])
        
    return goodMatches_pos, goodMatchesDist


def drawMatches(imgs, matches_pos):
    '''
        Draw the match points img with keypoints and connection line
    '''
    
    # initialize the output visualization image
    img_left, img_right = imgs
    (hl, wl) = img_left.shape[:2]
    (hr, wr) = img_right.shape[:2]
    vis = np.zeros((max(hl, hr), wl + wr, 3), dtype="uint8")
    vis[0:hl, 0:wl] = img_left
    vis[0:hr, wl:] = img_right
    
    # Draw the match
    for (img_left_pos, img_right_pos) in matches_pos:
        pos_l = img_left_pos
        pos_r = img_right_pos[0] + wl, img_right_pos[1]
        cv2.circle(vis, pos_l, 3, (255, 0, 0), 2)
        cv2.circle(vis, pos_r, 3, (255, 0, 0), 2)
        cv2.line(vis, pos_l, pos_r, (0, 255, 0), 1)
            
    # return the visualization
    plt.figure(1)
    plt.imshow(vis[:,:,::-1])
    #cv2.imwrite("Feature matching img/matching.jpg", vis)
    
    return vis

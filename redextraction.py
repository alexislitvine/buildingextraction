import cv2 as cv
import numpy as np


def extractRColor(src):
    """Extract the red componenet in a image and display it"""

    # src = cv.imread(filename)
    src_hsv = cv.cvtColor(src, cv.COLOR_BGR2HSV)

    #src_hsv = cv.blur(src_hsv, (3,3), 0)

    # define ranges of red color in HSV
    lower_red1 = np.array([160,40,140])
    upper_red1 = np.array([180,255,255])

    lower_red2 = np.array([0,40,140])
    upper_red2 = np.array([12,255,255])

    # Threshold the HSV image to get only blue colors
    mask = cv.add(cv.inRange(src_hsv, lower_red1, upper_red1), 
                cv.inRange(src_hsv, lower_red2, upper_red2))

    # Bitwise-AND mask and original image
    #res = cv.bitwise_and(src,src, mask= mask)

    # kernel pour ouverture (remove noise) 2 kernels possibles 
    kernel = cv.getStructuringElement(cv.MORPH_ELLIPSE, (5, 5))
    kernel_2 = np.ones((2,2), np.uint8)
    kernel_hm = np.array((
        [0, -1, 0],
        [-1, 1, -1],
        [0, -1, 0]), dtype="int")

    kernel_hm_diag = np.array((
        [-1, 0, -1],
        [0, 1, 0],
        [-1, 0, -1]), dtype="int")

    #e_im = cv.erode(mask, kernel_2, iterations=1)
    res_hm = cv.morphologyEx(mask, cv.MORPH_HITMISS, kernel_hm)
    res_hm_diag = cv.morphologyEx(mask, cv.MORPH_HITMISS, kernel_hm_diag)
    #res1 = cv.dilate(e_im, kernel_2, iterations=1) 
    #res = cv.dilate(res1, kernel_2, iterations=1) 

    # cv.imshow('frame',src)
    #cv.imshow('mask',res1)
    # cv.imshow('res',res)
    #cv.waitKey(0)

    #im_path='Image/Gold_standard/test44_filtre_hm_diagx2.png'
    mask = cv.subtract(mask, res_hm)
    mask = cv.subtract(mask, res_hm_diag)

    # Application twice
    res_hm = cv.morphologyEx(mask, cv.MORPH_HITMISS, kernel_hm)
    res_hm_diag = cv.morphologyEx(mask, cv.MORPH_HITMISS, kernel_hm_diag)
    mask = cv.subtract(mask, res_hm)
    mask = cv.subtract(mask, res_hm_diag)

    #res = cv.erode(mask, kernel_2, iterations=1)
    #mask2 = cv.dilate(res, kernel_2, iterations=1) 

    #cv.imwrite(im_path, mask)
    
    #step for OS survey dataset
    lower_white = np.array([253,253,253])
    upper_white = np.array([255,255,255])
    src_bg = cv.inRange(src, lower_white, upper_white)  #directement sortie en  1 dimensions
    src_final = 255 - cv.add(255 - mask, src_bg)

    return src_final


def colorExtraction_predict(images):
    """Prediction with color extraction (red component)"""

    # check if dimensions of images is correct
    print(images.shape)

    # predict and append to a list
    res = []
    for i in range(len(images)):
        mask_i = extractRColor(images[i])
        mask_i = np.array(mask_i)
        mask_i = np.expand_dims(mask_i, axis=-1)    #voir si utile
        mask_i = mask_i > 0
        res.append(mask_i)
    
    res = np.array(res)

    return res

def Jahanshahi(src_bin):

    src_bin = cv.bitwise_not(src_bin)

    l = 5
    # création des kernels dans plusieurs directions
    element0 = np.ones((l,l), dtype="uint8")*(-1)
    element45 = np.ones((l,l), dtype="uint8")*(-1)
    element90 = np.ones((l,l), dtype="uint8")*(-1)
    element135 = np.ones((l,l), dtype="uint8")*(-1)

    for i in range(l):
        element0[int(l / 2) -1][i] = 0
        element0[int(l / 2) +1][i] = 0
        element0[int(l / 2)][i] = 1

        element90[i][int(l / 2) -1] = 0
        element90[i][int(l / 2) +1] = 0
        element90[i][int(l / 2)] = 1

        if i < l - 1:
            element45[i+1][i] = 0
            element135[i][l - i - 2] = 0
        if i > 0:
            element45[i-1][i] = 0
            element135[i][l - i] = 0
        
        element45[i][i] = 1
        element135[i][l-i-1] =  1
    
    print(f'element0 = {element0}')
    print(f'element45 = {element45}')
    print(f'element90 = {element90}')
    print(f'element135 = {element135}')

    # Ouverture
    morph = cv.morphologyEx(src_bin, cv.MORPH_HITMISS, element0)
    #morph_temp = cv.morphologyEx(src_bin, cv.MORPH_HITMISS, element45)
    #cv.max(morph, morph_temp, morph)
    morph_temp = cv.morphologyEx(src_bin, cv.MORPH_HITMISS, element90)
    cv.max(morph, morph_temp, morph)
    #morph_temp = cv.morphologyEx(src_bin, cv.MORPH_HITMISS, element135)
    cv.max(morph, morph_temp, morph)

    # Fermeture
    # morph_temp = cv.morphologyEx(morph, cv.MORPH_CLOSE, element0)
    # morph_temp2 = cv.morphologyEx(morph, cv.MORPH_CLOSE, element45)
    # cv.max(morph_temp, morph_temp2, morph_temp)
    # morph_temp2 = cv.morphologyEx(morph, cv.MORPH_CLOSE, element90)
    # cv.max(morph_temp, morph_temp2, morph_temp)
    # morph_temp2 = cv.morphologyEx(morph, cv.MORPH_CLOSE, element135)
    # cv.max(morph_temp, morph_temp2, morph_temp)

    res = cv.max(cv.subtract(morph_temp, src_bin), src_bin)

    cv.imshow('result Jahanshahi', morph_temp)
    cv.waitKey(0)

    return cv.bitwise_not(res)


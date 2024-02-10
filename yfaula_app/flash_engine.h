#ifndef FLASHDETECTION_FLASH_ENGINE_H_
#define FLASHDETECTION_FLASH_ENGINE_H_



#include "opencv.hpp"
#include "flash_data_structure.h"
#include <bitset>

using namespace std;
using namespace cv;

void flash_process16(const Mat& src, 
	Structure_Points& detected_pts, 
	const Mat& MASK,
	int BETA, int _MIN_SIZE,
	int _NB_FRIENDS_MAX = 5, 
	int _DIFF_LEVEL_SIZE = 255,
	int _IDPALIER = 0,
	double _TH_EIGEN = 0.5,
	double _TH_LINEC = 50,
	double _ANGLE_SEARCH = 45);


#endif // !FLASHDETECTION_FLASH_ENGINE_H_
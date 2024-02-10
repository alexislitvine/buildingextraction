#ifndef FLASHDETECTION_PARAMS_HELPER_H_
#define FLASHDETECTION_PARAMS_HELPER_H_

#include <iostream>
#include <string>
#include <map>

using namespace std;

typedef map<string, double> Params;

//parameters definition for flash algorithm
/*//default parameters
_BETA = 10;
_NB_FRIENDS_MAX = 5;
_MIN_SIZE_GRAPH = 10;
_WITH_GRAPH = 0;
//_CRACK_MIN_SIZE = 50;
_DIFF_LEVEL_SIDE = 255;
_BonW = 1;
_RESOL_LVL = 1;
_THRESH_REL_EIGEN = 0.5;
_THRESH_LINE_CRITER = 30;
_ANGLE_SEARCH = 45;
*/

string getStringParams(Params params);

Params setParams(string params_str);


#endif // !FLASHDETECTION_PARAMS_HELPER_H_


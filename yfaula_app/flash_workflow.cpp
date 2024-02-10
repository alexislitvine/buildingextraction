#include <ctime>
#include <codecvt>
#include "flash_workflow.h"

#ifdef _WIN64
  #include <direct.h>
#endif // !_WIN64

#ifdef __gnu_linux__
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif // !__gnu_linux__


using namespace cv;


//flash string key parameters
#define _BETA_                "BETA"
#define _NB_FRIENDS_MAX_      "NB_FRIENDS_MAX"
#define _WITH_GRAPH_          "WITH_FUSION"
#define _MIN_SIZE_GRAPH_      "MIN_SIZE_GRAPH"
#define _DIFF_LEVEL_SIDE_     "DIFFERENCE_LEVEL_SIDES"
#define _XFACT_INTER_           "FACTOR_INTERVAL"
#define _XF_X2ORI_						"X_FACTOR_X2ORIGIN"
#define _BonW_                "BLACK_ON_WHITE?"
#define _THRESH_REL_EIGEN_    "THRESHOLD_REL_EIGEN"
#define _THRESH_LINE_CRITER_  "THRESHOLD_LINE_CRITER"
#define _ANGLE_SEARCH_        "ANGLE_SEARCH"
#define _IDPALIER_			  "ID_PALIER"


#ifdef _WIN32
static const char kPathSeparator = '\\';
#endif // !_WIN32

#ifdef __gnu_linux__
static const char kPathSeparator = '/';
#endif // !__gnu_linux__

string getFileNameWithoutPath(const string& full_name)
{
  string name = full_name;

  size_t pos = name.find_last_of(kPathSeparator);	//TODO : consider other separators like "/" or "\\"
  if (pos != string::npos)
  {
    name.erase(0, pos + 1);
  }

  pos = name.find_last_of('.');
  if (pos != string::npos)
  {
    name.erase(pos, string::npos);
  }

  return name;
}


void flash_detection_workflow(Params params, string output_directory, string source_name)
{
	int compteur = 0;
	int nb_resol;
	cv::Mat src_origin = cv::imread(source_name.c_str());

	//Scalar colors[7] = { Scalar(20,20,255), Scalar(43,119,255), Scalar(43,233,255), Scalar(43,255,120),
    //          Scalar(255,244,43), Scalar(255,43,60), Scalar(255,43,212) };

	Mat display_lines;
	display_lines = Mat::zeros(src_origin.size(), CV_8UC1);

	for(auto sfactor = params[_XF_X2ORI_]; sfactor <= 1.00; sfactor += params[_XFACT_INTER_])
		compteur++;
	nb_resol = compteur;

	for(auto sfactor = params[_XF_X2ORI_]; sfactor <= 1.00; sfactor += params[_XFACT_INTER_])
	{
		cv::Mat src;
		cv::cvtColor(src_origin, src, COLOR_BGR2GRAY);
		if(params[_BonW_] <= 0)
			src = 255 - src;
		

		//redimensionnement �ventuel
		if (params[_XF_X2ORI_] < 1.0)
		{
				resize(src, src, Size(), sfactor, sfactor, INTER_NEAREST);
		}

		//structure de donn�es
		Structure_Points detected_points = Structure_Points(src.rows, src.cols);
			
		/** Processus principal **/
		flash_process16(src,
			detected_points, 
			cv::Mat(), 
			(int)params[_BETA_],
			(int)params[_MIN_SIZE_GRAPH_],
			(int)params[_NB_FRIENDS_MAX_],
			(int)params[_DIFF_LEVEL_SIDE_],
			(int)params[_IDPALIER_], 
			params[_THRESH_REL_EIGEN_], 
			params[_THRESH_LINE_CRITER_],
			params[_ANGLE_SEARCH_]);
			
		//récupération des résultats
		for (Graph_DPt& subgraph : detected_points.graphs_)
		{
			if (subgraph.length > params[_MIN_SIZE_GRAPH_] && subgraph.m_relationEigenVal > params[_THRESH_REL_EIGEN_])
			{
				for (D_Point* pt : subgraph.d_pts)
				{
					Point2f temp = Point2f( (1/sfactor)*pt->pt.x - (nb_resol / 2), 
											(1/sfactor)*pt->pt.y - (nb_resol / 2) );
					Size s_temp = Size(nb_resol, nb_resol);

					//rectangle(display_lines, Rect(Point(temp), s_temp), colors[nb_resol-1], -1);
					rectangle(display_lines, Rect(Point(temp), s_temp), Scalar(255), -1);
				}
			}
		} 
		nb_resol--;
	}	//end boucle dimensions images diff
	//

	//for historical cartography
	cv::Mat src, results;
	cv::cvtColor(src_origin, src, COLOR_BGR2GRAY);
	bitwise_and(display_lines, src, results);

	string fn_withoutPath = getFileNameWithoutPath(source_name);
	imwrite(output_directory + "/" + fn_withoutPath + "_flashlines" + ".png", results);
		
}

Params flash::getDefaultParams()
{
  Params default_params;

  default_params[_BonW_] = 1;
  default_params[_BETA_] = 8;
  default_params[_DIFF_LEVEL_SIDE_] = 40;
  default_params[_MIN_SIZE_GRAPH_] = 20;
  default_params[_ANGLE_SEARCH_] = 22.5;
  default_params[_THRESH_REL_EIGEN_] = 0.5;
  default_params[_THRESH_LINE_CRITER_] = 100;
  default_params[_XF_X2ORI_] = 1;
  default_params[_XFACT_INTER_] = 0.25;
  default_params[_NB_FRIENDS_MAX_] = 10;
  default_params[_IDPALIER_] = 0;

  return default_params;
}

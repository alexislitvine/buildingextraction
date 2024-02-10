#include "flash_engine.h"

using namespace std;
using namespace cv;

void flash_process16(const Mat& src, 
	Structure_Points& detected_pts, 
	const Mat& MASK,
	int BETA, 
	int _MIN_SIZE,
	int _NB_FRIENDS_MAX,
	int _DIFF_LEVEL_SIDE,
	int _IDPALIER,
	double _TH_EIGEN,
	double _TH_LINEC,
	double _ANGLE_SEARCH)
{
	vector<Point> circle_pts;
	circle_pts.push_back(Point(-3, 0));
	circle_pts.push_back(Point(-3, -1));
	circle_pts.push_back(Point(-2, -2));
	circle_pts.push_back(Point(-1, -3));
	circle_pts.push_back(Point(0, -3));
	circle_pts.push_back(Point(1, -3));
	circle_pts.push_back(Point(2, -2));
	circle_pts.push_back(Point(3, -1));

	circle_pts.push_back(Point(3, 0));
	circle_pts.push_back(Point(3, 1));
	circle_pts.push_back(Point(2, 2));
	circle_pts.push_back(Point(1, 3));
	circle_pts.push_back(Point(0, 3));
	circle_pts.push_back(Point(-1, 3));
	circle_pts.push_back(Point(-2, 2));
	circle_pts.push_back(Point(-3, 1));

	vector<bitset<8>> error_masks;
	error_masks.push_back(bitset<8>(string("00001110")));
	error_masks.push_back(bitset<8>(string("00011100")));
	error_masks.push_back(bitset<8>(string("00111000")));
	error_masks.push_back(bitset<8>(string("01110000")));
	error_masks.push_back(bitset<8>(string("11100000")));
	error_masks.push_back(bitset<8>(string("11000001")));
	error_masks.push_back(bitset<8>(string("10000011")));
	error_masks.push_back(bitset<8>(string("00000111")));

	//variables necessaires pour la boucle
	const uchar beta = BETA;
	unsigned char compteur = 0;
	bitset<8> configurations(255);
	int i_pt;
	Point next = Point(-1, -1);

	for (int i = src.rows - 1; i >= 0; --i)
	{
		for (int j = src.cols - 1; j >= 0; --j)
		{
			if ((i - 3 >= 0 && j - 3 >= 0) && (i + 3 < src.rows && j + 3 < src.cols))
			{
				if (true)
				{
					configurations.set();
					int thd = src.at<uchar>(i, j) + beta;
					int mini = UCHAR_MAX;
					int average1 = 0, average2 = 0;

					i_pt = 0;
					compteur = 0;

					if (!((src.at<uchar>(i - 3, j) > thd && src.at<uchar>(i + 3, j) > thd) ||
						(src.at<uchar>(i, j - 3) > thd && src.at<uchar>(i, j + 3) > thd)))
						configurations.reset();

					while ((compteur < 5) && !configurations.none())
					{
						int ii = i_pt + compteur;
						int ii_sym = (ii + 8) % 16;

						average1 += src.at<uchar>(i + circle_pts[ii].y, j + circle_pts[ii].x);
						average2 += src.at<uchar>(i + circle_pts[ii_sym].y, j + circle_pts[ii_sym].x);

						if (src.at<uchar>(i + circle_pts[ii].y, j + circle_pts[ii].x) > thd
							&& src.at<uchar>(i + circle_pts[ii_sym].y, j + circle_pts[ii_sym].x) > thd)
						{
							++compteur;
						}
						else
						{
							configurations &= error_masks[ii];
							i_pt = ii + 1;
							compteur = 0;
							mini = 255;
							average1 = average2 = 0;
						}
					}

					unsigned char pix_min1 = mini, pix_min2 = mini;
					Point p1, p2;

					bool found1 = false, found2 = false;
					if (compteur == 5)
					{
						int thd2 = mini;
						int n = 5, nn = 13, step = 0;
						
						if (abs(average1 - average2) / 5 >= _DIFF_LEVEL_SIDE)	step = 3;

						while (step < 3)
						{
							if (src.at<uchar>(i + circle_pts[(i_pt + n + step) % 16].y, j + circle_pts[(i_pt + n + step) % 16].x) < pix_min1)
							{
								found1 = true;
								pix_min1 = src.at<uchar>(i + circle_pts[(i_pt + n + step) % 16].y, j + circle_pts[(i_pt + n + step) % 16].x);
								p1 = Point(j + circle_pts[(i_pt + n + step) % 16].x, i + circle_pts[(i_pt + n + step) % 16].y);
							}

							if (src.at<uchar>(i + circle_pts[(i_pt + nn + step) % 16].y, j + circle_pts[(i_pt + nn + step) % 16].x) < pix_min2)
							{
								found2 = true;
								pix_min2 = src.at<uchar>(i + circle_pts[(i_pt + nn + step) % 16].y, j + circle_pts[(i_pt + nn + step) % 16].x);
								p2 = Point(j + circle_pts[(i_pt + nn + step) % 16].x, i + circle_pts[(i_pt + nn + step) % 16].y);
							}
							++step;
						}
					}

					detected_pts.setNext(i, j, next);

					if (found1 && found2)
					{
						//calcul du score comme avec FAST normal
						unsigned int S = 0;

						float angle = std::atan2((p1 - p2).y, (p1 - p2).x) * 180 / 3.14159;
						if (angle < 0) {
							angle = angle + 180;
						}
						detected_pts.createDPoint(i, j, abs(angle), S);
						next = Point(j, i);
					}
					else if (found1 != found2)
					{
						unsigned int S = 0;

						if (found1)
						{
							S = std::min(mini - pix_min1, mini - (thd - beta));
							p2 = Point(j, i);
						}
						else
						{
							S = std::min(mini - pix_min2, mini - (thd - beta));
							p1 = Point(j, i);
						}

						float angle = std::atan2((p1 - p2).y, (p1 - p2).x) * 180 / 3.14159;
						if (angle < 0) {
							angle = angle + 180;
						}

						detected_pts.createDPoint(i, j, abs(angle), S);
						next = Point(j, i);
					}
					else if (!found1 && !found2 && compteur == 5)
					{
						unsigned int S = 0;

						detected_pts.createDPoint(i, j, -181, S);
						next = Point(j, i);
					}
				}
				else
				{
					detected_pts.setNext(i, j, next);
				}
			}
			else
			{
				detected_pts.setNext(i, j, next);
			}
		}
	}

	vector<pair<int, int>> paliers;
	if(_IDPALIER == 0)
		paliers = { make_pair(3, 2), make_pair(6, 3), make_pair(9, 5) };
	else if(_IDPALIER == 1)
		paliers = { make_pair(4, 2), make_pair(8, 4), make_pair(12, 6) };
	else if(_IDPALIER == 2)
		paliers = { make_pair(8, 2), make_pair(14, 4), make_pair(20, 6) };
	else if(_IDPALIER < 0)
		paliers = { make_pair(3, 2), make_pair(6, 3), make_pair(9, 5) , make_pair(15, 8) };
	else
		paliers = { make_pair(3, 2), make_pair(6, 3), make_pair(9, 5) };


	int NB_FRIENDS_MAX = _NB_FRIENDS_MAX*2;
	int NB_LEFT_MAX = NB_FRIENDS_MAX;
	int NB_RIGHT_MAX = NB_FRIENDS_MAX;

	int ami_precedent = 0;
	int palier_pre = 0;
	D_Point* current;

	for (pair<int, int> palier : paliers)
	{

		int nb_level = palier.first;
		current = detected_pts.next(true);
		while (current != NULL)
		{
			int nb_left = 0;
			int nb_right = 0;

			if (current->neighbors.size() >= ami_precedent)
			{
				int level = palier_pre + 1;

				while (current->neighbors.size() < NB_FRIENDS_MAX && level < nb_level)
				{
					list<D_Point*> temp_found_pts;
					detected_pts.getBestCirclingNeighboors(current, level, temp_found_pts);
					
					list<D_Point*>::iterator it = temp_found_pts.begin();
					list<D_Point*>::iterator it_end = temp_found_pts.end();

					for (; it != it_end; ++it)
					{
						int res = or_is_it_ok2(current->pt, current->angle , (*it)->pt, (*it)->angle , _ANGLE_SEARCH);
						if (res == 1)
						{
							if (nb_left <= NB_LEFT_MAX)
								current->neighbors.push_back(make_pair(1, (*it)));
							++nb_left;
						}
						else if (res == 2)
						{
							if (nb_right <= NB_RIGHT_MAX)
								current->neighbors.push_back(make_pair(2, (*it)));
							++nb_right;
						}
					}
					++level;
				}
				current->nb_left += nb_left;
				current->nb_right += nb_right;
			}
			current = detected_pts.next();
		}
		ami_precedent = palier.second;
		palier_pre = palier.first;
	}


	current = detected_pts.next(true);

	while (current != NULL)
	{
		list<pair<int, D_Point*>>::iterator it = current->neighbors.begin();
		for (; it != current->neighbors.end(); )
		{
			if (isElem(it->second->neighbors, make_pair(1, current)) || isElem(it->second->neighbors, make_pair(2, current)))
			{
				++it;	
			}
			else
			{
				if (it->first == 1)
					current->nb_left--;
				else
					current->nb_right--;
				
				it = current->neighbors.erase(it);
			}
		}
		current = detected_pts.next();
	}

	detected_pts.buildGraphs();

	detected_pts.displaygraphs_(_MIN_SIZE);

	//detected_pts.cleanGraphs(1, _TH_EIGEN, _TH_LINEC);

	detected_pts.cleanGraphs(_MIN_SIZE, _TH_EIGEN, _TH_LINEC);
}


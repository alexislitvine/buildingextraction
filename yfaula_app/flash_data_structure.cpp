#include "flash_data_structure.h"

int Graph_DPt::compteur = 0;

Structure_Points::Structure_Points() {
	data_ = NULL;
}

static Point vect_ors[8] = { Point(-1,0),
Point(-2,-1),
Point(-1,-1),
Point(-1,-2),
Point(0,-1),
Point(1,-2),
Point(1,-1),
Point(2,-1) };


bool isVectFace2Face(Point v, double or1, double or2)
{
	//1ere conditions
	//les vecteurs se font face => le cos < 0
	//2eme condition
	//cf function or_is_it_ok2 ==> ne suffit pas
	double v_or = atan2(v.y, v.x)*180/3.14159;
	if (v_or < 0) v_or += 180;

	if (abs(v_or - or1) >= 90)	return false;

	double or1_rad = or1*3.14159 / 180.0;
	double or2_rad = or2*3.14159 / 180.0;

	Point2f v1 = Point2f(cos(or1_rad), sin(or1_rad));
	Point2f v2 = Point2f(cos(or2_rad), sin(or2_rad));

	return  (v1.x*v2.x + v1.y*v2.y) < 0;
}

double cos2v(Point u, Point v)
{
  double norm_u = sqrt(u.x*u.x + u.y*u.y);
  double norm_v = sqrt(v.x*v.x + v.y*v.y);

  if (norm_u*norm_v != 0)
    return (u.x*v.x + u.y*v.y) / (norm_u*norm_v);
  else
    return 0;
}

pair<double, double> meanAngles(const vector<double>& angles)
{
  double sn_sum = 0;
  double cs_sum = 0;

  for (auto i = 0; i < angles.size(); ++i)
  {
    //angles must be between 0 and 179
    sn_sum += std::sin(angles[i] * 2 / 180 * 3.14159);
    cs_sum += std::cos(angles[i] * 2 / 180 * 3.14159);
  }

  double res = (std::atan2(sn_sum, cs_sum) * 180 / 3.14159) / 2;
  if (res < 0)	res = res + 180;

  double R_mean = std::sqrt(sn_sum*sn_sum + cs_sum*cs_sum) / angles.size();

  return make_pair(res, R_mean);
}

//on regarde si les orientations sont correctes
//CAUTION : ici pour les points
int or_is_it_ok2(Point pt1, float or1, Point pt2, float or2, double tol)
{
  int result = 0;
  Point vecteur = pt2 - pt1;

  double angle_vecteur = std::atan2(vecteur.y, vecteur.x) * 180 / 3.14159;
  if (angle_vecteur < 0) angle_vecteur = 180 + angle_vecteur;

  if ((angle_vecteur - or1) < 0)
  {
    result = 1;
    //cout << "DROITE" << endl;
  }


  double ecart1 = std::min(abs(angle_vecteur - or1), abs((180 - angle_vecteur) - or1));
  double ecart2 = std::min(abs(angle_vecteur - or2), abs((180 - angle_vecteur) - or2));
  if (ecart1 < tol && ecart2 < tol)
    return (result + 1);
  else
    return 0;
}

//CAUTION : ici pour les graphes
//entre 2 points extrémités et les vecteurs directeurs du nuage de points
int or_is_it_ok4Graph(Point pt1, Point v1, Point pt2, Point v2)
{
  int result = 0;
  Point vecteur = pt2 - pt1;

  double cos1 = cos2v(vecteur, v1);
  double cos2 = cos2v(vecteur, v2);

  if (abs(cos1) > 0.8 && abs(cos2) > 0.8)
    return (result + 1);
  else
    return 0;
}


Structure_Points::Structure_Points(int nb_rows, int nb_cols) :nb_rows_(nb_rows), nb_cols_(nb_cols) {
	data_ = new Struct_Point*[nb_rows];
	for (int i = 0; i < nb_rows; ++i)
		data_[i] = new Struct_Point[nb_cols];
}

D_Point* Structure_Points::createDPoint(int row, int col, float angle , unsigned int score) {
	if (data_)
	{
		D_Point* pt = new D_Point;
		pt->angle = angle ;
		pt->pt = Point(col, row);
		pt->marked = false;
		pt->id_graph = -1;
		pt->nb_left = 0;
		pt->nb_right = 0;
		pt->score = score;

		data_[row][col].dpt = pt;
		return pt;
	}
	else
		return NULL;
}

D_Point* Structure_Points::createDPoint(const D_Point* aDpt, int ratio) {
	if (data_)
	{
		D_Point* pt = new D_Point;
		pt->angle = aDpt->angle ;
//		pt->pt = aDpt->pt / ratio;
                //compatibility with opencv 249
       		pt->pt.x = aDpt->pt.x / ratio;
		pt->pt.y = aDpt->pt.y / ratio;
		pt->marked = false;
		pt->id_graph = aDpt->id_graph;
		pt->nb_left = aDpt->nb_left;
		pt->nb_right = aDpt->nb_right;
		pt->score = aDpt->score;
		/*pt->neighbors = aDpt->neighbors;*/

		int newx = pt->pt.x / ratio;
		int newy = pt->pt.y / ratio;

		data_[newy][newx].dpt = pt;
		return pt;
	}
	else
		return NULL;
}

//default : restart = false
D_Point* Structure_Points::next(bool restart, bool crack_point) {
	bool found = false;
	D_Point* res = NULL;
	if (restart)
	{
		current_ = Point(0, 0);
	}

	current_ = data_[current_.y][current_.x].next;

	while (!found && current_ != Point(-1, -1))
	{
		if (current_ != Point(-1, -1))
		{
			if (data_[current_.y][current_.x].dpt)
			{
				if (crack_point)
				{
					if (data_[current_.y][current_.x].dpt->angle >= 0)
					{
						res = data_[current_.y][current_.x].dpt;
						found = true;
					}
					else
						current_ = data_[current_.y][current_.x].next;
				}
				else
				{
					res = data_[current_.y][current_.x].dpt;
					found = true;
				}
			}
			else
			{
				current_ = data_[current_.y][current_.x].next;
			}
		}
	}

	return res;
}


//trouve les points sur la ligne <row> entre les colonnes <col1> et <col2>
//met les points trouves dans la liste <found_pts>
inline void Structure_Points::searchBySegment(int row, int col1, int col2, list<D_Point*>& found_pts) {
	int cur = col1;

	while (cur <= col2 && cur != -1)
	{
		if (data_[row][cur].dpt)
			found_pts.push_back(data_[row][cur].dpt);
		//on peut avoir la meme colonne sur 2 lignes diff�rentes
		cur = data_[row][cur].next.y == row ? data_[row][cur].next.x : -1;
	}
}

void Structure_Points::getBestCirclingNeighboors(D_Point* dpt, int rayon, list<D_Point*>& found_pts, bool crack_points) {
	int col1 = dpt->pt.x - rayon >= 0 ? dpt->pt.x - rayon : 0;
	int col2 = dpt->pt.x + rayon < nb_cols_ ? dpt->pt.x + rayon : nb_cols_ - 1;

	int row_start = dpt->pt.y - rayon >= 0 ? dpt->pt.y - rayon : 0;						//Be carefull with bounds
	int row_max = dpt->pt.y + rayon < nb_rows_ ? dpt->pt.y + rayon : nb_rows_ - 1;	//be carefull with bounds

																						//recherche sur les lignes
	searchBySegment(row_start, col1, col2, found_pts);
	searchBySegment(row_max, col1, col2, found_pts);

	//recherche sur les points colonnes
	//TODO : possibilit� de sauter plusieurs lignes
	for (int row = row_start + 1; row < row_max ; )
	{
		if (data_[row][col1].dpt)
		{
			found_pts.push_back(data_[row][col1].dpt);
		}
		Point temp = data_[row][col1].next;
		if (temp.y > row)
			row = temp.y;
		else
		{
			if (data_[row][col2].dpt)
			{
				found_pts.push_back(data_[row][col2].dpt);
			}
			row++;
		}
	}
	if (crack_points)
		found_pts.remove_if([](const D_Point* dpt) {return (dpt->angle < 0); });
}

void Structure_Points::bfsCutEdges(list<D_Point*>& visited, D_Point* cur, int ID)
{
	list<D_Point*> f;
	int depth_max = 1;
	int compteur;
	f.push_front(cur);

	cur->marked = true;
	cur->id_graph = ID;

	while (!f.empty())
	{
		compteur = 0;
		cur = f.back();
		visited.push_back(cur);
		f.pop_back();

		//pour boucle des voisins
		list<pair<int, D_Point*>>::iterator it = cur->neighbors.begin();
		for (; it != cur->neighbors.end();)
		{
			if (!(*it).second->marked)
			{
				f.push_front((*it).second);
				(*it).second->marked = true;
				(*it).second->id_graph = ID;
				++it;
				++compteur;	//useless
			}
			else
			{
				//it = cur->neighbors.erase(it);
				++it;
			}
		}
		if (compteur != 0) depth_max++;	//useless and false
	}

	//return depth_max;
}

void Structure_Points::buildGraphs() {
	D_Point* current = this->next(true);

	while (current != NULL)
	{
		Graph_DPt subgraph;

		if (!current->marked)
		{
			bfsCutEdges(subgraph.d_pts, current, subgraph.ID);
			//calcul des eigen values
			subgraph.relationEigenVal();
			//OPTIONAL computing average distance 4 nearest points
			//TODO...
			this->graphs_.push_back(subgraph);
		}

		current = this->next();
	}
}

Mat Structure_Points::buildMask(int radius)
{
	Mat output = Mat::zeros(nb_rows_, nb_cols_, CV_8UC1);
	//output = 255;

	for (Graph_DPt& subgraph : graphs_)
	{
		for (auto i = 0; i < subgraph.representation.size() - 1; ++i)
			line(output, subgraph.representation[i].pt, subgraph.representation[i + 1].pt, Scalar(100), radius);
	}

	/*namedWindow("MASK DETECTION", WINDOW_NORMAL);
	imshow("MASK DETECTION", output);
	waitKey(0);*/

	return output;
}


void Structure_Points::showOrientationHistogram(unsigned int score_min, unsigned int score_max)
{
	int histo_or[8] = { 0,0,0,0,0,0,0,0 };
	unsigned int counter = 0;

	D_Point* current = this->next(true);
	while (current)
	{
		if (current->score > score_min && current->score < score_max)
		{
			histo_or[(int)current->angle ]++;
			counter++;
		}
		current = this->next();
	}
}

//DEBUG : Affichage de la structure de donn�es
void Structure_Points::displayData(const Mat& background, unsigned int minScore) {
	Mat points;

	//cvtColor(background, points, CV_GRAY2RGB);
	background.copyTo(points);

	int MAX_VAL = 50;
	unsigned int max_score = 0;

	double step = (768 / (MAX_VAL - minScore));
	D_Point* current = this->next(true);

	while (current != 0)
	{
		if (current->score > minScore)
		{
			/*if (current->score > max_score)
				max_score = current->score;

			int val = (int)((MAX_VAL - current->score)*step);
			int n = val / 256;
			if (val > 0)
			{
				switch (n)
				{
				case 2:
					circle(points, current->pt, 2, Scalar(0, 0, 768 % val), 1);
					break;
				case 1:
					circle(points, current->pt, 2, Scalar(0, 768 % val, 255), 1);
					break;
				case 0:
					circle(points, current->pt, 2, Scalar(768 % val, 255, 255), 1);
					break;
				default:
					circle(points, current->pt, 2, Scalar(0, 0, 0), 1);
				}
			}
			else*/
				circle(points, current->pt, 0, Scalar(0, 0, 255), -1);
		}
		current = this->next();
	}

	//cout << "MAX_score = " << max_score << endl;

	namedWindow("DATA response", WINDOW_NORMAL);
	imshow("DATA response", points);
	//imwrite("D:\\PHD\\Media\\Results\\test_points.png", points);
	//waitKey(0);
}

void Structure_Points::displaygraphs_(int _MIN_SIZE)
{
	int compteur = 0;
	//suppression de graph inutile
	graphs_.remove_if([](const Graph_DPt& g) { return g.d_pts.size() < 2; });

	for (Graph_DPt& subgraph : graphs_)
	{
		subgraph.computeBounds2(_MIN_SIZE);
		////ajout des extremum
		//if (subgraph.length >= _MIN_SIZE)
		//{
		//	m_bounds.push_back(subgraph.bounds.first);
		//	m_bounds.push_back(subgraph.bounds.second);
		//}
	}
}

void Structure_Points::cleanGraphs(int min_l, const double r, double line_criter_max)
{
	graphs_.remove_if([&](const Graph_DPt& g) { return (g.m_relationEigenVal < r || (int)g.length < min_l 
		|| g.m_lineCriteria >= line_criter_max); });

	//graphs_.remove_if([&](const Graph_DPt& g) { return (g.m_relationEigenVal < r || (int)g.length < min_l); });


	for (Graph_DPt& subgraph : graphs_)
	{
		//ajout des extremum
		m_bounds.push_back(subgraph.bounds.first);
		m_bounds.push_back(subgraph.bounds.second);
	}

	/*list<Graph_DPt>::iterator it = graphs_.begin();
	for (; it != graphs_.end(); )
	{
		if (it->relationEigenVal < r || it->length < min_l)
		{
			it = graphs_.erase(it);
		}
		else
			it++;
	}*/
}

//preconditions : id existe
list<Graph_DPt>::iterator Structure_Points::searchGraphByID(int id)
{
	list<Graph_DPt>::iterator it_end = graphs_.end();
	list<Graph_DPt>::iterator it = graphs_.begin();

	while (it->ID != id && it != it_end)
		++it;

	return it;
}

Structure_Points::~Structure_Points() {
	Point start = Point(3, 3);
	Point cur = start;

	while (cur.x < nb_cols_ - 3 && cur.y < nb_rows_ - 3 && cur != Point(-1, -1))
	{
		if (data_[cur.y][cur.x].dpt)
			delete data_[cur.y][cur.x].dpt;

		cur = data_[cur.y][cur.x].next;
	}

	for (int i = 0; i < nb_rows_; ++i)
		delete[] data_[i];

	delete[] data_;
}

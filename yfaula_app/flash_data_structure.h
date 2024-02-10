#ifndef FLASHDETECTION_FLASH_DATA_STRUCTURE_H_
#define FLASHDETECTION_FLASH_DATA_STRUCTURE_H_


#include "opencv.hpp"
#include "flash_graph_structure.h"
#include <list>
#include <vector>
#include <fstream>

using namespace std;
using namespace cv;


template <typename T> bool isElem(list<T>& l, T x)
{
	typename list<T>::iterator it_end = l.end();
	typename list<T>::iterator it = l.begin();

	while ((it != it_end) && ((*it) != x)) ++it;

	return (it != it_end);
}

template <typename T> bool isElem(vector<T> l, T x)
{
	typename vector<T>::iterator it_end = l.end();
	typename vector<T>::iterator it = l.begin();

	while ((it != it_end) && ((*it) != x)) ++it;

	return (it != it_end);
}

/**
* class 
* class handling the detected points by the algorithm FAST crack detection
*	Some computations are available on these points. We can handle graphs_ formed by a set of points
**/
class Structure_Points
{
public:
	/** Constructors **/
	Structure_Points();
	Structure_Points(int nb_rows, int nb_cols);

	inline D_Point* getDPoint(int row, int col) {
		return data_ == NULL ? NULL : data_[row][col].dpt;
	}

	/** Create a DPoint **/
	D_Point* createDPoint(int row, int col, float angle , unsigned int score);
	D_Point* createDPoint(const D_Point* aDpt, int ratio = 1);

	inline void erase(const Point& pt){
		if (data_[pt.y][pt.x].dpt)
		{
			delete data_[pt.y][pt.x].dpt;
			data_[pt.y][pt.x].dpt = NULL;
		}
	}

	inline void setNext(int row, int col, Point next = Point(-1, -1)) {
		data_[row][col].next = next;
	}

	D_Point* next(bool restart = false, bool crack_point = true);

	//trouve les points sur la ligne <row> entre les colonnes <col1> et <col2>
	//met les points trouves dans la liste <found_pts>
	void searchBySegment(int row, int col1, int col2, list<D_Point*>& found_pts);

	void getBestCirclingNeighboors(D_Point* dpt, int rayon, list<D_Point*>& found_pts, bool crack_points = true);

	void bfsCutEdges(list<D_Point*>& visited, D_Point* cur, int ID);

	void buildGraphs();

	Mat buildMask(int radius);

	void showOrientationHistogram(unsigned int score_min = 0, unsigned int score_max = 255);

	//DEBUG : Affichage de la structure de donn�es
	void displayData(const Mat& background, unsigned int minScore = 0);

	void displaygraphs_(int _min_size);

	void cleanGraphs(int min_l, double r, double line_criter_max);

	//preconditions : <id> occurs
	list<Graph_DPt>::iterator searchGraphByID(int id);

	inline Size getSize() { return Size(nb_cols_, nb_rows_); }

	virtual ~Structure_Points();

	//affichage
	Mat m_display_graphs_;
	Mat zoom;

	//TODO : a changer acc�s s�curis� � r�aliser
	list<Graph_DPt> graphs_;

protected:
	Struct_Point** data_;
	int nb_rows_;
	int nb_cols_;
	Point current_;
	
	list<D_Point> m_bounds;
};


#endif // !FLASHDETECTION_FLASH_DATA_STRUCTURE_H_

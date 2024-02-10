#ifndef FLASHDETECTION_FLASH_GRAPH_STRUCTURE_H_
#define FLASHDETECTION_FLASH_GRAPH_STRUCTURE_H_

#include <iostream>
#include <list>
#include <vector>
#include "opencv.hpp"


using namespace cv;
using namespace std;

typedef struct Struct_Point Struct_Point;
typedef struct D_Point D_Point;
typedef struct Graph_DPt Graph_DPt;

pair<double, double> meanAngles(const vector<double>& angles);

double cos2v(Point u, Point v);
int or_is_it_ok2(Point pt1, float or1, Point pt2, float or2, double angle = 45);
int or_is_it_ok4Graph(Point pt1, Point v1, Point pt2, Point v2);

struct D_Point {
  Point pt;
  float angle;
  list< pair<int, D_Point*> > neighbors;
  int nb_left;
  int nb_right;
  int id_graph;
  bool marked;
  unsigned int score;
};



struct Struct_Point {
  Struct_Point() : dpt(NULL) {}
  Point next;
  D_Point* dpt;
};


struct Graph_DPt {
  Graph_DPt() : m_barycentre(Point(-1, -1)), m_histo_orientations(NULL) { ID = ++compteur; }

  void computeProperties() {

    m_histo_orientations = new int[8];
    for (int i = 0; i < 8; m_histo_orientations[i++] = 0);	//initialisation

                                                            /** Compute mean and  histogram of orientations **/
    nb_friends_mean = 0.0;
    for (D_Point* pt : d_pts)
    {
      nb_friends_mean += pt->neighbors.size();
      m_histo_orientations[(int)pt->angle ]++;
    }

    nb_friends_mean /= (d_pts.size() * 2);	//because every edges are counted 2 times
  }

  double lineCriteria(vector<Point2f>& pts) {
    double output = 0;

    if (pts.size() < 2)
      return -1;

    //to_debug
    /*	std::ofstream file;
    file.open("histogramme_graphs_.txt", std::ofstream::out | std::ofstream::app);
    if (file.is_open())
    {
    file << this->ID << "\n";
    }*/

    double norm_u = std::sqrt(vect_princ.x*vect_princ.x + vect_princ.y*vect_princ.y);
    double cs = -vect_princ.x / norm_u;
    double sn = vect_princ.y / norm_u;

    for (auto i = 0; i < pts.size() - 1; ++i)
    {
      double min_val = DBL_MAX;
      //double dist_min_val;
      int x_i = (int)(cs*pts[i].x + sn*pts[i].y);
      int min_ind = (int)std::min((double)(pts.size()), (double)(i + 20));
      //todo do_while
      //for (auto j = i+1; j < min_ind && (cs*pts[j].x + sn*pts[j].y) < x_i+5; j++)
      //for (auto j = i + 1; j < min_ind; j++)
      auto j = i + 1;
      do
      {
        Point2f v12 = pts[j] - pts[i];

        if (min_val > (cv::norm<double>(v12)*cv::norm<double>(v12))*(1 - (cos2v(v12, vect_princ)*cos2v(v12, vect_princ))))
        {
          min_val = (cv::norm<double>(v12)*cv::norm<double>(v12))*(1 - (cos2v(v12, vect_princ)*cos2v(v12, vect_princ)));
          //dist_min_val = cv::norm<double>(v12);
        }
        j++;
      } while (j < min_ind && (cs*pts[j].x + sn*pts[j].y) < x_i + 5);
      //Point2f v12 = pts[i + 1] - pts[i];
      //Point2f v13 = pts[i + 2] - pts[i];
      //output += (cv::norm<double>(v12)*cv::norm<double>(v12))*(1 - (cos2v(v12, vect_princ)*cos2v(v12, vect_princ)));
      output += min_val;
      //file << min_val << " || ";
      //file << (cv::norm<double>(v12)*cv::norm<double>(v12))*(1 - (cos2v(v12, vect_princ)*cos2v(v12, vect_princ))) << " || ";
      //2�me possibilit� => sans le carr�
      //output += (cv::norm<double>(v12))*(std::sin(std::acos(cos2v(v12, vect_princ))));

      //3 autre critere : chemin parcourus moyen d'un point au suivant.
      //output += cv::norm<double>(v12);

      //output += (cv::norm<double>(v13)*cv::norm<double>(v13))*(1 - (cos2v(v13, vect_princ)*cos2v(v13, vect_princ)));
    }
    //file << "\n" << endl;
    return output / (pts.size() - 1);
  }

  vector<D_Point> getGraphRepresentation(int nb_pt_max)
  {
    vector<D_Point> out;

    //preliminary conditions
    if (d_pts.size() <= nb_pt_max)
    {
      for (D_Point* dpt : d_pts)
        out.push_back(*dpt);

      //changer direction du dernier et du premier


      representation = out;

      return out;
    }

    //else
    vector<pair<Point2f, double>> new_points;

    double norm_u = std::sqrt(vect_princ.x*vect_princ.x + vect_princ.y*vect_princ.y);
    double cs = -vect_princ.x / norm_u;
    double sn = vect_princ.y / norm_u;

    double x_min = INT_MAX;
    double x_max = -x_min;

    for (D_Point* dpt : d_pts)
    {
      Point2f transform;
      transform.x = cs*dpt->pt.x + sn*dpt->pt.y;
      transform.y = cs*dpt->pt.y - sn*dpt->pt.x;

      if (transform.x < x_min)	x_min = transform.x;
      if (transform.x > x_max)	x_max = transform.x;

      new_points.push_back(make_pair(transform, dpt->angle ));
    }

    //calcul du step
    double step = (x_max - x_min) / nb_pt_max;

    std::sort(new_points.begin(), new_points.end(), [](pair<Point2f, double>& a, pair<Point2f, double>& b) { return a.first.x < b.first.x; });

    //calcul du descripteur de points
    vector<Point2f> sorted_v;

    for (auto i = 0; i < new_points.size(); i++)
    {
      sorted_v.push_back(Point2f(cs*new_points[i].first.x - sn*new_points[i].first.y,
        cs*new_points[i].first.y + sn*new_points[i].first.x));
    }

    m_lineCriteria = lineCriteria(sorted_v);

    vector<pair<Point2f, double>>::iterator it = new_points.begin();

    int nb_pts_extrema = 10;

    //FIRST extremum
    Point temp = Point(0, 0);
    vector<double> temp_or;

    int n = 0;
    for (; n < nb_pts_extrema; ++n)	//parametrable ?
    {
      temp.x += cs*it->first.x - sn*it->first.y;
      temp.y += cs*it->first.y + sn*it->first.x;
      temp_or.push_back(it->second);
      ++it;
    }
    D_Point toadd_first;
    toadd_first.id_graph = this->ID;
    toadd_first.pt = Point(temp.x / n, temp.y / n);
    //toadd_first.pt = Point(cs*new_points.begin()->first.x - sn*new_points.begin()->first.y,
    //	cs*new_points.begin()->first.y + sn*new_points.begin()->first.x);
    //toadd_first.or = temp_or / n;
    toadd_first.angle = meanAngles(temp_or).first;
    out.push_back(toadd_first);

    //parcours de la liste tri�e
    unsigned int nb_pts_zone = n;
    int i = 1;
    //temp = Point(0, 0);
    temp_or.clear();
    for (; it != new_points.end(); ++it)
    {
      if (it->first.x >= i*step + x_min)
      {
        D_Point toadd;
        toadd.id_graph = this->ID;
        toadd.pt = Point(temp.x / nb_pts_zone, temp.y / nb_pts_zone);
        toadd.angle = meanAngles(temp_or).first;
        out.push_back(toadd);
        nb_pts_zone = 0;
        temp.x = 0;
        temp.y = 0;
        temp_or.clear();
        ++i;
      }

      nb_pts_zone++;
      temp.x += cs*it->first.x - sn*it->first.y;
      temp.y += cs*it->first.y + sn*it->first.x;
      temp_or.push_back(it->second);
    }

    //SECOND extremum
    temp = Point(0, 0);
    temp_or.clear();
    n = 0;
    for (; n < nb_pts_extrema; ++n)	//parametrable ?
    {
      it--;
      temp.x += cs*it->first.x - sn*it->first.y;
      temp.y += cs*it->first.y + sn*it->first.x;
      temp_or.push_back(it->second);
    }
    D_Point toadd_sec;
    toadd_sec.id_graph = this->ID;
    toadd_sec.pt = Point(temp.x / n, temp.y / n);
    //toadd_sec.pt = Point(cs*(new_points.end()-1)->first.x - sn*(new_points.end() - 1)->first.y,
    //	cs*(new_points.end()-1)->first.y + sn*(new_points.end() - 1)->first.x);
    //toadd_sec.angle = temp_or / n;
    toadd_sec.angle = meanAngles(temp_or).first;
    out.push_back(toadd_sec);

    //transformation si necessaire du premier angle et du dernier
    //conditions avec produit scalaire
    //envisager une m�moisation plutot que calculer les coordonn�es d'un vecteur dans le repere OpenCV
    //calcul du vecteur de 1to2
    int ind_last = out.size() - 1;
    Point vect_12 = out[ind_last].pt - out[0].pt;
    Point2f vect_pt1 = Point2f(std::cos(out[0].angle *3.14159 / 180.0), std::sin(out[0].angle *3.14159 / 180.0));
    Point2f vect_pt2 = Point2f(std::cos(out[ind_last].angle *3.14159 / 180.0), std::sin(out[ind_last].angle *3.14159 / 180.0));

    //v�rification des produits scalaires
    if (vect_pt1.x*vect_12.x + vect_pt1.y*vect_12.y > 0)
      out[0].angle += 180;
    if (vect_pt2.x*vect_12.x + vect_pt2.y*vect_12.y < 0)
      out[ind_last].angle += 180;

    representation = out;

    return out;
  }


  Point barycentre()
  {
    //inutile pour l'instant
    /*if (m_barycentre != Point(-1, -1))
    return m_barycentre;*/

    Point output = Point(0, 0);
    for (D_Point* pt : d_pts)
    {
      output += pt->pt;
    }

    return Point(output.x / d_pts.size(), output.y / d_pts.size());
  }

  double relationEigenVal()
  {
    Point center = barycentre();
    Mat covMatrix = Mat(2, 2, CV_64FC1);
    Mat eigenValues;
    Mat eigenVectors;

    //on rend les valeurs centr�es sur le barycentre
    vector<Point> data;
    for (D_Point* pt : d_pts)
    {
      data.push_back(pt->pt - center);
    }

    //calcul des coefs de la matrice de covariance
    double sumx = 0, sumy = 0, sumxy = 0;
    for (Point pt : data)
    {
      sumx += pt.x*pt.x;
      sumy += pt.y*pt.y;
      sumxy += pt.x*pt.y;
    }

    covMatrix.at<double>(0, 0) = sumx;
    covMatrix.at<double>(0, 1) = sumxy;
    covMatrix.at<double>(1, 0) = sumxy;
    covMatrix.at<double>(1, 1) = sumy;


    r_square = (double)(sumxy*sumxy) / (double)(sumx*sumy);

    cv::eigen(covMatrix, eigenValues, eigenVectors);

    if (eigenValues.at<double>(0) > eigenValues.at<double>(1))
      vect_princ = Point2d(eigenVectors.at<double>(0, 0), eigenVectors.at<double>(0, 1)) * 10;
    else
      vect_princ = Point2d(eigenVectors.at<double>(1, 0), eigenVectors.at<double>(1, 1)) * 10;


    m_relationEigenVal = abs(eigenValues.at<double>(0) - eigenValues.at<double>(1)) /
      (eigenValues.at<double>(0) + eigenValues.at<double>(1));

    return m_relationEigenVal;
  }

  void newID(int nID)
  {
    for (D_Point* pt : d_pts)
      pt->id_graph = nID;
  }


  void splice(Graph_DPt& graph, Point extrem2erase, D_Point extrem2add)
  {
    //graph.newID(ID);
    d_pts.splice(d_pts.end(), graph.d_pts);
    bounding |= graph.bounding;

    //MAJ des extr�mit�s
    if (extrem2erase == bounds.first.pt)
      bounds.first = extrem2add;
    else
      bounds.second = extrem2add;

    //barycentre();
    //relationEigenVal();
  }

  void computeBounds2(int _MIN_SIZE)
  {
    int min_x = INT16_MAX;
    int min_y = INT16_MAX;
    int max_x = 0;
    int max_y = 0;

    for (D_Point* pt : d_pts)
    {
      if (pt->pt.x < min_x) min_x = pt->pt.x;
      if (pt->pt.x > max_x) max_x = pt->pt.x;
      if (pt->pt.y < min_y) min_y = pt->pt.y;
      if (pt->pt.y > max_y) max_y = pt->pt.y;
    }

    bounding = Rect(Point(min_x - 1, min_y - 1), Point(max_x + 1, max_y + 1));

    vector<D_Point> representation = getGraphRepresentation(_MIN_SIZE);


    bounds = make_pair(representation[0], representation[representation.size() - 1]);

    //NOT the fastest way
    length = cv::norm(bounds.first.pt - bounds.second.pt);
  }


  static int compteur;
  int ID;
  double r_square;
  double nb_friends_mean;
  double m_lineCriteria;
  double m_relationEigenVal;
  double length;
  int *m_histo_orientations;
  list<D_Point*> d_pts;
  vector<D_Point> representation;
  pair<D_Point, D_Point> bounds;
  Point m_barycentre;
  Point2d vect_princ;
  Rect bounding;
};

#endif // !FLASHDETECTION_FLASH_GRAPH_STRUCTURE_H_


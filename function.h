#ifndef _FUNCTION_h

#define _FUNCTION_h

#include "struct.h"

void data_menu();
void func_menu();
void shortcut();
void cal_valid_dist(Node* node);
double cal_its(Node node, double r);
Node* readFile_jz(FILE* fp, Node* ptr);
Terminal* readFile_yd(FILE* fp, Terminal* ptr);
Fake* readFile_wz(FILE* fp, Fake* ptr);
void initTree(QuadTree** root, region x, direction dir);
void initRegion(region* r);
void createTree(QuadTree *root, Node *arr, int n);
void searchBoundary(QuadTree* root, direction dir);
QuadTree* query_leaf(QuadTree* root, double x, double y);
void query_rec(QuadTree* root, double x1, double y1, double x2, double y2, Node* res, int* res_cnt);
double cal_intensity(QuadTree* root, double x, double y, int* ans);
void query_intensity(QuadTree* root, double x, double y);
Node search_id_jz(int id);
Fake search_id_wz(int id);
void query_region(QuadTree* root, int n, int m);
Terminal cal_position(Terminal t, double dist);
Fake cal_position_wz(Fake w, double dist);
double cal_time(Terminal t1, Terminal t2);
void bisection(double* xs, double* ys, double* xe, double* ye, int id);
bool query_in(Terminal t, Node node);
void connect_wz(Terminal yd_start, Fake wz_start);
void destroyTree(QuadTree* root);

#endif // _FUNCTION_h

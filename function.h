#ifndef _FUNCTION_h

#define _FUNCTION_h

#include "struct.h"

void data_menu();
void func_menu();
void cal_valid_dist(Node* node);
double cal_its(Node node, double r);
Node* readFile_jz(FILE* fp, Node* ptr);
Terminal* readFile_yd(FILE* fp, Terminal* ptr);
Fake* readFile_wz(FILE* fp, Fake* ptr);
void initTree(QuadTree** root, region x, direction dir);
void initRegion(region* r);
void insert(QuadTree* root, Node node);
void searchBoundary(QuadTree* root, direction dir);
QuadTree* query_leaf(QuadTree* root, double x, double y);
void query_distance(QuadTree* root, double x, double y, double r);
void query_intensity(QuadTree* root, double x, double y);
void query_region(QuadTree* root, double x, double y);
void destroyTree(QuadTree* root);

#endif // _FUNCTION_h

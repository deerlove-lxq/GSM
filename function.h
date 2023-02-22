#ifndef _FUNCTION_h

#define _FUNCTION_h

#include "struct.h"

void data_menu();
void func_menu();
void cal_valid_dist(Node* node);
double cal_its(Node node, double r);
Node* readFile(FILE* fp, Node* ptr);
void initTree(QuadTree** root, region x, direction dir);
void initRegion(region* r);
void insert(QuadTree* root, Node node);
void searchBoundary(QuadTree* root, direction dir);
void query_distance(QuadTree* root, double x, double y, double r);
void query_intensity(QuadTree* root, double x, double y);
void destroyTree(QuadTree* root);

#endif // _FUNCTION_h

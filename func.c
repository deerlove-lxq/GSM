//消去禁止使用scanf的限制
#define _CRT_SECURE_NO_WARNINGS  1
#pragma warning(disable:6031)

#include <stdio.h>
#include <stdlib.h>
#include "function.h"
#include <string.h>
#include <math.h>

//计算基站信号有效范围
void cal_valid_dist(Node* node) {
	double ans = sqrt(node->its);
	if (strcmp(node->loc, "城区") == 0) ans *= 300;
	else if (strcmp(node->loc, "乡镇") == 0) ans *= 1000;
	else if (strcmp(node->loc, "高速") == 0) ans *= 5000;
	node->valid_dist = ans;
}
//计算基站到给定距离点的信号强度
double cal_its(Node node, double r) {
	//基站对距离为r处的点的强度
	return node.its * (node.valid_dist / r) * (node.valid_dist / r);
}
//读取文件并将基站数据导入数组，并返回导入的最后一个位置的下一个位置
Node* readFile(FILE* fp, Node* ptr) {
	char buf[10];
	memset(buf, '\0', sizeof buf);
	if (fp == NULL) {
		printf("文件打开失败！\n\n");
		exit(1);
	}
	if (fscanf(fp, "%s", buf) != NULL) {
		if (strcmp(buf, "JZ") != 0) printf("误读其他格式的数据文件！\n\n");
		else {
			//注意清除行末尾的换行符
			fscanf(fp, "\n");

			int a = 0, b = 0, c = 0;
			double t = 0;
			while (fscanf(fp, "%d,%d", &a, &b) == 2 && a != -1 && b != -1) {
				memset(buf, '\0', sizeof buf);
				fscanf(fp, ",%s", buf);
				fscanf(fp, "%lf,%d\n", &t, &c);
				strcpy(ptr->loc, buf);
				ptr->x = a, ptr->y = b, ptr->ID = c, ptr->its = t;
				cal_valid_dist(ptr);	//计算出该基站的有效距离
				ptr++;
			}
			printf("数据读取完毕!\n\n");
		}
	}
	return ptr;
}
//初始化树根坐标范围
void initRegion(region* r) {
	r->bottom = r->left = _CRT_INT_MAX;
	r->top = r->right = -_CRT_INT_MAX;
}
//初始化四叉树
void initTree(QuadTree** root, region x, direction dir) {
	//为新的分裂树根分配空间，并传递区域信息。
	*root = (QuadTree*)malloc(sizeof(QuadTree));
	if (*root == NULL) 
	{
		printf("内存不足，无法分配空间！\n\n");
		exit(1);
	}
	else 
	{
		(*root)->father = (*root)->ne = (*root)->nw = (*root)->se = (*root)->sw = NULL;
		(*root)->is_leafNode = true;
		(*root)->nodesNum = 0;
		switch (dir)
		{
		case Origin :
			(*root)->boundary = x;
			break;
		case NW :
			(*root)->boundary.top = x.top;
			(*root)->boundary.left = x.left;
			(*root)->boundary.bottom = (x.top + x.bottom) / 2;
			(*root)->boundary.right = (x.left + x.right) / 2;
			break;
		case NE :
			(*root)->boundary.top = x.top;
			(*root)->boundary.right = x.right;
			(*root)->boundary.left = (x.right + x.left) / 2;
			(*root)->boundary.bottom = (x.top + x.bottom) / 2;
			break;
		case SW:
			(*root)->boundary.bottom = x.bottom;
			(*root)->boundary.left = x.left;
			(*root)->boundary.right = (x.right + x.left) / 2;
			(*root)->boundary.top = (x.top + x.bottom) / 2;
			break;
		case SE:
			(*root)->boundary.bottom = x.bottom;
			(*root)->boundary.right = x.right;
			(*root)->boundary.left = (x.right + x.left) / 2;
			(*root)->boundary.top = (x.top + x.bottom) / 2;
			break;
		}
	}
}
//四叉树插入函数
void insert(QuadTree* root, Node node) {
	if (root == NULL) {
		printf("四叉树尚未进行初始化建立！\n\n");
		exit(1);
	}
	while (!root->is_leafNode) {
		//该点不是叶子
		int root_x = (root->boundary.left + root->boundary.right) / 2;
		int root_y = (root->boundary.bottom + root->boundary.top) / 2;
		//不可能正好与基站重合
		if (node.x >= root_x && node.y >= root_y) root = root->ne;
		else if (node.x > root_x && node.y < root_y) root = root->se;
		else if (node.x < root_x && node.y > root_y) root = root->nw;
		else if (node.x <= root_x && node.y <= root_y) root = root->sw;
	}

	//找到了该坐标所属的叶子节点
	if (root->nodesNum < MAXJZ) {
		//该点是叶子且未满，直接插入。注意nodesNum为个数-1
		root->nodes[root->nodesNum++] = node;
	}
	else {
		//该点是叶子但是容量满了,则分裂点并且重新分配基站
		root->is_leafNode = false;
		region r = root->boundary;
		int root_x = (root->boundary.left + root->boundary.right) / 2;
		int root_y = (root->boundary.bottom + root->boundary.top) / 2;
		initTree(&(root->ne), r, NE);
		initTree(&(root->se), r, SE);
		initTree(&(root->nw), r, NW);
		initTree(&(root->sw), r, SW);
		QuadTree* pne = root->ne, * pse = root->se, * pnw = root->nw, * psw = root->sw;
		for (int i = 0; i < MAXJZ; i++) {
			Node node = root->nodes[i];
			if (node.x >= root_x && node.y >= root_y) insert(pne, node);
			else if (node.x > root_x && node.y < root_y) insert(pse, node);
			else if (node.x < root_x && node.y > root_y) insert(pnw, node);
			else if (node.x <= root_x && node.y <= root_y) insert(psw, node);
		}
		pne->father = pnw->father = pse->father = psw->father = root;
	}
}
//输出边界的基站ID
void searchBoundary(QuadTree* root, direction dir) {
	if (root == NULL) {
		printf("四叉树尚未进行初始化建立！\n\n");
		exit(1);
	}
	switch (dir)
	{
	case NW:
		printf("容量为%d时，最西北侧的基站有：", MAXJZ);
		while (root->nw != NULL) root = root->nw;
		for (int i = 0; i < root->nodesNum; i++) {
			printf("%d ", root->nodes[i].ID);
		}
		printf("\n\n");
		break;
	case NE:
		printf("容量为%d时，最东北侧的基站有：", MAXJZ);
		while (root->ne != NULL) root = root->ne;
		for (int i = 0; i < root->nodesNum; i++) {
			printf("%d ", root->nodes[i].ID);
		}
		printf("\n\n");
		break;
	case SW:
		printf("容量为%d时，最西南侧的基站有：", MAXJZ);
		while (root->sw != NULL) root = root->sw;
		for (int i = 0; i < root->nodesNum; i++) {
			printf("%d ", root->nodes[i].ID);
		}
		printf("\n\n");
		break;
	case SE:
		printf("容量为%d时，最东南侧的基站有：", MAXJZ);
		while (root->se != NULL) root = root->se;
		for (int i = 0; i < root->nodesNum; i++) {
			printf("%d ", root->nodes[i].ID);
		}
		printf("\n\n");
		break;
	}
}
//仅考虑距离，查找指定位置附近的所有基站
void query_distance(QuadTree* root, double x, double y, double r) {
	if (root == NULL) {
		printf("四叉树尚未进行初始化建立！\n\n");
		exit(1);
	}
	while (!root->is_leafNode) {
		//该点不是叶子
		int root_x = (root->boundary.left + root->boundary.right) / 2;
		int root_y = (root->boundary.bottom + root->boundary.top) / 2;
		//不可能正好与基站重合
		if (x >= root_x && y >= root_y) root = root->ne;
		else if (x > root_x && y < root_y) root = root->se;
		else if (x < root_x && y > root_y) root = root->nw;
		else if (x <= root_x && y <= root_y) root = root->sw;
	}
	//找到了该坐标所属的叶子节点，遍历其父节点包含的所有基站。
	QuadTree* ft = root->father;
	QuadTree* pne = ft->ne, * pse = ft->se, * pnw = ft->nw, * psw = ft->sw;
	//由于四叉树建立的对称性，只可以近似地取父节点的四个区域内的基站，然后再核查是否符合条件。
	Node set[4 * MAXJZ];
	int num = 0;
	printf("该点附近范围的待筛选基站ID有：");
	for (int i = 0; i < pne->nodesNum; i++) set[num++] = pne->nodes[i];
	for (int i = 0; i < pse->nodesNum; i++) set[num++] = pse->nodes[i];
	for (int i = 0; i < pnw->nodesNum; i++) set[num++] = pnw->nodes[i];
	for (int i = 0; i < psw->nodesNum; i++) set[num++] = psw->nodes[i];
	for (int i = 0; i < num; i++) printf("%d ", set[i].ID);
	puts("");
	printf("筛选后符合条件的基站ID有：");
	bool sign = true;
	for (int i = 0; i < num; i++) {
		int u = set[i].x - x, v = set[i].y - y;
		double tmp = sqrt(u * u + v * v);
		if (tmp < r) {
			printf("%d ", set[i].ID);
			sign = false;
		}
	}
	if (sign) printf("无");
	printf("\n\n");
}
//考虑信号强度，查找相对指定坐标信号最强的基站
void query_intensity(QuadTree* root, double x, double y) {
	//找到位置坐标附近的所有基站
	if (root == NULL) {
		printf("四叉树尚未进行初始化建立！\n\n");
		exit(1);
	}
	while (!root->is_leafNode) {
		//该点不是叶子
		int root_x = (root->boundary.left + root->boundary.right) / 2;
		int root_y = (root->boundary.bottom + root->boundary.top) / 2;
		//不可能正好与基站重合
		if (x >= root_x && y >= root_y) root = root->ne;
		else if (x > root_x && y < root_y) root = root->se;
		else if (x < root_x && y > root_y) root = root->nw;
		else if (x <= root_x && y <= root_y) root = root->sw;
	}
	//找到了该坐标所属的叶子节点，遍历其父节点包含的所有基站。
	QuadTree* ft = root->father;
	QuadTree* pne = ft->ne, * pse = ft->se, * pnw = ft->nw, * psw = ft->sw;
	//由于四叉树建立的对称性，只可以近似地取父节点的四个区域内的基站，然后再核查是否符合条件。
	Node set[4 * MAXJZ];
	int num = 0, id = -1;
	double max_intensity = 0;
	for (int i = 0; i < pne->nodesNum; i++) set[num++] = pne->nodes[i];
	for (int i = 0; i < pse->nodesNum; i++) set[num++] = pse->nodes[i];
	for (int i = 0; i < pnw->nodesNum; i++) set[num++] = pnw->nodes[i];
	for (int i = 0; i < psw->nodesNum; i++) set[num++] = psw->nodes[i];
	printf("根据您输入的x，y坐标，额定范围内的基站有：");
	for (int i = 0; i < num; i++) {
		int u = set[i].x - x, v = set[i].y - y;
		double tmp_dis = sqrt(u * u + v * v);
		if (tmp_dis < set[i].valid_dist) {
			//在有效范围内
			printf("%d ", set[i].ID);
			//计算符合条件的基站强度，选最大
			double tmp_its = cal_its(set[i], tmp_dis);
			if (max_intensity < tmp_its) {
				max_intensity = tmp_its;
				id = set[i].ID;
			}
		}
	}
	if (id == -1) printf("无\n\n");
	else {
		puts("");
		printf("其中强度最强的基站是：%d\n\n", id);
	}
}
//回收内存
void destroyTree(QuadTree* root) {
	//将指针分配的空间回收
	if (root == NULL) return;
	//既不是叶子，也非空
	destroyTree(root->ne);
	destroyTree(root->se);
	destroyTree(root->nw);
	destroyTree(root->sw);
	free(root);
}
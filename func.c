//消去禁止使用scanf的限制
#define _CRT_SECURE_NO_WARNINGS  1
#pragma warning(disable:6031)

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "function.h"

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
Node* readFile_jz(FILE* fp, Node* ptr) {
	char buf[10];
	memset(buf, '\0', sizeof buf);
	if (fp == NULL) {
		printf("文件打开失败！\n\n");
		return ptr;
	}
	if (fscanf(fp, "%s", buf) != NULL) {
		if (strcmp(buf, "JZ") != 0) {
			printf("误读其他格式的数据文件！\n\n");
			return ptr;
		}
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
			printf("数据读取完毕!");
		}
	}
	return ptr;
}
//读取移动终端信息
Terminal* readFile_yd(FILE* fp, Terminal* ptr) {
	char buf[10];
	memset(buf, '\0', sizeof buf);
	if (fp == NULL) {
		printf("文件打开失败！\n\n");
		return ptr;
	}
	if (fscanf(fp, "%s", buf) != NULL) {
		if (strcmp(buf, "YD") != 0) {
			printf("误读其他格式的数据文件！\n\n");
			return ptr;
		}
		else {
			//注意清除行末尾的换行符
			fscanf(fp, "\n");
			int a = 0, b = 0, c = 0, d = 0, v = 0, h = 0, min = 0;
			while (fscanf(fp, "%d,%d", &a, &b) == 2 && a != -1 && b != -1) {
				fscanf(fp, ",%d,%d,%d,%d,%d\n", &c, &d, &v, &h, &min);
				ptr->xs = a, ptr->ys = b, ptr->xe = c, ptr->ye = d, ptr->speed = v;
				ptr->hour = h, ptr->minute = min;
				ptr++;
			}
			printf("数据读取完毕!");
		}
	}
	return ptr;
}
//读取伪基站信息
Fake* readFile_wz(FILE* fp, Fake* ptr) {
	char buf[10];
	memset(buf, '\0', sizeof buf);
	if (fp == NULL) {
		printf("文件打开失败！\n\n");
		return ptr;
	}
	if (fscanf(fp, "%s", buf) != NULL) {
		if (strcmp(buf, "WZ") != 0) {
			printf("误读其他格式的数据文件！\n\n");
			return ptr;
		}
		else {
			//注意清除行末尾的换行符
			fscanf(fp, "\n");
			int a = 0, b = 0, c = 0, d = 0, v = 0, h = 0, min = 0, id = 0;
			while (fscanf(fp, "%d,%d", &a, &b) == 2 && a != -1 && b != -1) {
				fscanf(fp, ",%d,%d,%d,%d,%d,%d\n", &c, &d, &v, &h, &min, &id);
				ptr->xs = a, ptr->ys = b, ptr->xe = c, ptr->ye = d, ptr->speed = v;
				ptr->hour = h, ptr->minute = min, ptr->ID = id;
				ptr++;
			}
			printf("数据读取完毕!");
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
		return;
	}
	else 
	{
		(*root)->ne = (*root)->nw = (*root)->se = (*root)->sw = NULL;
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
		return;
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
	}
}
//输出边界的基站ID
void searchBoundary(QuadTree* root, direction dir) {
	if (root == NULL) {
		printf("四叉树尚未进行初始化建立！\n\n");
		return;
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
//查找指定点所归属的叶子
QuadTree* query_leaf(QuadTree* root, double x, double y) {
	if (root == NULL) {
		printf("四叉树尚未进行初始化建立！\n\n");
		return NULL;
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
	return root;
}
//仅考虑距离，查找指定位置附近的所有基站
void query_distance(QuadTree* root, double x, double y, double r) {
	QuadTree* goal = query_leaf(root, x, y);
	if (goal == NULL) return;

	//找到了该坐标所属的叶子，推算出以该叶子为中心的等大小九宫格（利用region信息）
	region rg = goal->boundary;
	double x2 = rg.right, x1 = rg.left, y2 = rg.top, y1 = rg.bottom;
	double u2 = (3 * x2 - x1) / 2, u1 = (3 * x1 - x2) / 2;
	double v2 = (3 * y2 - y1) / 2, v1 = (3 * y1 - y2) / 2;
	
	QuadTree* q[9];					//存储九宫格各中心所属的叶子
	Node set_st[100], set_sec[100];	//分别存储第一次筛选后符合的基站和第二次筛选的基站
	int n = 0, m = 0;
	bool sign = true;

	//搜索九宫格各中心所属叶子，并存入q数组。0-9从左到右，从上到下。
	q[0] = query_leaf(root, u1, v2);
	q[1] = query_leaf(root, x, v2);
	q[2] = query_leaf(root, u2, v2);
	q[3] = query_leaf(root, u1, y);
	q[4] = goal;
	q[5] = query_leaf(root, u2, y);
	q[6] = query_leaf(root, u1, v1);
	q[7] = query_leaf(root, x, v1);
	q[8] = query_leaf(root, u2, v1);

	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < q[i]->nodesNum; j++) {
			//简单的去重代码,由于体量较小，所以直接顺序查找。
			bool st = true;
			for (int k = 0; k < n; k++) {
				if (set_st[k].ID == q[i]->nodes[j].ID) {
					st = false;
					break;
				}
			}
			if (st) set_st[n++] = q[i]->nodes[j];	//将九宫格内所有可能的基站存入set数组
			sign = false;
		}
	}
	if (sign) {
		printf("该点邻近范围内无基站\n\n");
		return;
	}
	sign = true;

	printf("以(%.2lf, %.2lf)为中心的九宫格范围内待筛选基站ID有：", x, y);
	for (int i = 0; i < n; i++) {
		printf("%d ", set_st[i].ID);
		double u = set_st[i].x - x, v = set_st[i].y - y;
		double tmp = sqrt(u * u + v * v);
		if (tmp < r) {
			set_sec[m++] = set_st[i];
			sign = false;
		}
	}
	puts("");
	if (sign) {
		printf("以(%.2lf, %.2lf)为圆心，半径为%.2lf的范围内无符合半径条件的基站\n\n", x, y, r);
		return;
	}
	printf("筛选后：以(%.2lf, %.2lf)为圆心，半径为%.2lf的范围内的基站ID有：", x, y, r);
	for (int i = 0; i < m; i++) printf("%d ", set_sec[i].ID);
	puts("\n");
}
//考虑信号强度，查找相对指定坐标信号最强的基站
void query_intensity(QuadTree* root, double x, double y) {
	//找到位置坐标附近的所有基站
	QuadTree* goal = query_leaf(root, x, y);
	if (goal == NULL) return;

	//找到了该坐标所属的叶子，推算出以该叶子为中心的等大小九宫格（利用region信息）
	region rg = goal->boundary;
	double x2 = rg.right, x1 = rg.left, y2 = rg.top, y1 = rg.bottom;
	double u2 = (3 * x2 - x1) / 2, u1 = (3 * x1 - x2) / 2;
	double v2 = (3 * y2 - y1) / 2, v1 = (3 * y1 - y2) / 2;

	QuadTree* q[9];					//存储九宫格各中心所属的叶子
	double max_intensity = 0.01;	//如果小于相对强度小于0.01，视为无信号。
	int id = -1;
	bool sign = true;

	//搜索九宫格各中心所属叶子，并存入q数组。0-9从左到右，从上到下。
	q[0] = query_leaf(root, u1, v2);
	q[1] = query_leaf(root, x, v2);
	q[2] = query_leaf(root, u2, v2);
	q[3] = query_leaf(root, u1, y);
	q[4] = goal;
	q[5] = query_leaf(root, u2, y);
	q[6] = query_leaf(root, u1, v1);
	q[7] = query_leaf(root, x, v1);
	q[8] = query_leaf(root, u2, v1);

	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < q[i]->nodesNum; j++) {
			//此处如果希望避免重复运算，可以用空间换时间，使用set数组存储。综合考虑选择了直接运算比较。
			double u = q[i]->nodes[j].x - x, v = q[i]->nodes[j].y - y;
			double r = sqrt(u * u + v * v);
			double tmp = cal_its(q[i]->nodes[j], r);
			if (max_intensity < tmp) {
				max_intensity = tmp;
				id = q[i]->nodes[j].ID;
				sign = false;
			}
		}
	}

	if (sign) {
		printf("(%.2lf, %.2lf)处在无信号覆盖的区域\n\n", x, y);
		return;
	}

	printf("在(%.2lf, %.2lf)处 \t 信号最好的基站ID是：%d \t 信号强度为：%.2lf\n\n", x, y, id, max_intensity);
}
//查询指定点所处的叶子的矩形区域大小
void query_region(QuadTree* root, int n, int m) {
	//用于测试容量是否合理，防止长宽太小导致九宫格范围不足。
	if (n <= 0 || m <= 0) {
		printf("您的输入有误，请输入正整数。\n\n");
		return;
	}
	srand(time(NULL));
	int sum_x = 0, sum_y = 0, sum_jz = 0, count = 0;
	for (int i = 0; i < n; i++) {
		//x的范围为：left ~ right		y的范围为：bottom ~ top	(因最左和最下都小于0，取绝对值后可得到此公式)
		int x = rand() % (root->boundary.right - root->boundary.left + 1) + root->boundary.left;
		int y = rand() % (root->boundary.top - root->boundary.bottom + 1) + root->boundary.bottom;
		QuadTree* goal = query_leaf(root, x, y);
		if (goal == NULL) continue;
		region r = goal->boundary;
		sum_x += r.right - r.left, sum_y += r.top - r.bottom;
		sum_jz += (goal->nodesNum);
		count++;
	}
	int ave_x = sum_x / count, ave_y = sum_y / count;
	double ave_jz = 100.0 * sum_jz / (count * MAXJZ);
	printf("叶子容量为%d的条件下, 随机在区域内取%d个坐标。得到叶子的基站数量和控制区域的平均大小。\n", MAXJZ, n);
	printf("每个叶子平均的基站占有率：%.2lf%%, 区域纵向宽度：%d, 区域水平宽度: %d\n", ave_jz, ave_y, ave_x);
	printf("以该叶子区域为中心的九宫格的纵向宽度：%d, 水平宽度：%d\n", 3 * ave_y, 3 * ave_x);

	
	//计算九宫格内的平均基站数量

	int sum_nine = 0, count_nine = 0;

	for (int i = 0; i < m; i++) {
		int x = rand() % (root->boundary.right - root->boundary.left + 1) + root->boundary.left;
		int y = rand() % (root->boundary.top - root->boundary.bottom + 1) + root->boundary.bottom;
		QuadTree* goal = query_leaf(root, x, y);
		if (goal == NULL) continue;
		//找到了该坐标所属的叶子，推算出以该叶子为中心的等大小九宫格（利用region信息）
		region rg = goal->boundary;
		double x2 = rg.right, x1 = rg.left, y2 = rg.top, y1 = rg.bottom;
		double u2 = (3 * x2 - x1) / 2, u1 = (3 * x1 - x2) / 2;
		double v2 = (3 * y2 - y1) / 2, v1 = (3 * y1 - y2) / 2;

		QuadTree* q[9];					//存储九宫格各中心所属的叶子
		Node set_st[100], set_sec[100];	//分别存储第一次筛选后符合的基站和第二次筛选的基站
		int t = 0;

		//搜索九宫格各中心所属叶子，并存入q数组。0-9从左到右，从上到下。
		q[0] = query_leaf(root, u1, v2);
		q[1] = query_leaf(root, x, v2);
		q[2] = query_leaf(root, u2, v2);
		q[3] = query_leaf(root, u1, y);
		q[4] = goal;
		q[5] = query_leaf(root, u2, y);
		q[6] = query_leaf(root, u1, v1);
		q[7] = query_leaf(root, x, v1);
		q[8] = query_leaf(root, u2, v1);

		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < q[i]->nodesNum; j++) {
				//简单的去重代码,由于体量较小，所以直接顺序查找。
				bool st = true;
				for (int k = 0; k < t; k++) {
					if (set_st[k].ID == q[i]->nodes[j].ID) {
						st = false;
						break;
					}
				}
				if (st) set_st[t++] = q[i]->nodes[j];	//将九宫格内所有可能的基站存入set数组
			}
		}
		sum_nine += t;
		count_nine++;
	}
	printf("九宫格内部的平均基站数量为：%d\n\n", sum_nine / count_nine);
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



//代码量：470行
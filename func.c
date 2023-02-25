//消去禁止使用scanf的限制
#define _CRT_SECURE_NO_WARNINGS  1
#pragma warning(disable:6031)

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "function.h"

extern Node gaosu[20];								//单独存储少量的高速基站信息
extern int cnt_gao;									//高速基站个数

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
			double a = 0, b = 0, t = 0;
			int c = 0;
			while (fscanf(fp, "%lf,%lf", &a, &b) == 2 && a != -1 && b != -1) {
				memset(buf, '\0', sizeof buf);
				fscanf(fp, ",%s", buf);
				fscanf(fp, "%lf,%d\n", &t, &c);
				strcpy(ptr->loc, buf);
				ptr->x = a, ptr->y = b, ptr->ID = c, ptr->its = t;
				cal_valid_dist(ptr);	//计算出该基站的有效距离
				if (strcmp(buf, "高速") == 0) gaosu[cnt_gao++] = *ptr;
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
			double a = 0, b = 0, c = 0, d = 0, v = 0;
			int h = 0, min = 0;
			while (fscanf(fp, "%lf,%lf", &a, &b) == 2 && a != -1 && b != -1) {
				fscanf(fp, ",%lf,%lf,%lf,%d,%d\n", &c, &d, &v, &h, &min);
				ptr->xs = a, ptr->ys = b, ptr->xe = c, ptr->ye = d, ptr->speed = v / 3.6;
				ptr->full_dist = sqrt((a - c) * (a - c) + (b - d) * (b - d));
				ptr->hour = h, ptr->minute = min, ptr->seconds = 0;
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
			double a = 0, b = 0, c = 0, d = 0, v = 0;
			int h = 0, min = 0, id = 0;
			while (fscanf(fp, "%lf,%lf", &a, &b) == 2 && a != -1 && b != -1) {
				fscanf(fp, ",%lf,%lf,%lf,%d,%d,%d\n", &c, &d, &v, &h, &min, &id);
				ptr->xs = a, ptr->ys = b, ptr->xe = c, ptr->ye = d, ptr->speed = v / 3.6;
				ptr->hour = h, ptr->minute = min, ptr->ID = id, ptr->seconds = 0;
				ptr->full_dist = sqrt((a - c) * (a - c) + (b - d) * (b - d));
				ptr->valid_dist = 40;
				ptr++;
			}
			printf("数据读取完毕!");
		}
	}
	return ptr;
}

//初始化树根坐标范围
void initRegion(region* r) {
	r->bottom = r->left = (double)_CRT_INT_MAX;
	r->top = r->right = -(double)_CRT_INT_MAX;
}

//初始化四叉树
void initTree(QuadTree** root, region r, direction dir) {
	//region x是当前的中心的区域
	//为新的分裂树根分配空间，并传递区域信息。
	double root_x = (r.left + r.right) / 2;
	double root_y = (r.bottom + r.top) / 2;
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
			(*root)->boundary = r;
			break;
		case NW :
			(*root)->boundary.top = r.top;
			(*root)->boundary.left = r.left;
			(*root)->boundary.bottom = root_y;
			(*root)->boundary.right = root_x;
			break;
		case NE :
			(*root)->boundary.top = r.top;
			(*root)->boundary.right = r.right;
			(*root)->boundary.left = root_x;
			(*root)->boundary.bottom = root_y;
			break;
		case SW:
			(*root)->boundary.bottom = r.bottom;
			(*root)->boundary.left = r.left;
			(*root)->boundary.right = root_x;
			(*root)->boundary.top = root_y;
			break;
		case SE:
			(*root)->boundary.bottom = r.bottom;
			(*root)->boundary.right = r.right;
			(*root)->boundary.left = root_x;
			(*root)->boundary.top = root_y;
			break;
		}
	}
}

//输出边界的基站ID
void searchBoundary(QuadTree* root, direction dir) {
	if (root == NULL) {
		printf("四叉树尚未进行初始化建立！\n\n");
		return;
	}
	region r;
	switch (dir)
	{
	case NW:
		while (root->nw != NULL) root = root->nw;
		r = root->boundary;
		printf("西北角的叶子区域范围为：(%.2lf, %.2lf, %.2lf, %.2lf)\n", r.left, r.bottom, r.right, r.top);
		printf("容量为%d时，最西北侧的基站有：", MAXJZ);
		for (int i = 0; i < root->nodesNum; i++) {
			printf("%d ", root->nodes[i].ID);
		}
		printf("\n\n");
		break;
	case NE:
		while (root->ne != NULL) root = root->ne;
		r = root->boundary;
		printf("东北角的叶子区域范围为：(%.2lf, %.2lf, %.2lf, %.2lf)\n", r.left, r.bottom, r.right, r.top);
		printf("容量为%d时，最东北侧的基站有：", MAXJZ);
		for (int i = 0; i < root->nodesNum; i++) {
			printf("%d ", root->nodes[i].ID);
		}
		printf("\n\n");
		break;
	case SW:
		while (root->sw != NULL) root = root->sw;
		r = root->boundary;
		printf("西南角的叶子区域范围为：(%.2lf, %.2lf, %.2lf, %.2lf)\n", r.left, r.bottom, r.right, r.top);
		printf("容量为%d时，最西南侧的基站有：", MAXJZ);
		for (int i = 0; i < root->nodesNum; i++) {
			printf("%d ", root->nodes[i].ID);
		}
		printf("\n\n");
		break;
	case SE:
		while (root->se != NULL) root = root->se;
		r = root->boundary;
		printf("东南角的叶子区域范围为：(%.2lf, %.2lf, %.2lf, %.2lf)\n", r.left, r.bottom, r.right, r.top);
		printf("容量为%d时，最东南侧的基站有：", MAXJZ);
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
		double root_x = (root->boundary.left + root->boundary.right) / 2;
		double root_y = (root->boundary.bottom + root->boundary.top) / 2;
		//不可能正好与基站重合，压右上线算在框内
		if (x >= root_x && y >= root_y) root = root->ne;
		else if (x > root_x && y < root_y) root = root->se;
		else if (x < root_x && y > root_y) root = root->nw;
		else if (x <= root_x && y <= root_y) root = root->sw;
	}
	return root;
}

//查找矩形范围内的所有基站(自上而下：利用四叉树的分割平面的几何性质)
void query_rec(QuadTree* root, double x1, double y1, double x2, double y2, Node* res, int* res_cnt) {
	if (root == NULL) {
		printf("四叉树尚未进行初始化建立！\n\n");
		return NULL;
	}
	//该点是叶子
	if (root->is_leafNode) {
		for (int i = 0; i < root->nodesNum; i++) {
			double jz_x = root->nodes[i].x, jz_y = root->nodes[i].y;
			if (jz_x <= x2 && jz_x >= x1 && jz_y >= y1 && jz_y <= y2) {
				res[(*res_cnt)++] = root->nodes[i];
			}
		}
		return;
	}
	//该点不是叶子
	
	QuadTree* p[4] = {root->ne, root->nw, root->se, root->sw};
	for (int i = 0; i < 4; i++) {
		region r = p[i]->boundary;
		if (r.right >= x1 && r.left <= x2 && r.bottom <= y2 && r.top >= y1) {
			query_rec(p[i], x1, y1, x2, y2, res, res_cnt);
		}
	}
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
	double min_r = _CRT_INT_MAX;	//设置距离因素指标min_r
	int id_its = -1, id_r = -1;
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
			double valid_r = q[i]->nodes[j].valid_dist;
			double tmp = cal_its(q[i]->nodes[j], r);
			if (r < min_r) {
				min_r = r;
				id_r = q[i]->nodes[j].ID;
			}
			//必须满足在有效距离内才有信号覆盖
			if (r < valid_r) {
				sign = false;
				if (max_intensity < tmp) {
					max_intensity = tmp;
					id_its = q[i]->nodes[j].ID;
				}
			}
		}
	}

	//单独考虑是否有高速基站的可能
	
	for (int i = 0; i < cnt_gao; i++) {
		Node gs = gaosu[i];
		double u = gs.x - x, v = gs.y - y;
		double r = sqrt(u * u + v * v);
		double valid_r = gs.valid_dist;
		double tmp = cal_its(gs, r);
		
		if (r < min_r) {
			min_r = r;
			id_r = gs.ID;
		}
		//必须满足在有效距离内才有信号覆盖
		if (r < valid_r) {
			sign = false;
			if (max_intensity < tmp) {
				max_intensity = tmp;
				id_its = gs.ID;
			}
		}
	}

	if (sign) {
		printf("(%.2lf, %.2lf)处在无信号覆盖的区域\n", x, y);
		printf("距离该点最近的基站为：%d\n\n", id_r);
		return;
	}

	printf("在(%.2lf, %.2lf)处信号最好的基站ID是：%d\t信号强度为：%.2lf\n", x, y, id_its, max_intensity);
	printf("距离该点最近的基站为：%d\n\n", id_r);
}

//只返回信号强度不返回
double cal_intensity(QuadTree* root, double x, double y, int* ans) {
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
	double min_r = _CRT_INT_MAX;	//设置距离因素指标min_r
	int id_its = -1, id_r = -1;
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
			double valid_r = q[i]->nodes[j].valid_dist;
			double tmp = cal_its(q[i]->nodes[j], r);
			if (r < min_r) {
				min_r = r;
				id_r = q[i]->nodes[j].ID;
			}
			//必须满足在有效距离内才有信号覆盖
			if (r < valid_r) {
				sign = false;
				if (max_intensity < tmp) {
					max_intensity = tmp;
					id_its = q[i]->nodes[j].ID;
				}
			}
		}
	}

	//单独考虑是否有高速基站的可能
	
	for (int i = 0; i < cnt_gao; i++) {
		Node gs = gaosu[i];
		double u = gs.x - x, v = gs.y - y;
		double r = sqrt(u * u + v * v);
		double valid_r = gs.valid_dist;
		double tmp = cal_its(gs, r);
		if (r < min_r) {
			min_r = r;
			id_r = gs.ID;
		}
		//必须满足在有效距离内才有信号覆盖
		if (r < valid_r) {
			sign = false;
			if (max_intensity < tmp) {
				max_intensity = tmp;
				id_its = gs.ID;
			}
		}
	}
	*ans = id_its;
	if (sign) return 0;
	else return max_intensity;
}

//查询指定点所处的叶子的矩形区域大小
void query_region(QuadTree* root, int n, int m) {
	//用于测试容量是否合理，防止长宽太小导致九宫格范围不足。
	if (n <= 0 || m <= 0) {
		printf("您的输入有误，请输入正整数。\n\n");
		return;
	}
	if (root == NULL) {
		printf("四叉树尚未进行初始化建立！\n\n");
		return;
	}
	srand(time(NULL));
	double sum_x = 0, sum_y = 0;
	int sum_jz = 0, count = 0;
	for (int i = 0; i < n; i++) {
		//x的范围为：left ~ right		y的范围为：bottom ~ top	(因最左和最下都小于0，取绝对值后可得到此公式)
		double x = rand() % ((int)(root->boundary.right - root->boundary.left) + 1) + root->boundary.left;
		double y = rand() % ((int)(root->boundary.top - root->boundary.bottom) + 1) + root->boundary.bottom;
		QuadTree* goal = query_leaf(root, x, y);
		if (goal == NULL) continue;
		region r = goal->boundary;
		sum_x += r.right - r.left, sum_y += r.top - r.bottom;
		sum_jz += (goal->nodesNum);
		count++;
	}
	double ave_x = sum_x / count, ave_y = sum_y / count;
	double ave_jz = 100.0 * sum_jz / (count * MAXJZ);
	printf("叶子容量为%d的条件下, 随机在区域内取%d个坐标。得到叶子的基站数量和控制区域的平均大小。\n", MAXJZ, n);
	printf("每个叶子平均的基站占有率：%.2lf%%, 区域纵向宽度：%.2lf, 区域水平宽度: %.2lf\n", ave_jz, ave_y, ave_x);
	printf("以该叶子区域为中心的九宫格的纵向宽度：%.2lf, 水平宽度：%.2lf\n", 3 * ave_y, 3 * ave_x);

	
	//计算九宫格内的平均基站数量

	int sum_nine = 0, count_nine = 0;

	for (int i = 0; i < m; i++) {
		double x = rand() % ((int)(root->boundary.right - root->boundary.left) + 1) + root->boundary.left;
		double y = rand() % ((int)(root->boundary.top - root->boundary.bottom) + 1) + root->boundary.bottom;
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

//返回经过dist距离后，移动终端的位置信息
Terminal cal_position(Terminal t, double dist) {
	Terminal ans;
	//x和y的改变量
	double chx = (t.xe - t.xs) * dist / t.full_dist, chy = (t.ye - t.ys) * dist / t.full_dist;	
	//新的位置ans（同终点）变起点，变full_dist，变时间
	ans.speed = t.speed, ans.xe = t.xe, ans.ye = t.ye;
	ans.full_dist = t.full_dist - dist;
	ans.xs = t.xs + chx, ans.ys = t.ys + chy;
	//换算时间
	double time = dist / t.speed;
	double all_sec = time + t.seconds;
	ans.seconds = all_sec - (int)all_sec + (int)all_sec % 60;
	int all_minute = (int)(all_sec - ans.seconds) / 60 + t.minute;
	ans.minute = all_minute % 60;
	ans.hour = all_minute / 60 + t.hour;
	return ans;
}

//二分法算出边界
void bisection(double *xs, double *ys, double *xe, double *ye, int id) {
	//给定两个坐标，在两坐标构成的线段上进行二分计算边界点
	Node goal = search_id(id);
	double standard = goal.valid_dist;
	double u = xe - xs, v = ye - ys, r = sqrt(u * u + v * v);
	double us = goal.x - *xs, vs = goal.y - *ys, rs = sqrt(us * us + vs * vs);
	double ue = goal.x - *xe, ve = goal.y - *ye, re = sqrt(ue * ue + ve * ve);
	
	if (rs > re) {
		//使得s->e为从小到大，rs在区域内，re在区域外
		double tmp;
		tmp = *xs, *xs = *xe, *xe = tmp;
		tmp = *ys, *ys = *ye, *ye = tmp;
		tmp = rs, rs = re, re = tmp;
	}
	
	while (r / 2 > 0.1) {
		double x_mid = (*xe + *xs) / 2, y_mid = (*ye + *ys) / 2;	//二分
		double u_mid = goal.x - x_mid, v_mid = goal.y - y_mid;
		double r_mid = sqrt(u_mid * u_mid + v_mid * v_mid);
		if (r_mid < standard) *xs = x_mid, *ys = y_mid;
		else if (r_mid > standard) *xe = x_mid, *ye = y_mid;
		else break;
		u = *xe - *xs, v = *ye - *ys, r = sqrt(u * u + v * v);
	}
}

//判断现在的位置是不是在指定基站范围内
bool query_in(Terminal t, Node node) {
	double x = t.xs - node.x, y = t.ys - node.y, r = sqrt(x * x + y * y);
	if (r < node.valid_dist) return true;
	else return false;
}

//输入两个位置，计算之间相隔的时间
double cal_time(Terminal t1, Terminal t2) {
	double ans = (t2.hour * 3600 + t2.minute * 60 + t2.seconds) - (t1.hour * 3600 + t1.minute * 60 + t1.seconds);
	if (ans < 0) return -ans;
	else return ans;
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
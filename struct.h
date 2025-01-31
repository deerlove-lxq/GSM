#ifndef _STRUCT_H

#define _STRUCT_H

//容量
#define MAXJZ 6
#define MaxSize 10000

typedef enum {false, true} bool;
typedef enum {Origin, NW, NE, SW, SE} direction;

//基站的数据结构
typedef struct Node {
	double x, y;		//坐标
	char loc[10];		//乡镇、城区、高速
	double its;			//信号强度
	double valid_dist;	//有效范围
	int ID;				//编号
} Node;

//基站坐标范围的数据结构：确认中点
typedef struct region {
	double top, bottom, left, right;
} region;

//四叉树的数据结构
typedef struct QuadTree {
	struct QuadTree* nw, * ne, * sw, * se;	//四个象限
	Node nodes[MAXJZ];	//存储基站信息
	bool is_leafNode;	//是否为叶子节点：只有叶子节点存储基站信息
	int nodesNum;		//该点附属的所有基站数：用于判断是否需要分裂
	region boundary;	//该点对应区域的边界信息
} QuadTree;

//移动终端的数据结构
typedef struct {
	double xs, ys, xe, ye;	//xs是现在的位置
	double full_dist;
	double speed;	//换算成米/秒
	int hour;
	int minute;
	double seconds;
} Terminal;

//伪基站的数据结构
typedef struct {
	double xs, ys, xe, ye;
	double full_dist, valid_dist, full_time;
	double speed;
	int hour;
	int minute;
	double seconds;
	int ID;
} Fake;

#endif // struct.h


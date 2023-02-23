#define _CRT_SECURE_NO_WARNINGS  1
#pragma warning(disable:6031)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "function.h"

extern Node jz[MaxSize];							//从文件中读取约8000+基站的信息
extern int cnt;										//基站总个数 = cnt + 1
extern bool file_jz1, file_jz2, file_wz, file_yd;	//保证数据只被用户导入一次
extern QuadTree* root;								//四叉树树根

extern Terminal yd[100];	//存储终端移动情况
extern Fake wz[100];		//存储伪基站移动情况
extern int yd_num, wz_num;	//存储终端、伪基站的数据条数（12/5）

//数据导入菜单
void data_menu() {
	int op = -1, num = 6;
	FILE *fp;
	Node *ptr1_jz, *ptr2_jz;	//指向尚未添加的节点
	Fake *ptr1_wz, *ptr2_wz;
	Terminal* ptr1_yd, *ptr2_yd;
	
	region t;
	initRegion(&t);

	char menu[10][50] = { "0-主目录", "1-导入基站1数据", "2-导入基站2数据", "3-导入伪基站数据", "4-导入移动数据", "5-生成四叉树"};
	printf("--------------------------------------------------\n");		//30个*
	printf("|                输入数字选择功能                |\n");
	printf("--------------------------------------------------\n");
	for (int i = 0; i < num; i++) {
		printf("|%-48s|\n", menu[i]);
	}
	printf("--------------------------------------------------\n\n");
	printf("请输入您的选择: ");
	scanf("%d", &op);
	puts("");
	//进入二级菜单
	while (op >= 7 || op < 0) {
		printf("您输入的选项不合法！请重新输入：");
		scanf("%d", &op);
		puts("");
	}
	switch (op) {
	case 0:
		break;
	case 1:
		if (file_jz1) {
			printf("jz001.txt已被成功导入！请勿重复操作\n\n");
			break;
		}
		ptr1_jz = &(jz[cnt]);
		fp = fopen("./data/jz001.txt", "r");
		printf("jz001文件读取中：请稍后……\n");
		ptr2_jz = readFile_jz(fp, ptr1_jz);
		fclose(fp);
		cnt += (ptr2_jz - ptr1_jz);
		printf("共%d条基站数据已录入完成。\n\n", cnt);
		file_jz1 = true;
		break;
	case 2:
		if (file_jz2) {
			printf("jz002.txt已被成功导入！请勿重复操作\n\n");
			break;
		}
		ptr1_jz = &(jz[cnt]);
		fp = fopen("./data/jz002.txt", "r");
		printf("jz002文件读取中：请稍后……\n");
		ptr2_jz = readFile_jz(fp, ptr1_jz);
		fclose(fp);
		cnt += (ptr2_jz - ptr1_jz);
		printf("共%d条基站数据已录入完成。\n\n", cnt);
		file_jz2 = true;
		break;
	case 3:
		if (file_wz) {
			printf("wz001.txt已被成功导入！请勿重复操作\n\n");
			break;
		}
		ptr1_wz = &(wz[wz_num]);
		fp = fopen("./data/wz001.txt", "r");
		printf("wz001文件读取中：请稍后……\n");
		ptr2_wz = readFile_wz(fp, ptr1_wz);
		fclose(fp);
		wz_num += (ptr2_wz - ptr1_wz);
		printf("共%d条伪基站数据已录入完成。\n\n", wz_num);
		file_wz = true;
		break;
	case 4:
		if (file_yd) {
			printf("yd001.txt已被成功导入！请勿重复操作\n\n");
			break;
		}
		ptr1_yd = &(yd[yd_num]);
		fp = fopen("./data/yd001.txt", "r");
		printf("yd001文件读取中：请稍后……\n");
		ptr2_yd = readFile_yd(fp, ptr1_yd);
		fclose(fp);
		yd_num += (ptr2_yd - ptr1_yd);
		printf("共%d条移动终端数据已录入完成。\n\n", yd_num);
		file_yd = true;
		break;
	case 5:
		if (cnt == 0) {
			printf("请先导入数据，再建立四叉树！\n\n");
			break;
		}
		//步骤1：利用基站坐标中的边界条件，创建并初始化根节点
		for (int i = 0; i < cnt; i++) {
			t.bottom = min(t.bottom, jz[i].y);
			t.top = max(t.top, jz[i].y);
			t.left = min(t.left, jz[i].x);
			t.right = max(t.right, jz[i].x);
		}
		initTree(&root, t, Origin);
		//步骤2：将基站数据插入四叉树
		for (int i = 0; i < cnt; i++) {
			insert(root, jz[i]);
		}
		printf("四叉树已经建立完毕！共使用了%d个基站\n\n ", cnt);
		break;
	}
}
//函数功能菜单
void func_menu() {
	int op = -1, num = 7, dir = -1, n = -1, m = -1;
	bool sign = false;
	double x, y, r;
	char str[10];
	char menu[20][100] = { "0-主目录", "1-查找最西(东)北(南)的基站", "2-查找指定范围内的基站", 
		"3-指定点是否处在无信号区域，输出信号最强的基站", "4-指定容量下平均单元块大小和叶子包含的基站数量", 
		"5-输出依次切换的基站和通信连接状态", "6-"};

	printf("--------------------------------------------------\n");		//50个
	printf("|                输入数字选择功能                |\n");
	printf("--------------------------------------------------\n");
	for (int i = 0; i < num; i++) {
		printf("|%-48s|\n", menu[i]);
	}
	printf("--------------------------------------------------\n\n");
	printf("请输入您的选择: ");
	scanf("%d", &op);
	puts("");
	//进入二级菜单
	while (op >= 7 || op < 0) {
		printf("您输入的选项不合法！请重新输入: ");
		scanf("%d", &op);
		puts("");
	}
	switch (op) {
	case 0:
		break;
	case 1:
		printf("请输入您想查询的方向(例：东南)：");
		scanf("%s", str);
		puts("");
		if (strcmp(str, "东南") == 0) dir = SE;
		else if (strcmp(str, "东北") == 0) dir = NE;
		else if (strcmp(str, "西南") == 0) dir = SW;
		else if (strcmp(str, "西北") == 0) dir = NW;
		else {
			printf("您的输入格式错误！\n\n");
			break;
		}
		searchBoundary(root, dir);
		break;
	case 2:
		printf("请输入该范围的圆心(查询点)坐标: ");
		scanf("%lf %lf", &x, &y);
		printf("请输入距离查询点的半径: ");
		scanf("%lf", &r);
		puts("");
		query_distance(root, x, y, r);
		break;
	case 3:
		printf("请输入查询点坐标: ");
		scanf("%lf %lf", &x, &y);
		puts("");
		query_intensity(root, x, y);
		break;
	case 4:
		printf("请问需要设置多少个随机点去测试叶子区域的平均宽度？请输入正整数n：");
		scanf("%d", &n);
		printf("请问需要设置多少个随机点去测试九宫格内部的平均基站数量？(建议小于5000)请输入正整数m：");
		scanf("%d", &m);
		query_region(root, n, m);
		break;
	case 5:
		break;
	case 6:
		break;
	}
}
//一键导入
void shortcut() {
	FILE* fp;
	Node* ptr1_jz, * ptr2_jz;	//指向尚未添加的节点
	Fake* ptr1_wz, * ptr2_wz;
	Terminal* ptr1_yd, * ptr2_yd;

	region t;
	initRegion(&t);

	if (file_jz1) {
		printf("jz001.txt已被成功导入！请勿重复操作\n\n");
		return;
	}
	ptr1_jz = &(jz[cnt]);
	fp = fopen("./data/jz001.txt", "r");
	printf("jz001文件读取中：请稍后……\n");
	ptr2_jz = readFile_jz(fp, ptr1_jz);
	fclose(fp);
	cnt += (ptr2_jz - ptr1_jz);
	printf("共%d条基站数据已录入完成。\n\n", cnt);
	file_jz1 = true;

	if (file_jz2) {
		printf("jz002.txt已被成功导入！请勿重复操作\n\n");
		return;
	}
	ptr1_jz = &(jz[cnt]);
	fp = fopen("./data/jz002.txt", "r");
	printf("jz002文件读取中：请稍后……\n");
	ptr2_jz = readFile_jz(fp, ptr1_jz);
	fclose(fp);
	cnt += (ptr2_jz - ptr1_jz);
	printf("共%d条基站数据已录入完成。\n\n", cnt);
	file_jz2 = true;
		
	if (file_wz) {
		printf("wz001.txt已被成功导入！请勿重复操作\n\n");
		return;
	}
	ptr1_wz = &(wz[wz_num]);
	fp = fopen("./data/wz001.txt", "r");
	printf("wz001文件读取中：请稍后……\n");
	ptr2_wz = readFile_wz(fp, ptr1_wz);
	fclose(fp);
	wz_num += (ptr2_wz - ptr1_wz);
	printf("共%d条伪基站数据已录入完成。\n\n", wz_num);
	file_wz = true;
	
	if (file_yd) {
		printf("yd001.txt已被成功导入！请勿重复操作\n\n");
		return;
	}
	ptr1_yd = &(yd[yd_num]);
	fp = fopen("./data/yd001.txt", "r");
	printf("yd001文件读取中：请稍后……\n");
	ptr2_yd = readFile_yd(fp, ptr1_yd);
	fclose(fp);
	yd_num += (ptr2_yd - ptr1_yd);
	printf("共%d条移动终端数据已录入完成。\n\n", yd_num);
	file_yd = true;
		
	if (cnt == 0) {
		printf("数据为空，请检查文件是否正常\n\n");
		return;
	}
	//步骤1：利用基站坐标中的边界条件，创建并初始化根节点
	for (int i = 0; i < cnt; i++) {
		t.bottom = min(t.bottom, jz[i].y);
		t.top = max(t.top, jz[i].y);
		t.left = min(t.left, jz[i].x);
		t.right = max(t.right, jz[i].x);
	}
	initTree(&root, t, Origin);
	//步骤2：将基站数据插入四叉树
	for (int i = 0; i < cnt; i++) {
		insert(root, jz[i]);
	}

	printf("四叉树已经建立完毕！共使用了%d个基站。\n所有数据导入完毕！\n\n ", cnt);
}


//代码量：280行

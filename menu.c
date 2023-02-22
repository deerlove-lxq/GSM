#define _CRT_SECURE_NO_WARNINGS  1
#pragma warning(disable:6031)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "function.h"

extern Node jz[MaxSize];	//从文件中读取约8000+基站的信息
extern int cnt;				//基站总个数 = cnt + 1
extern bool file_jz1, file_jz2, file_wz, file_yd;	//保证数据只被用户导入一次
extern QuadTree* root;		//四叉树树根

//数据导入菜单
void data_menu() {
	int op = -1, num = 7;
	FILE* fp;
	Node* ptr1, * ptr2;	//指向尚未添加的节点
	
	region t;
	initRegion(&t);

	char menu[10][100] = { "0-退出 ", "1-导入基站1数据 ", "2-导入基站2数据 ", "3-导入伪基站数据 ", "4-导入移动数据 ", "5-生成四叉树 ", "6-返回上级菜单 " };
	printf("------------------------------\n");		//30个*
	printf("|      输入数字选择功能      |\n");
	printf("------------------------------\n");
	for (int i = 0; i < num; i++) {
		printf("|%-28s|\n", menu[i]);
	}
	printf("------------------------------\n\n");
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
		ptr1 = &(jz[cnt]);
		fp = fopen("./data/jz001.txt", "r");
		printf("jz001文件读取中：请稍后……\n");
		ptr2 = readFile(fp, ptr1);
		fclose(fp);
		cnt += (ptr2 - ptr1);
		printf("共%d条基站数据已录入完成。\n\n", cnt);
		file_jz1 = true;
		break;
	case 2:
		if (file_jz2) {
			printf("jz002.txt已被成功导入！请勿重复操作\n\n");
			break;
		}
		ptr1 = &(jz[cnt]);
		fp = fopen("./data/jz002.txt", "r");
		printf("jz002文件读取中：请稍后……\n");
		ptr2 = readFile(fp, ptr1);
		fclose(fp);
		cnt += (ptr2 - ptr1);
		printf("共%d条基站数据已录入完成。\n\n", cnt);
		file_jz2 = true;
		break;
	case 3:
		//待完成
		if (file_wz) {
			printf("wz001.txt已被成功导入！请勿重复操作\n\n");
			break;
		}
		fp = fopen("./data/wz001.txt", "r");
		printf("wz001文件读取中：请稍后……\n");
		fclose(fp);
		file_wz = true;
		break;
	case 4:
		//待完成
		if (file_yd) {
			printf("yd001.txt已被成功导入！请勿重复操作\n\n");
			break;
		}
		fp = fopen("./data/yd001.txt", "r");
		printf("yd001文件读取中：请稍后……\n");
		fclose(fp);
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
	case 6:
		return;
	}
}
//函数功能菜单
void func_menu() {
	int op = -1, num = 7;
	double x, y, r;
	char menu[20][50] = { "0-退出 ", "1-查找最西(东)北(南)的基站 ", "2-查找指定范围内的基站 ", 
		"3-选择信号最强的基站 ", "4- ", "5- ", "6- "};
	printf("------------------------------\n");		//30个
	printf("|      输入数字选择功能      |\n");
	printf("------------------------------\n");
	for (int i = 0; i < num; i++) {
		printf("|%-28s|\n", menu[i]);
	}
	printf("------------------------------\n\n");
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
		searchBoundary(root, SE);
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
		printf("请输入该范围的圆心(查询点)坐标: ");
		scanf("%lf %lf", &x, &y);
		puts("");
		query_intensity(root, x, y);
		break;
	case 4:
		break;
	case 5:
		break;
	case 6:
		break;
	}
}
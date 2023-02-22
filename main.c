﻿//消除vs不能使用scanf的限制
#define _CRT_SECURE_NO_WARNINGS  1
#pragma warning(disable:6031)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "function.h"

Node jz[MaxSize];	//从文件中读取约8000+基站的信息
int cnt;			//基站总个数 = cnt + 1
bool file_jz1, file_jz2, file_wz, file_yd;	//保证数据只被用户导入一次
QuadTree* root;		//四叉树树根

int main() {
	int op = -1, num = 3;
	char menu[5][50] = { "0-退出 ", "1-导入数据信息 ", "2-打开功能菜单 " };
	while (op != 0) {
		printf("------------------------------\n");		//30个*
		printf("|      输入数字选择功能      |\n");
		printf("------------------------------\n");
		for (int i = 0; i < num; i++) {
			printf("|%-28s|\n", menu[i]);
		}
		printf("------------------------------\n\n");
		printf("请输入您的选择: ");
		scanf("%d", &op);
		//进入二级菜单
		while (op >= 3 || op < 0) {
			printf("您输入的选项不合法！请重新输入: ");
			scanf("%d", &op);
		}
		switch (op) {
		case 0:
			break;
		case 1:
			data_menu();
			break;
		case 2:
			func_menu();
			break;
		}
	}

	/*功能：释放空间摧毁四叉树*/
	destroyTree(root);
	printf("\n使用完毕，内存已经完全释放！\n");
	return 0;
}
//消除vs不能使用scanf的限制
#define _CRT_SECURE_NO_WARNINGS  1
#pragma warning(disable:6031)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "function.h"

Node jz[MaxSize];							//从文件中读取约8000+基站的信息
Node gaosu[20];								//单独存储少量的高速基站信息
int cnt_gao;								//高速基站个数
int cnt;									//基站总个数 = cnt + 1
bool file_jz1, file_jz2, file_wz, file_yd;	//保证数据只被用户导入一次
QuadTree* root;								//四叉树树根

Terminal yd[100];	//存储终端移动情况
Fake wz[100];		//存储伪基站移动情况
int yd_num, wz_num;	//存储终端、伪基站的数据条数（12/5）

int main() {
	int op = -1, num = 4;
	char menu[5][50] = { "0-退出", "1-导入数据信息", "2-导入全部数据并生成四叉树(快捷)", "3-打开功能菜单"};
	while (op != 0) {
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
		while (op >= 4 || op < 0) {
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
			shortcut();
			break;
		case 3:
			if (!file_jz1 && !file_jz2) {
				printf("请先导入基站数据！\n\n");
				break;
			}
			func_menu();
			break;
		}
	}

	/*功能：释放空间摧毁四叉树*/
	destroyTree(root);
	printf("\n使用完毕，内存已经完全释放！\n");
	return 0;
}

//代码量：66行
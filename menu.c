#define _CRT_SECURE_NO_WARNINGS  1
#pragma warning(disable:6031)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "function.h"

extern Node jz[MaxSize];							//从文件中读取约8000+基站的信息
extern Node gaosu[20];								//单独存储少量的高速基站信息
extern int cnt_gao;									//高速基站个数
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
		createTree(root, jz, cnt);
		printf("四叉树已经建立完毕！共使用了%d个基站\n\n ", cnt);
		break;
	}
}

//函数功能菜单
void func_menu() {
	int op = -1, num = 9, dir = -1, n = -1, m = -1, op_route = 0, op_jz = 1, op_wz = 0;
	int res_cnt = 0, plan = 1;
	Node res[50], pre_jz, back_jz, goal_jz;
	double x1, y1, x2, y2, x3, y3;

	double foot = 20;
	double its, k, dx, dy, dr;
	
	Terminal t, tmp_t1, tmp_t2, t1, t2, t3, start_t;
	Fake w;
	int t_jz_num = 0, id_pre = -1, id_wz = -1;
	int id_its = -1, goal_id = -1, first_idx = 0;
	int tmp = 0, all_id[100], tmp_num = 0;

	//为第5问设计的数据 - 菜单中的功能5
	int total_jz = 0, total_none = 0, total_city = 0, total_highway = 0, total_town = 0;
	int change_times = 0, total_change = 0;

	bool sign = false;
	double x, y;
	char str[10];
	char menu[20][100] = { "0-主目录", "1-查找最西(东)北(南)基站", "2-查找指定矩形区域内的基站", 
		"3-指定点是否处在无信号区域，输出信号最强的基站", "4-指定容量下平均单元块大小和叶子包含的基站数量", 
		"5-输出依次切换的基站和通信连接状态", "6-离开/进入第k条线路的第n个基站的有效信号范围", 
		"7-计算通过首个重叠区的时间长度", "8-查询是否能连上伪基站"};

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
	while (op >= 9 || op < 0) {
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
		printf("请输入该矩形范围的边界信息（顺序输入左下右上边界：x1 y1 x2 y2）");	
		scanf("%lf %lf %lf %lf", &x1, &y1, &x2, &y2);
		puts("");
		printf("选择1：通过从上而下的方式进行区域划分（100%%精度，效率略低）\n");
		printf("选择2：通过自下而上的方式进行九宫格查找（含有近似，效率更高）\n请输入您的选择：");
		scanf("%d", &plan);
		while (plan > 2 || plan < 1) {
			printf("您的输入不合法！请重新输入：");
			scanf("%d", &plan);
		}
		puts("");

		if (plan == 1) {
			printf("方法1：利用平面切割的均匀性，只要略有重合区域就去搜，因为平面内任何区域内总会被分割，所以保证了搜索的全面性。另外此方法不需要考虑重复录入。\n不足：考虑情况略多，效率略低。\n\n");
			query_rec(root, x1, y1, x2, y2, res, &res_cnt);	//区域划分法
		}
			
		else if (plan == 2) {
			printf("方法2：采用九宫格法覆盖原区域，采用最小的环绕区域，极大地缩小了范围，然后再进行二次筛选。");
			printf("另外巧妙运用哈希表散列基站ID，使用尽量小的空间保证基站不重复被考虑\n");
			printf("不足：对于一小部分区域的搜索可能并不全面，包含近似所以精度略低。\n\n");
			query_rec_2(root, x1, y1, x2, y2, res, &res_cnt);	//九宫格法
		}

		if (res_cnt == 0) printf("(%.2lf, %.2lf, %.2lf, %.2lf)区域内无基站", x1, y1, x2, y2);
		else {
			printf("(%.2lf, %.2lf, %.2lf, %.2lf)区域内的基站：\n", x1, y1, x2, y2);
			for (int i = 0; i < res_cnt; i++) {
				printf("%d - (%.1lf, %.1lf) - %s\n", res[i].ID, res[i].x, res[i].y, res[i].loc);
			}
		}
		puts("\n");
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
		printf("请问需要设置多少个随机点去测试九宫格内部的平均基站数量？请输入正整数m：");
		scanf("%d", &m);
		puts("");
		query_region(root, n, m);
		break;


	case 5:
		printf("不考虑伪基站的条件下, 输出%d条路线经过的基站情况\n请输入步长：", yd_num);
		scanf("%lf", &foot);
		
		for (int i = 0; i < yd_num; i++) {
			t = yd[i];
			t_jz_num = 0;
			change_times = 1;
			total_city = total_highway = total_town = 0;
			its = cal_intensity(root, t.xs, t.ys, &id_its);
			k = t.full_dist / foot;
			printf("fdist=%.1lf,步长:%.1lf米,", t.full_dist, foot);
			printf("%.2lf秒,k=%.1lf\n(%d)\n", foot / t.speed, k, i + 1);

			//输出起始位置
			
			its = cal_intensity(root, t.xs, t.ys, &id_its);

			if (id_its != -1) {
				t_jz_num++;
				judge_loc(&total_city, &total_town, &total_highway, search_id_jz(id_its));
				printf("%d = %d时%d分%.3lf秒, 信号强度为：%.3lf, 类型为%s\n", id_its, t.hour, t.minute, t.seconds, its, search_id_jz(id_its).loc);
			}
			else {
				printf("0 = %d时%d分%.3lf秒, 信号强度为0\n", t.hour, t.minute, t.seconds);
			}

			tmp = id_its;

			//按步长遍历
			for (int j = 0; j < k; j++) {

				//计算出单步后的下一个位置：t，记录上一个位置为pre_t
				t = cal_position(t, foot);
				its = cal_intensity(root, t.xs, t.ys, &id_its);

				if (id_its != tmp && id_its != -1) {
					t_jz_num++, change_times++;
					judge_loc(&total_city, &total_town, &total_highway, search_id_jz(id_its));
					printf("%d = %d时%d分%.3lf秒, 信号强度为：%.3lf, 类型为%s\n", id_its, t.hour, t.minute, t.seconds, its, search_id_jz(id_its).loc);
				}
				else if (id_its != tmp && id_its == -1) {
					change_times++;
					//输出上一个位置的信息
					printf("0 = %d时%d分%.3lf秒, 信号强度为0\n", t.hour, t.minute, t.seconds);
				}

				tmp = id_its;
			}

			printf("第%d条路线：终端共经过了%d个基站，共切换了%d次\n", i + 1, t_jz_num, change_times);
			printf("合计高速基站%d个，乡镇基站%d个，城区基站%d个\n", total_highway, total_town, total_city);
			puts("\n");
		}
		break;


	case 6:
		printf("请输入分析的移动路线编号（1 - %d）：", yd_num);
		scanf("%d", &op_route);
		t = yd[op_route - 1];
		start_t = t;
		printf("请输入想查询第几个基站：");
		scanf("%d", &op_jz);
		printf("第%d条路线中，移动终端经过的第%d个基站的进入/离开信息如下：\n", op_route, op_jz);

		//功能实现：移动终端在切换连接基站的过程中的信息
		foot = 5;
		t_jz_num = 0;
		k = t.full_dist / foot;
		tmp = id_its;

		for (int j = 0; j < k; j++) {
			if (t_jz_num == op_jz - 1) x1 = t.xs, y1 = t.ys;
			else if (t_jz_num == op_jz && id_its != -1) {
				x2 = t.xs, y2 = t.ys;
				goal_id = id_its;
			}	
			else if ((t_jz_num == op_jz && id_its == -1) || t_jz_num == op_jz + 1) {
				x3 = t.xs, y3 = t.ys;
				break;
			}
			t = cal_position(t, foot);
			its = cal_intensity(root, t.xs, t.ys, &id_its);
			if (id_its != tmp && id_its != -1) t_jz_num++;
			tmp = id_its;
		}
		printf("进入该基站范围的坐标在(%.1lf, %.1lf)和(%.1lf, %.1lf)之间迭代\n", x1, y1, x2, y2);
		printf("离开该基站范围的坐标在(%.1lf, %.1lf)和(%.1lf, %.1lf)之间迭代\n", x2, y2, x3, y3);
		x = x2, y = y2;	//因为会函数会修改x2, y2所以保存一份
		
		bisection(&x1, &y1, &x2, &y2, goal_id);
		dx = x1 - start_t.xs, dy = y1 - start_t.ys, dr = sqrt(dx * dx + dy * dy);
		tmp_t1 = cal_position(start_t, dr);
		printf("误差为0.1米：%d时%d分%.4lf秒，在(%.1lf, %.1lf)-(%.1lf, %.1lf)处进入基站%d\n", tmp_t1.hour, tmp_t1.minute, tmp_t1.seconds, x1, y1, x2, y2, goal_id);
		
		bisection(&x, &y, &x3, &y3, goal_id);
		dx = x - start_t.xs, dy = y - start_t.ys, dr = sqrt(dx * dx + dy * dy);
		tmp_t2 = cal_position(start_t, dr);
		printf("误差为0.1米：%d时%d分%.4lf秒，在(%.1lf, %.1lf)-(%.1lf, %.1lf)处离开基站%d\n", tmp_t2.hour, tmp_t2.minute, tmp_t2.seconds, x, y, x3, y3, goal_id);
		
		goal_jz = search_id_jz(goal_id);
		printf("综上所述：第%d条路线终端经过的第%d个基站为：类型为%s，编号为%d\n", op_route, op_jz, goal_jz.loc, goal_jz.ID);
		printf("终端在该基站的区域内持续时间为%.1lf秒", cal_time(tmp_t1, tmp_t2));
		puts("\n");
		break;


	case 7:
		printf("请输入分析的移动路线编号（1 - %d）：", yd_num);
		scanf("%d", &op_route);
		printf("第%d条线路遇到的首个重叠区域为：\n\n", op_route);
		
		foot = 5;
		t = yd[op_route - 1];
		k = t.full_dist / foot;
		cal_intensity(root, t.xs, t.ys, &id_its);
		id_pre = id_its;
			
		//t1为前一个位置; t2为内部位置; tmp为第一个基站id，id_its为第二个基站id
		for (int i = 0; i < k; i++) {
			t = cal_position(t, foot);
			cal_intensity(root, t.xs, t.ys, &id_its);	//获取当前位置连接的基站id_its
			if (id_its != id_pre && id_its != -1 && id_pre != -1) {
				//当前连接与上次不同且当前和之前不为空
				pre_jz = search_id_jz(id_pre), back_jz = search_id_jz(id_its);
				if (query_in(t, pre_jz) && query_in(t, back_jz)) {
					//在两个基站的共同区域
					t2 = t;
					tmp = i;	//存储步数
					break;
				}
			}
			id_pre = id_its;	//记录上一次的基站id
		}

		printf("从基站%d切换到基站%d\n", id_pre, id_its);

		//t1位置任取前面任何一个都可以
		t1 = yd[op_route - 1];
		
		//t3位置已经离开前一个基站的有效范围
		t = yd[op_route - 1];
		for (int i = tmp; i < k; i++) {
			t = cal_position(t, foot);
			if (!query_in(t, pre_jz)) {
				t3 = t;
				break;
			}
		}

		//对t1 & t2, t2 & t3分别用二分法找到边界
		t = yd[op_route - 1];
		x1 = t1.xs, y1 = t1.ys, x2 = t2.xs, y2 = t2.ys;
		printf("在(%.1lf, %.1lf)和(%.1lf, %.1lf)之间迭代逼近（以误差0.1米逼近）\n", x1, y1, x2, y2);
		bisection(&x1, &y1, &x2, &y2, id_its);
		dx = x1 - t.xs, dy = y1 - t.ys, dr = sqrt(dx * dx + dy * dy);
		tmp_t1 = cal_position(t, dr);
		printf("误差为0.1米：%d时%d分%.4lf秒，在(%.1lf, %.1lf)-(%.1lf, %.1lf)处进入基站%d\n", tmp_t1.hour, tmp_t1.minute, tmp_t1.seconds, x1, y1, x2, y2, id_its);

		x1 = t2.xs, y1 = t2.ys, x2 = t3.xs, y2 = t3.ys;
		printf("在(%.1lf, %.1lf)和(%.1lf, %.1lf)之间迭代逼近（以误差0.1米逼近）\n", x1, y1, x2, y2);
		bisection(&x1, &y1, &x2, &y2, id_pre);
		dx = x1 - t.xs, dy = y1 - t.ys, dr = sqrt(dx * dx + dy * dy);
		tmp_t2 = cal_position(t, dr);
		printf("误差为0.1米：%d时%d分%.4lf秒，在(%.1lf, %.1lf)-(%.1lf, %.1lf)处离开基站%d\n", tmp_t2.hour, tmp_t2.minute, tmp_t2.seconds, x1, y1, x2, y2, id_pre);

		printf("在重合区域持续时间为%.1lf秒", cal_time(tmp_t1, tmp_t2));

		puts("\n");
		break;

	case 8:
		printf("请输入分析的移动路线编号（1 - %d）：", yd_num);
		scanf("%d", &op_route);
		printf("请输入分析的伪基站ID：");
		scanf("%d", &op_wz);
		
		//功能实现：判断该伪站和该移动终端连接的时间
		w = search_id_wz(op_wz);
		t = yd[op_route - 1];
		printf("伪基站%d相对于第%d条移动路线：\n", op_wz, op_route);
		connect_wz(t, w);
		puts("\n");
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
		printf("所有数据已被成功导入！请勿重复操作\n\n");
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
	createTree(root, jz, cnt);

	printf("四叉树已经建立完毕！共使用了%d个基站。\n所有数据导入完毕！\n\n ", cnt);
}

//生成四叉树
void createTree(QuadTree* root, Node* arr, int n) {
	if (root == NULL) {
		printf("树根尚未进行初始化！\n\n");
		return;
	}

	//终止条件，数组的元素个数小于容量，则直接填入，该点作为叶子
	if (n <= MAXJZ) {
		for (int i = 0; i < n; i++) {
			root->nodes[(root->nodesNum)++] = arr[i];
		}
		return;
	}

	//否则发生点的分裂，分裂为4个子叶子
	int num_nw = 0, num_ne = 0, num_sw = 0, num_se = 0;
	int nw = 0, ne = 0, sw = 0, se = 0;
	double root_x = (root->boundary.left + root->boundary.right) / 2;
	double root_y = (root->boundary.bottom + root->boundary.top) / 2;

	//将数组按象限分为四个数组
	for (int i = 0; i < n; i++) {
		Node node = arr[i];
		//压x和y轴正向或原点算在东北框内。
		if (node.x >= root_x && node.y >= root_y) num_ne++;
		else if (node.x > root_x && node.y < root_y) num_se++;
		else if (node.x < root_x && node.y > root_y) num_nw++;
		else if (node.x <= root_x && node.y <= root_y) num_sw++;
	}
	//构造四个数组，以实现递归的形式统一
	Node* node_nw = (Node*)malloc(num_nw * sizeof(Node));
	Node* node_ne = (Node*)malloc(num_ne * sizeof(Node));
	Node* node_sw = (Node*)malloc(num_sw * sizeof(Node));
	Node* node_se = (Node*)malloc(num_se * sizeof(Node));

	for (int i = 0; i < n; i++) {
		Node node = arr[i];
		//不可能正好与基站重合，压x和y轴正向或原点算在东北框内。
		if (node.x >= root_x && node.y >= root_y) node_ne[ne++] = node;
		else if (node.x > root_x && node.y < root_y) node_se[se++] = node;
		else if (node.x < root_x && node.y > root_y) node_nw[nw++] = node;
		else if (node.x <= root_x && node.y <= root_y) node_sw[sw++] = node;
	}

	root->is_leafNode = false;	//分裂后不再是叶子
	
	//初始化四个树叶
	region r = root->boundary;
	initTree(&(root->ne), r, NE);
	initTree(&(root->se), r, SE);
	initTree(&(root->nw), r, NW);
	initTree(&(root->sw), r, SW);

	//递归构造子树
	createTree(root->ne, node_ne, num_ne);
	createTree(root->nw, node_nw, num_nw);
	createTree(root->se, node_se, num_se);
	createTree(root->sw, node_sw, num_sw);
		
	//回收动态分配的内存
	free(node_nw);
	free(node_ne);
	free(node_sw);
	free(node_se);
	
}

//根据id搜索基站
Node search_id_jz(int id) {
	Node ans;
	if (id <= 0) {
		printf("输入的id不合法！");
		exit(1);
	}
	for (int i = 0; i < cnt; i++) {
		if (jz[i].ID == id) {
			ans = jz[i];
			break;
		}
	}
	return ans;
}

//根据id搜索伪站
Fake search_id_wz(int id) {
	Fake ans;
	if (id <= 0) {
		printf("输入的id不合法！");
		exit(1);
	}
	for (int i = 0; i < wz_num; i++) {
		if (wz[i].ID == id) {
			ans = wz[i];
			break;
		}
	}
	return ans;
}

//计算何时连接/断开伪基站
void connect_wz(Terminal yd_start, Fake wz_start) {
	Terminal t1 = yd_start, t2 = yd_start;		//记录刚连接，刚断开，两个时刻的位置信息
	Terminal t = yd_start;						//t记录终端行进位置
	Fake w = wz_start;							//w记录伪站行进位置
	double foot = 0.1;							//步长foot
	double dist_yd = foot * t.speed, dist_wz = foot * w.speed;	//换算为步长dist
	double k = w.full_time / foot;								//次数（以伪站停止运动为主)
	bool flag_in = false, flag_out = false;						//判断是否有连接时间

	//由于开始时间不同，所以第一步应当使得时间统一
	double dt = (t.hour * 3600 + t.minute * 60 + t.seconds) - (w.hour * 3600 + w.minute * 60 + w.seconds);
	if (dt > 0) {
		//说明伪基站先出发，经过dt后终端出发
		w = cal_position_wz(w, dt * w.speed);
	}
	else if (dt < 0) {
		//说明终端先出发，经过dt后伪基站出发
		t = cal_position(t, (-dt) * t.speed);
	}
	
	//功能实现：固定时间步长移动，不断探测与伪基站的距离是否在有效距离以内
	double dx = t.xs - w.xs, dy = t.ys - w.ys;
	double r = sqrt(dx * dx + dy * dy), pre_r = r;	//距离伪基站的距离
	int tmp = 0;	//临时保存步数

	if (r <= w.valid_dist) {
		//刚开始就连上了
		t1 = t;
		flag_in = true;
	}
	else {
		//刚开始未连上
		for (int i = 0; i < k; i++) {
			t = cal_position(t, dist_yd);
			w = cal_position_wz(w, dist_wz);
			dx = t.xs - w.xs, dy = t.ys - w.ys;
			r = sqrt(dx * dx + dy * dy);
			if (r <= w.valid_dist && pre_r >= w.valid_dist) {
				//说明找到了刚连接的坐标
				t1 = t;
				tmp = i;
				flag_in = true;
				break;
			}
			pre_r = r;
		}
	}
	
	for (int i = tmp; i < k; i++) {
		t = cal_position(t, dist_yd);
		w = cal_position_wz(w, dist_wz);
		dx = t.xs - w.xs, dy = t.ys - w.ys;
		r = sqrt(dx * dx + dy * dy);
		if (r > w.valid_dist) {
			//说明找到了刚离开的坐标
			t2 = t;
			flag_out = true;
			break;
		}
	}

	if (flag_in && !flag_out) t2 = t;
	if (!flag_in) printf("没有连接的时间!\n");
	else printf("终端从%d时%d分%.1lf秒开始与伪基站连接，到%d时%d分%.1lf秒断开连接。共持续了%.1lf秒的连接时间。\n", t1.hour, t1.minute, t1.seconds, t2.hour, t2.minute, t2.seconds, cal_time(t1, t2));
}

//代码量：680行

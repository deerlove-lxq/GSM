#include <graphics.h>
#include <iostream>
#include "cpp_func.hpp"

using namespace std;

void draw_cpp(double right, double left, double top, double bottom) {
	int origin_x = (right + left) / 2, origin_y = (top + bottom) / 2;	//指定原点的坐标，用于换算
	int width = right - left, height = top - bottom;
	initgraph(width, height);	//创建窗口
	setorigin(width / 2, height / 2);
	setaspectratio(1, -1);
	for (int i = 0; i < draw_cnt; i++) {
		putpixel((draw_jz_x[i] - origin_x) * 5, (draw_jz_y[i] - origin_y) * 15, WHITE);
	}
	
	line(0, 0, 50, 50);
	_gettch();
	closegraph();//关闭窗口
}



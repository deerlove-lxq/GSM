#ifndef _CPP_FUNC_HPP
#define _CPP_FUNC_HPP

#ifdef __cplusplus     
extern "C" {                     
#endif                               

void draw_cpp(double right, double left, double top, double bottom);    //要被c函数调用

extern int cnt;
extern double draw_jz_x[10000], draw_jz_y[10000];
extern int draw_cnt;

#ifdef __cplusplus      
}
#endif

#endif // !_CPP_FUNC_HPP

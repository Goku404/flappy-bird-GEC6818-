#ifndef __LCD_H_
#define __LCD_H_
#include <sys/mman.h>

static int fd_fb;     //帧缓冲设备文件描述符
static int* plcd = NULL;      //帧缓冲的首地址

//打开帧缓冲设备文件
void lcd_open();

//关闭帧缓冲设备文件
void lcd_close();

//指定位置画点
void draw_point(int x, int y, unsigned int color);

//画矩形 参数(x0,y0)位置画一个高为height,宽为width的矩形
void lcd_draw_rectangle(int x0,int y0,int height,int width);


#endif
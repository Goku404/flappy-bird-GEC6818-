#ifndef __BMP_H_
#define __BMP_H_

//显示图片
void bmp_display(const char *bmpfile, int x0, int y0);

//搜索图片
int search_pics(char (*filename)[50],int *n);


#endif
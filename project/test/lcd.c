//lcd的操作接口
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include "lcd.h"

//打开帧缓冲设备文件
void lcd_open()
{
    //打开文件,读写打开方式，建立映射
    fd_fb = open("/dev/fb0",O_RDWR);
    if (-1 == fd_fb)
    {
        //打开失败
        perror("open lcd error");
        return;
    }

    //建立映射
    plcd =  mmap(NULL, 800*480*4, PROT_READ | PROT_WRITE, MAP_SHARED, fd_fb, 0);
    
    if (MAP_FAILED == plcd)
    {
        //映射失败
        perror("mmap error");
        close(fd_fb);
    }
}

//关闭帧缓冲设备文件
void lcd_close()
{
    //关闭文件
    close(fd_fb);
    //解除映射
    munmap(plcd,sizeof(fd_fb));
}

//指定位置画点
void draw_point(int x, int y, unsigned int color)
{
    if ((x >= 0)&&(x < 800)&&(y >= 0)&&(y < 480))
    {
        *(plcd+x+y*800) = color;
    }
}

//画矩形
void lcd_draw_rectangle(int x0,int y0,int height,int width)
{
    int x=x0+width,y=y0+height;
    int i,j;
    if(x<800 && y<480)   //判断边界
    {
        for(i=x0;i<x;i++)
        {
            for (j=y0;j<y;j++)
            {
                draw_point(i,j,0x00141414);
            } 
        }
    }
    else
    {
        printf("越界啦!");
    }
}

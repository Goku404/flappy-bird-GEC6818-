//显示BMP图片
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "lcd.h"
#include "bmp.h"
#include <dirent.h>
#include <string.h>

void bmp_display(const char *bmpfile, int x0, int y0)
{   
    // 打开图片文件
    int fd_pic = open(bmpfile,O_RDONLY);
    if (-1 == fd_pic)
    {
        //打开图片失败
        perror("open pic error");
        return;
    }

    // 读取图片属性
    int width,height;
    short deepth;
    unsigned char buf[4];  //存储 width,height,deepth数据

    //读取图片宽度
    //设置光标位置
    lseek(fd_pic,0x12,SEEK_SET);
    read(fd_pic,buf,4);
    //小端存储转换
    width = (buf[3] << 24 )| (buf[2] << 16 ) | (buf[1] << 8 ) | (buf[0]);
    //read(fd_pic,&width,4);   (适合ARM的小端存储)

    //读取图片高度 紧接着宽度  0x16表示bmp存储高度位置
    lseek(fd_pic,0x16,SEEK_SET);
    read(fd_pic,buf,4);
    //小端存储转换
    height = (buf[3] << 24 ) | (buf[2] << 16) | (buf[1] << 8) | (buf[0]);
    //read(fd_pic,&height,4);  (适合ARM的小端存储)
    
    //读取色深  0x1C表示bmp存储色深位置 通常为 24,36 表示3字节和4字节颜色值,两个字节
    lseek(fd_pic,0x1C,SEEK_SET);
    read(fd_pic,buf,2);
    deepth = (buf[1] << 8) | (buf[0]);
    //read(fd_pic,&deepth,2);
    
    if(!(deepth == 24 || deepth == 32 ))
    {
        printf("不支持的图片格式!\n");
        close(fd_pic);
        return;
    }
   printf("width = %d,height = %d,deepth = %d\n",abs(width),abs(height),deepth);

    int line_valid_bytes ; //一行有效数据量
    int laizi = 0 ;//一行中的“赖子”数量
    int line_bytes; // 一行总字节数
    int total_bytes; //整个像素数组数据的字节数

    line_valid_bytes = abs(width) * (deepth/8);
    if (line_valid_bytes % 4)
    {
        laizi = 4 - line_valid_bytes % 4;
    }
    line_bytes = line_valid_bytes + laizi;

    total_bytes = line_bytes * abs(height);
    unsigned char *pixels = malloc( total_bytes );
    lseek(fd_pic, 54, SEEK_SET);
    read(fd_pic, pixels, total_bytes);

    //todo ...
    unsigned char a = 0, r,g,b;
    int color;
    int x1, y1 ;//像素点对应在显示屏上的坐标
    int i = 0;
    int x, y;

    for (y = 0; y < abs(height); y++)
    {
        for (x = 0; x < abs(width); x++ )
        {
            b = pixels[i++];
            g = pixels[i++];
            r = pixels[i++];
            if (deepth == 32)
            {
                a = pixels[i++];
            }
            color = (a << 24) | (r << 16) | (g << 8) | (b);

            x1 = (width > 0 ? (x0 + x) : ( x0 + abs(width) - 1 -x) );
            y1 = (height > 0 ? (y0 + abs(height) - 1 - y) : (y0 + y));

            draw_point(x1, y1, color);
        }
        i += laizi; //跳过“赖子”
    }

    // unsigned char color[width*height*deepth/8]; 
    // lseek(fd_pic,0x36,SEEK_SET);
    // read(fd_pic,color,width*height*deepth/8);

    // // 重新排列颜色值
    // //遍历图片的每个像素点，一个一个地处理
    // unsigned char *p = color;
    // for (int y = 0; y < height; y++)
    // {
    //     for (int x = 0; x < width; x++)
    //     {
    //         unsigned char a,r,g,b;
    //         b = *p;
    //         p++; 
    //         g = *p;
    //         p++;
    //         r = *p;
    //         p++;
    //         if (32 == deepth)
    //         {
    //             //图片色深为32位，即每个颜色值用4个字节存储
    //             a = *p;
    //             p++;
    //         }
    //         else if (24 == deepth)
    //         {
    //             //图片色深为24位，即每个颜色值用3个字节存储
    //             a = 0;
    //         }
    //         //重新排列颜色分量，组成一个新的颜色值
    //         unsigned int c = a << 24 | r << 16 | g << 8 | b;
            
    //         // 将重新排列好的颜色值写入到帧缓冲设备中(映射区)
    //         draw_point(x,479-y,c);
    //     }
    // }
  
    // 关闭图片文件
    close(fd_pic);
    free(pixels);
}

//专门搜索mp4,放到一个数组中
int search_pics(char (*filename)[50],int *n)
{
	struct dirent *p;
	DIR * pdir = opendir("./pic");
	if(pdir == NULL)
	{

		printf("opendir failed!\n");
		return -1;
	}

	while(1)
	{
		p = readdir(pdir);
		if(p==NULL)
			break;
		if(p->d_type==8)
		{
			if(strcmp(p->d_name+strlen(p->d_name)-4,".bmp")==0)
			{
				//printf("%s\n",p->d_name);
				//strcpy(filename[n],p->d_name);
				sprintf(filename[*n],"pic/%s",p->d_name);//"mp4/1.mp4"---->存放到数组中
				(*n)++;
			}
			
		}
	}
    printf("数组中的列表是：\n");
	int j;
	for(j=0;j<(*n);j++)
		printf("%s\n",filename[j]);

	return 0;
}
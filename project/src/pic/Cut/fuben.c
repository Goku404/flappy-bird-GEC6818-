#include <stdio.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include "bmp.h"
#include "lcd.h"
#include "jpg.h"

/** Gravitational acceleration constant */
const float GRAV = 0.05;             //重力加速度常量

/** Initial velocity with up arrow press */
const float V0 = -0.5;              //按向上箭头的初始速度

/** Represents Flappy the Bird. */
typedef struct flappy {
	/* Height of Flappy the Bird at the last up arrow press. */  
	int h0;   //Flappy the Bird在最后一个向上箭头按下时的高度。

    int h;
	/* Time since last up arrow pressed. */
	int t;   //自上次按下向上箭头后的时间。
} flappy;

//游戏区域屏幕
typedef struct game
{
    flappy g_bird;      //屏幕上的小鸟
    int *g_ptr;        //屏幕开始指针
    int height;         //屏幕高度
} game;


//清空屏幕
void clean_lcd()
{
    int i=0,j=0;
    for(;i<480;i++)
    {
        for(;j<800;j++)
        {
            draw_point(j,i,0xFFFFFF);
        }
    }

}


//清屏(游戏区域)
void clean_game(game g)  
{
    g.g_ptr = plcd + 35*800;   //屏幕开始位置g
    for(;g.g_ptr < plcd + 445*800;g.g_ptr++)
    {
        *g.g_ptr = 0xFFFFFF;       //白色
    }
}

//画屏幕
void draw(game g)
{
    clean_game(g);

    //画鸟
    bmp_display("./pic/bird.bmp",100,g.g_bird.h);
}

int get_flappy_position(flappy f) {
	return f.h0 + V0 * f.t + 0.5 * GRAV * f.t * f.t;
}

//游戏开始
void game_start()
{
    clean_lcd();
    bmp_display("./pic/wall.bmp",0,445);  //围墙
    bmp_display("./pic/wall.bmp",0,0);    //围墙



}


//小鸟游戏加载动画
// void loading()
// {
//     clean_lcd();
//     bmp_display("./pic/birds_bg.bmp",175,45);

//     //loading 图片
//     char a[3][30] = {{"./loading/loading.bmp"},
//                      {"./loading/loading2.bmp"},
//                      {"./loading/loading3.bmp"}};

//     int l_x = 150;
//     int l_y = 380;

//     bmp_display(a[2],l_x,l_y);
//     l_x = l_x + 50;
//     bmp_display(a[0],l_x,l_y);
    
//     int i;
//     for(i = l_x;i<650;i = i+50)
//     {
//         bmp_display(a[0],i,l_y);
//         if(i-50 != 150)
//         {
//             bmp_display(a[1],i-50,l_y);
//         }
//         sleep(1);
//     }
//     bmp_display("./pic/birds_start.bmp",175,45);

//     int x = 0; //之前
//     int y = 0;
//     struct input_event ev;//定义一个结构体变量用来保存输入事件的值

//     // 打开触摸屏的设备文件(/dev/ipnut/event0)
//     int fd_touch = open("/dev/input/event0",O_RDONLY);
//     if (-1 == fd_touch)
//     {
//         //打开触摸屏设备失败
//         perror("open fd_touch error");
//         return ;
//     }

//     // 从触摸屏文件中读取数据
    
//     while (1)       //加载开机动画
//     {
       
//        read(fd_touch,&ev,sizeof(ev));
//        //按下按键
//        if(ev.type == 3 && ev.code == 0)
//        {
//            x = ev.value * 800 /1024;
//        }
    
//         if(ev.type == 3 && ev.code == 1)
//        {
//            y = ev.value * 480 /800;
//        }
        
//        if(ev.type == 3 && ev.code == 1)
//        {
//             if(x < 625 && x > 175 && y>45 && y<345)
//             {
//                 close(fd_touch);
//                 clean_lcd();  
//                 Bird();
//             }
//        }
//     }
// }

void Bird()
{
    bmp_display("./pic/bird.bmp",100,100);
    bmp_display("./pic/wall.bmp",0,445);
    bmp_display("./pic/wall.bmp",0,0);
    bmp_display("./pic/water.bmp",410,245);
}

int main()
{
    //int i,j;
    lcd_open();

    // char pics[20][50]={0};
    // int n = 0;

    // search_pics(pics,&n);  //搜索图片
    // loading();
    //Bird();
    

    



    lcd_close();


    return 0;
}
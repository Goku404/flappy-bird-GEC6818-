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
#include <pthread.h>
#include<signal.h>
#include "bmp.h"
#include "lcd.h"
#include "jpg.h"

/** Gravitational acceleration constant */
const float GRAV = 0.06;             //重力加速度常量

/** Initial velocity with up arrow press */
const float V0 = -0.5;              //按向上箭头的初始速度

/**撞墙标志位**/
static int CRASH_WALL;      

/**撞水管标志位**/
static int CRASH_PIPE;

/*水管开口高度*/
static int OPEN = 166;

/*分数*/
static int SCORE;

/*难度,难度越高，水管移动越快*/
static int LEVEL;

/** Represents Flappy the Bird. */
typedef struct flappy {
	/* Height of Flappy the Bird at the last up arrow press. */  
    int h;           //小鸟高度    
	/* Time since last up arrow pressed. */
	int t;   //自上次按下向上箭头后的时间。
} flappy;

/** pipe **/
typedef struct waterpipe
{
    int Open_h;             //管子1开口高度
    int pipe_x;             //管子1画的x轴坐标

    int Open_h1;            //管子2开口高度
    int pipe_x1;            //管子2画的x轴高度    

    int Open_h2;            //管子3开口高度
    int pipe_x2;            //管子3的x轴高度
} waterpipe;

//游戏区域屏幕
typedef struct game
{
    int time;
    flappy *g_bird;      //屏幕上的小鸟
    waterpipe *g_pipe;  //水管
    int height;         //屏幕高度
} game;

//清屏
void clean_lcd();

//刷屏
void draw_game(game);

//线程函数，读取显示屏
void* touch(int *);

//游戏开始
void game_start();

//画管子  (参数，帧缓冲设备指针和pipe结构体)
void draw_game2(game);

//画管子
void *draw_pipe(game);

//更新分数
void fresh_score();

//清空屏幕
void clean_lcd()
{
    for(int i=0;i<480;i++)
    {
        for(int j=0;j<800;j++)
        {
            draw_point(j,i,0xFFFFFF);
        }
    }

}


//清屏(游戏区域)
// void clean_game(game *g)  
// {
//        //屏幕开始位置g
//     for(g->g_ptr = p + 35*800;g->g_ptr < plcd + 445*800;g->g_ptr++)
//     {
//        *(g->g_ptr) = 0x00FF00;       //白色
//     }

// }

//画屏幕
void draw_game(game G)
{
    //clean_game(g);
    for(int i = 35;i<445;i++)
    {
        for(int j = 0;j<800;j++)
        {
            draw_point(j,i,0xFFFFFF);
        }
    }
    //printf("%d\n",G.g_bird->h);

    //画鸟
    bmp_display("./pic/bird.bmp",100,G.g_bird->h);
    usleep(100000* 0.8);
}


//线程函数 (读取触摸屏)
void* touch(int *time)
{
    //pthread_detach(pthread_self());
    int fd_touch = open("/dev/input/event0",O_RDONLY);
    if (-1 == fd_touch)
    {
        //打开触摸屏设备失败
        perror("open fd_touch error");
        return NULL;
    }
    struct input_event ev;
    //回收线程资源
    //pthread_detach(pthread_self());
    while(1)
    {
        //如果撞墙了
        if(CRASH_WALL == 1)
        {
            close(fd_touch);
            pthread_detach(pthread_self());
            printf("bye bye\n");
            pthread_exit(0);
        }
        read(fd_touch,&ev,sizeof(ev));     //读取触摸屏
        if(ev.type == 3)
        {
            *time = 0;
        }
    }
}


//参考参数
void Bird()
{
    clean_lcd();
    bmp_display("./pic/bird.bmp",100,100);
    bmp_display("./pic/wall.bmp",0,445);
    bmp_display("./pic/wall.bmp",0,0);
    bmp_display("./pic/water.bmp",410,245);
}

//画水管
void draw_game2(game G)
{
    //画小鸟
    int h = G.g_bird->h;
    int mid = h + 29;

    //解析小鸟
    int fd_bird = open("./pic/bird.bmp",O_RDONLY);
    if(fd_bird == -1)
    {
        perror("open bird.bmp error");
        return;
    }
    
    unsigned char *B_color = malloc(60*58*24/8);
    lseek(fd_bird,0x36,SEEK_SET);
    read(fd_bird,B_color,60*58*24/8);
    unsigned char *B_p = B_color;
    unsigned char b_a,b_r,b_g,b_b;  

    //解析水管
    int fd_pipe = open("./pic/water.bmp",O_RDONLY);
    if(fd_pipe == -1)
    {
        perror("open pipe.bmp error");
        return NULL;
    }
    
    unsigned char *P_color = malloc(34*410*24/8);
    lseek(fd_pipe,0x36,SEEK_SET);
    read(fd_pipe,P_color,34*410*24/8);
    unsigned char *P_p = P_color;          //1号水管
    unsigned char *P_p1 = P_color;         //2号水管
    unsigned char p_a,p_r,p_g,p_b;  

    for(int i = 35;i<445;i++)
    {
        for(int j = 0;j<800;j++)
        {

            //加载小鸟
            if(h <= i && i <h+58 && 100 <= j && j<160)
            {
                //解析rgb
                b_b = *B_p;
                *B_p++;

                b_g = *B_p;
                *B_p++;

                b_r = *B_p;
                *B_p++;

                
                unsigned int b_c = b_r << 16 | b_g << 8 | b_b;
                int y = 0;
                
                //关于中心对称
                if(i - mid >= 0)
                {
                    y = mid -(i-mid);
                    draw_point(j,y,b_c);
                }

                if(i - mid < 0)
                {
                    y = mid+(mid-i);
                    draw_point(j,y,b_c);
                }
            }

            //1号上水管区域
            else if(35 <= i && i < G.g_pipe->Open_h && G.g_pipe->pipe_x <=j && j <G.g_pipe->pipe_x+34)
            {
                p_b = *P_p;
                *P_p++;

                p_g = *P_p;
                *P_p++;

                p_r = *P_p;
                *P_p++;

                unsigned int p_c = p_r << 16 | p_g << 8 | p_b;
                draw_point(j,i,p_c);
            }
            
            //1号下水管区域
            else if( G.g_pipe->Open_h + OPEN <= i && i < 445+ OPEN && G.g_pipe->pipe_x <=j && j <G.g_pipe->pipe_x+34)
            {
                p_b = *P_p;
                *P_p++;

                p_g = *P_p;
                *P_p++;

                p_r = *P_p;
                *P_p++;

                unsigned int p_c = p_r << 16 | p_g << 8 | p_b;
                draw_point(j,i,p_c);
            }


            //2号上水管区域
            else if(35 <= i && i < G.g_pipe->Open_h1 && G.g_pipe->pipe_x1 <=j && j <G.g_pipe->pipe_x1 + 34)
            {
                p_b = *P_p1;
                *P_p1++;

                p_g = *P_p1;
                *P_p1++;

                p_r = *P_p1;
                *P_p1++;

                unsigned int p_c = p_r << 16 | p_g << 8 | p_b;
                draw_point(j,i,p_c);
            }
            
            //2号下水管区域
            else if( G.g_pipe->Open_h1 + OPEN <= i && i < 445+ OPEN && G.g_pipe->pipe_x1 <=j && j <G.g_pipe->pipe_x1+34)
            {
                p_b = *P_p1;
                *P_p1++;

                p_g = *P_p1;
                *P_p1++;

                p_r = *P_p1;
                *P_p1++;

                unsigned int p_c = p_r << 16 | p_g << 8 | p_b;
                draw_point(j,i,p_c);
            }

            //留白
            else
            {
                draw_point(j,i,0xFFFFFF);
            }

            //如果小鸟撞上1号上水管
            if(h <= i && i <h+58 && 100 <= j && j<160)
            {
                if(35 < i && i < G.g_pipe->Open_h && G.g_pipe->pipe_x <j && j <G.g_pipe->pipe_x+34)
                {
                    CRASH_PIPE = 1;
                }
            }

            //如果小鸟撞上1号下水管
            if(h <= i && i <h+58 && 100 <= j && j<160)
            {
                if(G.g_pipe->Open_h + OPEN < i && i < 445+ OPEN && G.g_pipe->pipe_x <j && j <G.g_pipe->pipe_x+34)
                {
                    CRASH_PIPE = 1;
                }
            }

            //如果小鸟撞上2号上水管
            if(h <= i && i <h+58 && 100 <= j && j<160)
            {
                if(35 < i && i < G.g_pipe->Open_h1 && G.g_pipe->pipe_x1 <j && j <G.g_pipe->pipe_x1 + 34)
                {
                    CRASH_PIPE = 1;
                }
            }

            //如果小鸟撞上2号下水管
            if(h <= i && i <h+58 && 100 <= j && j<160)
            {
                if(G.g_pipe->Open_h1 + OPEN < i && i < 445+ OPEN && G.g_pipe->pipe_x1 <j && j <G.g_pipe->pipe_x1+34)
                {
                    CRASH_PIPE = 1;
                }
            }
            
        }
    }
    
    free(P_color);
    free(B_color);
    close(fd_bird);
    close(fd_pipe);

}


//更新分数
void fresh_score()
{    
    int x = SCORE % 10;    //个位数
    printf("%d\n",x);
    int y = (SCORE /10) % 10;   //十位数
    int z = (SCORE /100) % 10;           //百位数 

    char nums[10][13];
    for(int i=0;i<9;i++)
    {
        sprintf(nums[i],"./pic/%d.bmp",i);
    }

    bmp_display(nums[z],150,0);
    bmp_display(nums[y],181,0);
    bmp_display(nums[x],212,0);
}


//游戏开始
void game_start()
{
    clean_lcd();

    bmp_display("./pic/wall.bmp",0,445);  //围墙
    bmp_display("./pic/wall1.bmp",0,0);    //围墙
    bmp_display("./pic/0.bmp",150,0);
    bmp_display("./pic/0.bmp",181,0);
    bmp_display("./pic/0.bmp",212,0);

    pthread_t thread;       //触摸屏线程

    CRASH_WALL = 0;        //没撞墙
    CRASH_PIPE = 0;        //没撞水管
    SCORE = 0;             //分数为零
    LEVEL = 2;            //水管每次移动2

    game G;
    G.g_bird = NULL;
    G.g_pipe = NULL;
    G.time = 0;

    //初始化小鸟
    flappy bird;
    bird.h = 200;     //小鸟初始高度
    bird.t = 0;

    //初始化水管
    waterpipe w_pipe;
    w_pipe.Open_h = 100;     //1号开口高度
    w_pipe.pipe_x = 700;      //1号初始位置

    w_pipe.Open_h1 = 200;     //2号开口高度
    w_pipe.pipe_x1 = 400;     //2号初始位置

    
    //初始化屏幕
    G.g_bird = &bird;
    G.g_pipe = &w_pipe;

    pthread_create(&thread,NULL,&touch,&G.time);

    while(1)
    {
        //draw_game(G);
        draw_game2(G);
        G.g_bird->h = G.g_bird->h + V0*G.time + 0.5*GRAV*G.time*G.time;   //重力位移公式
        G.g_pipe->pipe_x = G.g_pipe->pipe_x -LEVEL;                          //1号水管移动两格
        G.g_pipe->pipe_x1 = G.g_pipe->pipe_x1 -LEVEL;                        //2号水管移动两格

        //1号重置
        if(G.g_pipe ->pipe_x <= 0)
        {
            G.g_pipe ->pipe_x = 700;
        }

        //2号水管重置
        if(G.g_pipe->pipe_x1 <= 0)
        {
            G.g_pipe ->pipe_x1 = 700;
        }

        G.time ++;                                                        

        //撞墙或者撞水管
        if(G.g_bird->h + 58 >445 || G.g_bird->h  <= 35 || CRASH_PIPE == 1)
        {
            bmp_display("./pic/lose.bmp",300,100);
            CRASH_WALL = 1;
            pthread_join(thread,NULL);
            return;
        }

        //过1号水管
        if(100 == G.g_pipe->pipe_x)
        {
            printf("YES\n");
            SCORE = SCORE+5;
            fresh_score();
        }

        //过2号水管
        if(100 == G.g_pipe->pipe_x1)
        {
            printf("YES\n");
            SCORE = SCORE+5;
            fresh_score();
        }

        usleep(10000* 0.7);
    }

}


//小鸟游戏加载动画
void loading()
{
    clean_lcd();
    bmp_display("./pic/birds_bg.bmp",175,45);

    //loading 图片
    char a[3][30] = {{"./loading/loading.bmp"},
                     {"./loading/loading2.bmp"},
                     {"./loading/loading3.bmp"}};

    int l_x = 150;
    int l_y = 380;

    bmp_display(a[2],l_x,l_y);
    l_x = l_x + 50;
    bmp_display(a[0],l_x,l_y);
    
    int i;
    for(i = l_x;i<650;i = i+50)
    {
        bmp_display(a[0],i,l_y);
        if(i-50 != 150)
        {
            bmp_display(a[1],i-50,l_y);
        }
        usleep(1000000 * 0.5);
    }
    bmp_display("./pic/birds_start.bmp",175,45);

    int x = 0; //之前
    int y = 0;
    struct input_event ev;//定义一个结构体变量用来保存输入事件的值

    // 打开触摸屏的设备文件(/dev/ipnut/event0)
    int fd_touch = open("/dev/input/event0",O_RDONLY);
    if (-1 == fd_touch)
    {
        //打开触摸屏设备失败
        perror("open fd_touch error");
        return ;
    }

    // 从触摸屏文件中读取数据
    
    while (1)       //加载开机动画
    {
       
       read(fd_touch,&ev,sizeof(ev));
       //按下按键
       if(ev.type == 3 && ev.code == 0)
       {
           x = ev.value * 800 /1024;
       }
    
        if(ev.type == 3 && ev.code == 1)
       {
           y = ev.value * 480 /800;
       }
        
       if(ev.type == 3 && ev.code == 1)
       {
            if(x < 625 && x > 175 && y>45 && y<345)
            {
                close(fd_touch);
                // clean_lcd();  
               
                return;
            }
       }
    }
}




int main()
{
    int * p = lcd_open();

    int x,y;
    clean_lcd();
    struct input_event ev;//定义一个结构体变量用来保存输入事件的值

    loading();               //加载动画

    start:
    game_start();

    bmp_display("./pic/fail.bmp",200,200);

        
    int fd_touch = open("/dev/input/event0",O_RDONLY);
    if (-1 == fd_touch)
    {
        //打开触摸屏设备失败
        perror("open fd_touch error");
        return -1;
    }

    while(1)
    {
        read(fd_touch,&ev,sizeof(ev));

        if (ev.type == 3 && ev.code == 0){
            x = ev.value * 800.0 / 1024;
        }

        if (ev.type == 3 && ev.code == 1){
            y = ev.value * 480.0 / 800;
        }

        if(ev.type == 3 && ev.code == 1)
        {
            if(x < 349 && x > 295 && y > 194 && y < 238)   //返回游戏
            {
                close(fd_touch);
                goto start;
            }
            if(437 < x && x < 491 && 193 < y && y < 229)   //退出游戏
            {
                close(fd_touch);
                bmp_display("./pic/gameover.bmp",0,0);
                break;
            }
        }
    }



    lcd_close();


    return 0;
}
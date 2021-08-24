#include<sys/types.h>
#include<dirent.h>
#include<stdio.h>
#include<sys/stat.h>
#include<string.h>
#include"lcd.h"
char filename[20][50]={0};
static int n = 0;
//专门搜索mp4,放到一个数组中
int search_musics()
{
	struct dirent *p;
	DIR * pdir = opendir("pic");
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
				sprintf(filename[n],"pic/%s",p->d_name);//"mp4/1.mp4"---->存放到数组中
				n++;
			}
			
		}
	}
    printf("数组中的列表是：\n");
	int j;
	for(j=0;j<n;j++)
		printf("%s\n",filename[j]);

	return 0;
}

int main()
{
    search_musics();
    return 0;
}
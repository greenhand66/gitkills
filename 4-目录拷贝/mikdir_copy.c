#include"myhead.h"
//全部目录查询只显示所有目录不显示普通文件
int show_all_dir(const char *src_dir,int deep)
{
	int i;//遍历层数
    DIR *dp = opendir(src_dir);
    if(dp == NULL)
    {
        perror("opendir failed!\n");
        return -1;
    }

    struct dirent *read_dir;
    char path[300] = {0};
    while(1)
    {
        read_dir = readdir(dp);
        if(read_dir == NULL)
            break;

        if(strcmp(read_dir->d_name,".") == 0 || strcmp(read_dir->d_name,"..") == 0)
            continue;

		if(read_dir->d_type == 8)
            continue;

		for(i=0;i!=deep;i++)
		{
			printf("│  ");
		}

        if(read_dir->d_type == 4)
        {
            printf("|—— \033[1;32m%s\033[0m\n",read_dir->d_name);
            snprintf(path,sizeof(path),"%s/%s",src_dir,read_dir->d_name);
            show_all_dir(path,deep+1);
        }
    }
    closedir(dp);
    return 0;
}
//指定目录查询显示所有目录文件与普通文件
int show_special_dir(const char *src_dir,int deep)
{
    DIR *dp = opendir(src_dir);
    if(dp == NULL)
    {
        perror("opendir failed!\n");
        return -1;
    }
	int i;
    struct dirent *read_dir;
    char path[300] = {0};
    
    while(1)
    {
        read_dir = readdir(dp);
        if(read_dir == NULL)
            break;

        if(strcmp(read_dir->d_name,".") == 0 || strcmp(read_dir->d_name,"..") == 0)
            continue;
		
		for(i=0;i!=deep;i++)
		{
			printf("│  ");
		}

        if(read_dir->d_type == 8)//普通文件	蓝色
        {
            printf("|__ \033[1;34m%s\033[0m\n",read_dir->d_name);
        }
        if(read_dir->d_type == 4)//目录文件	绿色
        {
            printf("|__ \033[1;32m%s\033[0m\n",read_dir->d_name);
            snprintf(path,sizeof(path),"%s/%s",src_dir,read_dir->d_name);
            show_special_dir(path,deep+1);
        }
       
    }
    closedir(dp);
    return 0;   
}

int copy_all_dir(const char *src_dir,const char *dest_dir)
{
	struct stat s_buf;
	if(stat(src_dir, &s_buf))
	{
		fprintf(stderr, "%s: ", src_dir);
		perror("stat failed");
		return -1;
	}

	mkdir(dest_dir, 0777);

	DIR *dp = opendir(src_dir);
	if(dp == NULL)
	{
		perror("opendir failed");
		return -2;
	}

	struct dirent *p;
	while(1)
	{
		p = readdir(dp);
		if(p == NULL)	
			break;
		if(strcmp(p->d_name, ".")==0 || strcmp(p->d_name, "..")==0)
			continue;

		if(p->d_type == 8)	// 普通文件: 1000
		{
			char src_path[300] = {0};
			char dest_path[300] = {0};
			snprintf(src_path, sizeof(src_path), "%s/%s", src_dir, p->d_name);
			snprintf(dest_path, sizeof(dest_path), "%s/%s", dest_dir, p->d_name);
			copy_file(src_path, dest_path);
		}
		else if(p->d_type == 4)	// 目录文件: 0100
		{
			char src_path[300] = {0};
			char dest_path[300] = {0};
			snprintf(src_path, sizeof(src_path), "%s/%s", src_dir, p->d_name);
			snprintf(dest_path, sizeof(dest_path), "%s/%s", dest_dir, p->d_name);
			copy_all_dir(src_path, dest_path);
		}
	}

	closedir(dp);
}

void copy_file(const char *src_name,const char *dest_name)
{
	FILE *fp = fopen(src_name, "r");
	if(fp == NULL)
	{
		perror("fopen src failed");
		return;
	}

	FILE *fp_cp = fopen(dest_name, "w");
	if(fp_cp == NULL)
	{
		perror("fopen dest failed");
		return;
	}

	char ch1, ch2;
	while(1)
	{
		ch1 = fgetc(fp);
		if(ch1 == EOF)
		{
			if(feof(fp))		
				break;
			else if(ferror(fp))	
			{
				perror("fgetc failed");
				return;
			}
		}
		
		ch2 = fputc(ch1, fp_cp);
		if(ch2 == EOF)
		{
			if(ferror(fp_cp))	
			{
				perror("fputc failed");
				return;
			}
		}
	}
	return;
}
//指定目录拷贝只拷贝目录文件不拷贝普通文件
//指定目录拷贝，会拷贝普通文件，待解决
int copy_special_dir(const char *src_dir,const char *dest_dir)
{
    DIR *dp = opendir(src_dir);
    if(dp == NULL)
    {
        perror("opendir failed!\n");
        return -1;
    }

	mkdir(dest_dir, 0777);
	
    struct dirent *read_dir;
    char path[300] = {0};
    while(1)
    {
        read_dir = readdir(dp);
        if(read_dir == NULL)
            break;
        
        if(strcmp(read_dir->d_name,".") == 0 || strcmp(read_dir->d_name,"..") == 0)
            continue;

		if(read_dir->d_type == 8)
            continue;
			
        if(read_dir->d_type == 4)
		{
            char src_path[300] = {0};
			char dest_path[300] = {0};
			snprintf(src_path, sizeof(src_path), "%s/%s", src_dir, read_dir->d_name);
			snprintf(dest_path, sizeof(dest_path), "%s/%s", dest_dir, read_dir->d_name);
			copy_special_dir(src_path, dest_path);
		}
    }
    closedir(dp);
    return 0;
}
#include"myhead.h"
extern thread_pool *pool;

void cp(char *argv1,char *argv2)
{
    pool = malloc(sizeof(thread_pool));
    if(pool == NULL)
    {
        perror("malloc pool failed!");
        return;
    }
    init_pool(pool,10);
	printf("throwing 10 tasks...\n");

    filename *file_name = malloc(sizeof(filename));
    if(file_name == NULL)
    {
        perror("malloc file_name failed!");
        return;
    }
    strcpy(file_name->src,argv1);
    strcpy(file_name->dest,argv2);
    printf("src %s\n",file_name->src);
    printf("dest %s\n",file_name->dest);
    struct stat src_info;
    struct stat dest_info;
    int flag = 1;
    if(stat(argv1,&src_info))
    {
        fprintf(stderr,":%s",argv1);
        perror("stat error");
        return;
    }
    if(stat(argv2,&dest_info))
    {
        flag = 0;
    }
      
    total_size = 0;
    complete_size = 0;
    get_size(file_name->src,&total_size);
    printf("totoal size: %lu\n",total_size);

    if(S_ISDIR(src_info.st_mode) && (flag == 0))
    {
//当参数一为目录文件时，参数二不存在时，是目录拷贝目录
	    add_task(pool, dir_copy, (void *)file_name);
    }
    else if(S_ISDIR(src_info.st_mode) && S_ISDIR(dest_info.st_mode))
    {
//当参数一为目录文件时，参数二为目录文件时，是目录拷贝目录
        unsigned long n = 0;
        get_size(file_name->dest,&n);
        if(n == total_size)
        {
            printf("this dir has been copied!\n");
            destroy_pool(pool);
            return;
        }
	    add_task(pool, dir_copy, (void *)file_name);
    }
    else if(S_ISDIR(src_info.st_mode) && S_ISREG(dest_info.st_mode))
    {
//当参数一为目录文件时，参数二为存在的普通文件，就报错
        printf("cp: cannot overwrite non-directory \'%s\' with directory \'%s\'",argv2,argv1);
        destroy_pool(pool);
        return;
    }
    else if(S_ISREG(src_info.st_mode) && flag == 0)
    {
// 当参数一为普通文件，参数二原本不存在时，为文件拷贝文件
	    add_task(pool, file_copy, (void *)file_name);
    }
    else if(S_ISREG(src_info.st_mode) && S_ISREG(dest_info.st_mode))
    {
//当参数一为普通文件，参数二为普通文件时，为文件拷贝文件
	    add_task(pool, file_copy, (void *)file_name);
    }
    else if(S_ISREG(src_info.st_mode) && S_ISDIR(dest_info.st_mode))
    {
//当参数一为普通文件，参数二存在，是目录文件时，为文件拷贝到目录
        printf("\ndest_name: %s\n",file_name->dest);
        snprintf(file_name->dest,sizeof(file_name->dest),"%s/%s",argv2,argv1);
        printf("\ndest_name: %s\n",file_name->dest);
        add_task(pool, file_copy, (void *)file_name);
    }
    while(complete_size < total_size)
    {
        printf("now waiting_task %d\n",pool->waiting_tasks);
        printf("now active_threads %d\n",pool->active_threads);
        printf("total size:%lu\n",total_size);
        printf("complete size:%lu\n",complete_size);
        sleep(5);
        if(pool->waiting_tasks > 2*pool->active_threads)
        {
	        printf("throwing 5 tasks...\n");
            add_thread(pool,5);
        }
    }
	destroy_pool(pool);	
}

void get_size(char *src_dir,unsigned long *len)
{   
    struct stat src_info;
    stat(src_dir,&src_info);
    if(S_ISREG(src_info.st_mode))
    {
        *len += src_info.st_size;
        return;
    }
    char src_path[4096] = {0};
    //打开源目录
    DIR *dp = opendir(src_dir);
    if(dp == NULL)
    {
        printf("opendir %s failed!\n",src_dir);
        pthread_exit(NULL);
    }
    struct dirent *read_dir;
    int n;
//循环读取该源目录下的文件信息
    while(1)
    {
        //判断源目录下的文件类型
        read_dir = readdir(dp);
        if(read_dir == NULL)
            break;
        //忽略.和..隐藏文件
        if(strcmp(read_dir->d_name,".")==0 || strncmp(read_dir->d_name,"..",2)==0)
            continue;
        bzero(src_path,sizeof(src_path));
        n = strlen(src_path);
        if(src_path[n-1] != '/')
            snprintf(src_path,sizeof(src_path),"%s/%s",src_dir,read_dir->d_name);
        else 
            snprintf(src_path,sizeof(src_path),"%s%s",src_dir,read_dir->d_name);
        if(read_dir->d_type == 4)
        {
            get_size(src_path,len);
        }
        else if(read_dir->d_type == 8)
        {
            struct stat info;
            stat(src_path,&info);
            *len += info.st_size;
        }
    }
    closedir(dp);
}

void *file_copy(void *arg)
{
    filename *file_name = (filename*)arg;
    // printf("%s file_copy tid [%lu]\n",file_name->src,pthread_self());
    FILE *fp1 = fopen(file_name->src,"r");
    if(fp1 == NULL)
    {
        sleep(3);
        printf("fopen src_file %s fail\n",file_name->src);
        sleep(5);
        return NULL;
    }
    // printf("%s file_copy tid [%lu]\n",file_name->dest,pthread_self());
    FILE *fp2 = fopen(file_name->dest,"w");
    if(fp2 == NULL)
    {
        sleep(3);
        printf("fopen dest_file %s fail",file_name->dest);
        sleep(5);
        return NULL;
    }
    char ch1,ch2;
    // printf("开始拷贝%s\n",file_name->src);
    while(1)
    {
        ch1 = fgetc(fp1);
        if(ch1 == EOF)
        {
            if(feof(fp1))
                break;
            if(ferror(fp1))
            {
        sleep(3);
                perror("=====fgetc error");
        sleep(5);
                return NULL;
            }
        }
        ch2 = fputc(ch1,fp2);
        if(ch2 == EOF)
        {
            if(ferror(fp2))
            {
        sleep(3);
                perror("======fputc error");
        sleep(5);
                return NULL;
            }
        }
        complete_size++;
    }
    // printf("tid [%lu]拷贝完成%s\n",pthread_self(),file_name->src);
    fclose(fp1);
    fclose(fp2);
}
//拷贝目录
void *dir_copy(void *arg)
{
    //文件名结构体
    filename *file_name = (filename*)arg;
    //创建目标目录
    int n = mkdir(file_name->dest,0777);
    // int n = mkdir_p(file_name->dest);
    if(n == -1)
    {
        printf("\nmkdir %s failed!\n",file_name->dest);
        sleep(5);
        return NULL;
    }
    // printf("\nmkdir dest %s\n",file_name->dest);
//打开源目录
    DIR *dp = opendir(file_name->src);
    if(dp == NULL)
    {
        printf("opendir %s failed!\n",file_name->src);
        sleep(5);
        return NULL;
    }

    struct dirent *read_dir;
    int len;
//循环读取该源目录下的文件信息
    while(1)
    {
        //判断源目录下的文件类型
        read_dir = readdir(dp);
        if(read_dir == NULL)
            break;
        //忽略.和..隐藏文件
        if(strcmp(read_dir->d_name,".")==0 || strncmp(read_dir->d_name,"..",2)==0)
            continue;
        filename *get_name = malloc(sizeof(filename));
        len = strlen(get_name->src);
        if(get_name->src[len-1] != '/')
            snprintf(get_name->src,sizeof(get_name->src),"%s/%s",file_name->src,read_dir->d_name);
        else 
            snprintf(get_name->src,sizeof(get_name->src),"%s%s",file_name->src,read_dir->d_name);
        n = strlen(get_name->dest);
        if(get_name->dest[len-1] != '/')
            snprintf(get_name->dest,sizeof(get_name->dest),"%s/%s",file_name->dest,read_dir->d_name);
        else
            snprintf(get_name->dest,sizeof(get_name->dest),"%s%s",file_name->dest,read_dir->d_name);
        //若是目录文件就进行递归目录拷贝
        if(read_dir->d_type == 4)
        {
            // dir_copy((void*)get_name);
// printf("\ntids[%lu] 进入目录 %s\n",pthread_self(),get_name->dest);
            // else
                add_task(pool,dir_copy,(void*)get_name);
        }
        //若是普通文件则进行文件拷贝
        else if(read_dir->d_type == 8)
        {
// printf("\ntids[%lu] 拷贝文件 %s 加入任务\n",pthread_self(),get_name->src);
            // add_task(pool,file_copy,(void*)get_name);
            file_copy((void*)get_name);
        }
    }
    closedir(dp);
    return NULL;
}

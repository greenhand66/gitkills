#include "thread_pool.h"
#include "mycp.h"
#define BUF_SIZE 10240
extern thread_pool *pool;
extern unsigned long total_size;
extern unsigned long complete_size;

static void ffflush()
{			//清空输入缓冲区函数
	char ch;
	while((ch=getchar())!='\n' && ch!=EOF)
	{
		//printf("(%x)",ch);
	}
}

void *process(void *arg)	//打印进度条
{ 
	double rate=0;
	char *str = "-\\|/";
	char *s = calloc(101,sizeof(char));
	int max = 0;
	while(1)
	{
		rate = (double)complete_size * 100 / total_size;
		printf("[%-101s][%.2f%%][%c]\r",s,rate,str[(int)rate%4]);
		// printf("%ld/%ld\n",complete_size,total_size);
		if((int)rate == 100)
			break;

		// s[(int)rate] = '#';
		for(int i=0;i<(int)rate;i++)
		{
			if(s[i] == '\0')
				s[i] = '#';
		}
		fflush(stdout);		//刷新缓冲区
		usleep(10000);
	}
	printf("\n");
	free(s);
	return NULL;
}

//计算大小
void CalcFileSize(char *srcDir)
{
	char srcPath[4096] = {0};
	//打开源目录
	DIR *dp = opendir(srcDir);
	if(dp == NULL)
	{
		printf("%s opendir() failed",srcDir);
		return;
	}
	//读取目录项
	struct dirent *ep = NULL;
	while(1)
	{
		ep = readdir(dp);
		if(ep == NULL)    //读完
			break;
		
		//排除隐藏文件
		if(strncmp(ep->d_name,".",1) == 0 || strcmp(ep->d_name,"..")==0)
			continue;
		bzero(srcPath,4096);
		sprintf(srcPath,"%s/%s",srcDir,ep->d_name);  
		if(ep->d_type == DT_DIR)    //如果是目录，则递归
		{
			CalcFileSize(srcPath);
		}
		else if(ep->d_type == DT_REG)	//如果是文件，则计算文件大小
		{
			struct stat statBuf;
			stat(srcPath,&statBuf);
			total_size += statBuf.st_size;
		}
	}
}

void MyCopy(file_name *fileName)
{
	unsigned lenSrc = strlen(fileName->src);
	unsigned lenDst = strlen(fileName->dst);
	char *tmp = fileName->src;
	if(tmp[lenSrc-1] == '/')
		tmp[lenSrc-1] = '\0';
	tmp = fileName->dst;
	if(tmp[lenDst-1] == '/')
		tmp[lenDst-1] = '\0';

	struct stat statBuf;
	if(stat(fileName->src,&statBuf) < 0)
	{
		perror("stat()");
		return ;
	}

	switch (statBuf.st_mode & S_IFMT) 
	{
		case S_IFBLK:  printf("block device\n");            break;
		case S_IFCHR:  printf("character device\n");        break;
		case S_IFDIR:  printf("directory\n"); add_task(pool,CopyDir,(void*)fileName);         break;
		case S_IFIFO:  printf("FIFO/pipe\n");               break;
		case S_IFLNK:  printf("symlink\n");                 break;
		case S_IFREG:  printf("regular file\n"); add_task(pool,CopyFile,(void*)fileName);      break;
		case S_IFSOCK: printf("socket\n");                  break;
		default:       printf("unknown?\n");                break;
    }	
}


void *CopyDir(void *arg)
{
	file_name *fileName = (file_name *)arg;

	if(mkdir(fileName->dst, 0777) < 0)
	{
		perror("mkdir()");
		return NULL;
	}

	DIR *dir = opendir(fileName->src);
	struct dirent *op = NULL;
	while(1)
	{
		op = readdir(dir);
		if(op == NULL)
			break;
		if(strcmp(op->d_name,".")==0||strcmp(op->d_name,"..")==0)
			continue;
		file_name *path = calloc(1,sizeof(file_name));
		snprintf(path->dst,sizeof(path->dst)*2,"%s/%s",fileName->dst,op->d_name);
		snprintf(path->src,sizeof(path->src)*2,"%s/%s",fileName->src,op->d_name);

		if(op->d_type == DT_DIR)	//目录
		{
			// printf("开始拷贝目录：%s ----> %s\n",fileName->src,fileName->dst );
			add_task(pool,CopyDir,(void*)path);
		}
		else if(op->d_type == DT_REG)	//普通文件
		{
			// printf("开始拷贝文件：%s ----> %s\n",fileName->src,fileName->dst );
			add_task(pool,CopyFile,(void*)path);
		}
	}
	closedir(dir);
	return NULL;
}


void* CopyFile(void *arg)
{
	file_name *fileName = (file_name *)arg;
	int sfd;
	int dfd;
	int len,ret,pos;
	
	sfd = open(fileName->src,O_RDONLY);
	if(sfd < 0)
	{
		perror("open");
		return NULL;
	}
	dfd= open(fileName->dst,O_WRONLY|O_CREAT,O_TRUNC,0777);
	if(dfd < 0)
	{
		close(sfd);
		perror("open");
		return NULL;
	}
	struct stat *statBuf = calloc(1,sizeof(struct stat));
	if(stat(fileName->src,statBuf) < 0)
	{
		perror("stat()");
		return NULL;
	}
	unsigned long fileSize = statBuf->st_size;	//获取文件大小
	unsigned long pos1;
	char *buf = calloc(BUF_SIZE,sizeof(char));

	while(1)
	{
		len = read(sfd,buf,BUF_SIZE);
		if(len < 0)
		{
			perror("read");
			break;
		}
		if(len == 0)
			break;

		pos = 0;
		while(len > 0)
		{
			// pos1 = lseek(dfd,0,SEEK_CUR);
			//printf("\r文件复制中:-----%.2lf%%-----",(double)pos1/(double)fileSize*100);
			ret = write(dfd,buf + pos,len);
			if(ret < 0)
			{
				perror("write");
				exit(1);
			}
			pos += ret;
			len -= ret;
		}
	}
	complete_size += fileSize;

	free(buf);
	free(statBuf);
	close(sfd);
	close(dfd);
}



#include "thread_pool.h"
#include "mycp.h"
thread_pool *pool;
unsigned long total_size;	//总文件大小
unsigned long complete_size;//实时copy文件大小

int main(int argc, char const *argv[])
{
	//初始化线程池，创建15条线程
	pool = malloc(sizeof(thread_pool));
	init_pool(pool, 10);

	if(argc!= 3)
	{
		printf("Usage: %s <src> <dst>\n",argv[0]);
		return 0;
	}
	file_name fileName;
	strcpy(fileName.src,argv[1]);
	strcpy(fileName.dst,argv[2]);
	//计算文件的总大小
	CalcFileSize((char *)&fileName.src);	
	// printf("total_size = %lu\n",total_size);
	add_task(pool,process,NULL);

	MyCopy(&fileName);

	//销毁线程池
	destroy_pool(pool);
	
	return 0;
}


#ifndef _MYHEAD_H_
#define _MYHEAD_H_

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/shm.h>
#include<semaphore.h>
#include<fcntl.h>
#include<string.h>

#define SHM_SIZE 1024   //共享内存大小
#define SEM_NAME "mysem"    //有名信号量的名字

#endif
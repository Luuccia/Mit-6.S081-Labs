#include "kernel/types.h"
#include "user/user.h"


void sieve(int* pd)
{
    //读端，关闭写
    close(pd[1]);
    int min_prime;
    if(read(pd[0], &min_prime, sizeof(int))==0)//若未读入数据，则已经筛选完毕
    {
        close(pd[0]);//关闭读端
        exit(0);
    }
    printf("prime %d\n", min_prime);
    int pd_new[2];//创建通向下一级的管道
    pipe(pd_new);

    if(fork()==0)
    {
        close(pd[0]);//子进程也有这个描述符表
        sieve(pd_new);
    }
    close(pd_new[0]);//相对下一级的写端。关闭通向下一级读端
    int buffer;
    while(read(pd[0], &buffer, sizeof(int)))
    {
        if((buffer % min_prime)!=0)
        {
            write(pd_new[1], &buffer, sizeof(int));
        }
    } //筛选质数
    close(pd[0]);//关闭上一级的读端
    close(pd_new[1]);//关闭通向下一级的写端

    wait((int*)0);
    exit(0);
}


int main()
{
    int status;
    int pd[2];
    pipe(pd);

    if(fork()==0)
    {
        sieve(pd);
    }
    close(pd[0]);//写端，关闭读，务必要在fork后关闭
    for(int i = 2;i <= 35;i++)
    {
        write(pd[1],&i,sizeof(int));
    }    
    close(pd[1]);
    wait(&status);

    exit(0);
}
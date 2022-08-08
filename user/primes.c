#include "kernel/types.h"
#include "user/user.h"


void generate_nums();
void send_primes(int pd[], int infos[], int infoslen);
void process(int pd[]);

int
main(int argc, char** argv){
    //声明管道
    int pd[2];
    //创建管道
    pipe(pd);
    int pid;

    //Child Process
    if((pid = fork()) == 0){
        process(pd);
        exit(0);
    }
    //Parent Process
    else{
        int nums[34];
        generate_nums(nums);
        send_primes(pd, nums, 34);
        exit(0);
    }
}

void process(int pd[]){
    int p;
    int n;
    int len;
    int pid;
    int pd_child[2];
    int infos[34];
    int infos_i = 0;
    pipe(pd_child);
    
    close(pd[1]);
    len = read(pd[0], &p, sizeof(p));
    printf("prime %d\n", p);  
    
    while(len != 0) {
        //从管道读端继续读取
        len = read(pd[0], &n, sizeof(n));
        if(n % p != 0){
            *(infos + infos_i) = n;
            infos_i++;
        }
        //若能被p整除，则不是质数，要从数组中剔除
    }

    close(pd[0]);
    //若infos[]为空，则质数已经筛选完毕
    if(infos_i == 0) {
        exit(0);
    }
    
    // Child Process
    if((pid = fork()) == 0){
        process(pd_child);
    }
    // Parent Process
    else{
        send_primes(pd_child, infos, infos_i);
    }
    exit(0);
}

void
generate_nums(int nums[34]){
    int i = 0;
    for(int count = 2; count <= 35; count++){
        nums[i] = count;
        i++;
    }
    
}

//将数组中的内容写入管道
void
send_primes(int pd[], int infos[], int infoslen){
    int info;
    close(pd[0]);
    for(int i = 0; i < infoslen; i++){
        info = infos[i];
        write(pd[1],&info,sizeof(info));
    }
}
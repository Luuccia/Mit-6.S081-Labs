#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

#define CMDSTYLE        2

char *cutoffinput(char *buf);
void substring(char s[], char sub[], int pos, int len);
/* 子串 */
void 
substring(char s[], char *sub, int pos, int len) {
   int c = 0;   
   while (c < len) {
      *(sub + c) = s[pos+c];
      c++;
   }
   *(sub + c) = '\0';
}

/* 截断 '\n' */
char* 
cutoffinput(char *buf){
    /* 记得要为char *新分配一片地址空间，否则编译器默认指向同一片地址 */
    if(strlen(buf) > 1 && buf[strlen(buf) - 1] == '\n'){
        char *subbuff = (char*)malloc(sizeof(char) * (strlen(buf) - 1));
        substring(buf, subbuff, 0, strlen(buf) - 1);
        return subbuff;
    }
    else
    {
        char *subbuff = (char*)malloc(sizeof(char) * strlen(buf));
        strcpy(subbuff, buf);
        return subbuff;
    }
}

int 
main(int argc, char *argv[])
{
    /* code */
    int pid;
    char buf[MAXPATH];
    char *args[MAXARG];
    char *cmd;
    /* 默认命令为echo */
    if(argc == 1){
        cmd = "echo";
    }
    else{
        cmd = argv[1];
    }
    /* 参数的计数器 */
    int args_num = 0;
    while (1)
    {
        memset(buf, 0, sizeof(buf));
        gets(buf, MAXPATH);
        char *arg = cutoffinput(buf);
        if(strlen(arg) == 0 || args_num >= MAXARG){
            break;
        }
        args[args_num]= arg;
        args_num++;
    }

    int argstartpos = 1;
    char *argv2exec[MAXARG];
    argv2exec[0] = cmd;

    for (; argstartpos + 1 < argc; argstartpos++)
    {
        argv2exec[argstartpos] = argv[argstartpos + 1];
    }
    
    for (int i = 0; i < args_num; i++)
    {
        argv2exec[i + argstartpos] = args[i];
    }
    argv2exec[args_num + argstartpos] = 0;
    
    /* 运行cmd */
    if((pid = fork()) == 0){   
        exec(cmd, argv2exec);
        exit(0); 
    }  
    else
    {
        wait(0);
    }
    exit(0);
}
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// Copy from Grep.c
char buf[1024];
int match(char*, char*);
int matchhere(char*, char*);
int matchstar(int, char*, char*);

int
match(char *re, char *text)
{
  if(re[0] == '^')
    return matchhere(re+1, text);
  do{  //即使字符串为空，也必须要检查
    if(matchhere(re, text))
      return 1;
  }while(*text++ != '\0');
  return 0;
}

//在text中的开头，查找(*re)
int matchhere(char *re, char *text)
{
  if(re[0] == '\0')
    return 1;
  if(re[1] == '*')
    return matchstar(re[0], re+2, text);
  if(re[0] == '$' && re[1] == '\0')
    return *text == '\0';
  if(*text!='\0' && (re[0]=='.' || re[0]==*text))
    return matchhere(re+1, text+1);
  return 0;
}

//在text中的开头，查找c*(*re)
int matchstar(int c, char *re, char *text)
{
  do{  //通配符* 匹配0个或多个实例
    if(matchhere(re, text))
      return 1;
  }while(*text!='\0' && (*text++ == c || c == '.'));
  return 0;
}


/*
  find.c
*/
char* fmtname(char *path);
void find(char *path, char *re);

int 
main(int argc, char** argv){
    if(argc < 2){
      printf("Parameters are not enough\n");
    }
    else{
      //在路径path下递归搜索文件 
      find(argv[1], argv[2]);
    }
    exit(0);
}

// 参考ls中的fmtname，去掉了空白字符串
char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // 找到指针，最后一个/后的第一个字符
  for(p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  if(strlen(p) >= DIRSIZ)
    return p;
  //设置一块内存区域，[1]内存首地址，[2]要设置的值，[3]这块内存的大小
  memset(buf, 0, sizeof(buf));
  //不考虑重叠内容的内存拷贝，[1]目标内存首地址，[2]源内存首地址，[3]拷贝字节数
  memmove(buf, p, strlen(p));
  return buf;
}

void 
find(char *path, char *re){
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;
  
  if((fd = open(path, 0)) < 0){
      fprintf(2, "find: cannot open %s\n", path);
      return;
  }

  //由已打开的文件描述词fd，取得文件状态复制到结构&st中
  //执行成功返回0，失败返回-1
  if(fstat(fd, &st) < 0){
      fprintf(2, "find: cannot stat %s\n", path);
      close(fd);
      return;
  }
  
  switch(st.type){
  case T_FILE:
      if(match(re, fmtname(path)))
          printf("%s\n", path);
      break;
      //如果是文件

  case T_DIR:
      if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)){
          printf("find: path too long\n");
          break;
      }
      strcpy(buf, path);
      p = buf + strlen(buf);
      *p++ = '/';
      while(read(fd, &de, sizeof(de)) == sizeof(de)){
          if(de.inum == 0)
              continue;
          memmove(p, de.name, DIRSIZ);
          p[DIRSIZ] = 0;
          if(stat(buf, &st) < 0){
              printf("find: cannot stat %s\n", buf);
              continue;
          }
          char* lstname = fmtname(buf);
          //"."：当前目录，".."：回到上一级目录
          if(strcmp(".", lstname) == 0 || strcmp("..", lstname) == 0){
            continue;
          }
          else{
            find(buf, re);
          }
      }
      break;
      //如果是文件夹，则递归查看
  }
  close(fd);
}

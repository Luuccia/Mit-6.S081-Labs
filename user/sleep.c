#include "kernel/types.h"
#include "user/user.h"

//sleep只需要传入一个参数，表示持续时间，以字符串形式储于argv[1]，dueation_pos为其位置
const int duration_pos = 1;
typedef enum {wrong_char, success_parse, toomany_char} cmd_parse;
cmd_parse parse_cmd(int argc, char** argv);

int 
main(int argc, char* argv[]){
    //argc=1，表示只传入了一个元素，即argv[0]，输入的程序路径和名称
    if(argc == 1){
        printf("Please enter the parameters!");
        exit(1);
    }
    else{
        cmd_parse parse_result;
        parse_result = parse_cmd(argc, argv);
        if(parse_result == toomany_char){
            printf("Too many args! \n");
            exit(1);
        }
        else if(parse_result == wrong_char){
            printf("Cannot input alphabet, number only \n");
            exit(1);
        }
        else{
            int duration = atoi(argv[duration_pos]);
            sleep(duration);
            exit(0);
        }
        
    }
}

cmd_parse
parse_cmd(int argc, char** argv){
    if(argc > 2){
        return toomany_char;
    }
    else {
        int i = 0;
        while (argv[duration_pos][i] != '\0')
        {
            //检测第一个参数是否为数字
            if(!('0' <= argv[duration_pos][i] && argv[duration_pos][i] <= '9')){
                return wrong_char;
            }
            i++;
        }
        
    }
    return success_parse;
}
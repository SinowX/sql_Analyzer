#include<iostream>
#include<string.h>
#include<string>
// #include<dirent.h>
#include<unistd.h>
#include<pwd.h>
#include<stack>
#include<queue>
#include"definition.h"
#include"function.h"
#include"yacc.h"
using namespace std;

int main(){
    int result = CurrentWorkDir();
    if(result==ERROR_SYSTEM){
        fprintf(stderr,"\e[31;1mError: System error while getting current work directory.\n\e[0m");
        exit(ERROR_SYSTEM);
    }
    GetUsername();
    GetHostname();

    char argv[BUF_SZ];
    while(true)
    {
        printf("\e[32;1m[%s@%s]\e[0m$ ",username,hostname);
        fgets(argv,BUF_SZ,stdin);
        int len= strlen(argv);
        // std::cout<<len<<std::endl;
        if(len!=BUF_SZ)
        {
            argv[len-1]='\0';
        }
        result=stoLower(argv);
        if(result==ERROR_INVALIDCHAR)
        {
            fprintf(stderr,"Invalid Character\n");
            continue;
        }
        Lexical(argv);
        trSegNode();
        freeSegNode();
        // printf("%s\n",argv);
        // printf("%d\n",strlen(argv));
    }

    exit(RESULT_NORMAL);
}

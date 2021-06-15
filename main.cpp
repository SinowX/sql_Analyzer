#include<iostream>
#include<string.h>
#include<string>
// #include<dirent.h>
#include<unistd.h>
#include<pwd.h>
#include<stack>
#include<queue>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<signal.h>
#include"definition.h"
#include"function.h"
#include"yacc.h"
// using namespace std;

int Curfd=-1;

void secureINTR(int sig)
{
    printf("Client Caught INTR\n");
    if(Curfd>=0)
        close(Curfd);
    exit(0);
}

void registerSigHandler(struct sigaction *act)
{
    act->sa_handler=secureINTR;

    if(sigaction(SIGINT,act,NULL)==-1)
    {
        printf("ERROR SIGACTION: %s\n",strerror(errno));
        exit(0);
    }
}

int connectServer()
{
    int connect_fd;
    struct sockaddr_in server_addr;
    if((connect_fd = socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        printf("ERROR: %s\n",strerror(errno));
        exit(-1);
    }    
    Curfd=connect_fd;

    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port=htons(SERVER_PORT);
    server_addr.sin_addr.s_addr=inet_addr(SERVER_ADDR);
    
    if(connect(connect_fd,(struct sockaddr*)&server_addr,sizeof(server_addr))==-1)
    {
        printf("ERROR CONNECT: %s\n",strerror(errno));
        exit(-1);
    }
    else{
        printf("CONNECT SUCCESS\n");
    }

    return connect_fd;
}



int main(){
    struct sigaction act;
    registerSigHandler(&act);

    int connect_fd=connectServer();

    char *buff=(char *)malloc(8192*sizeof(char));


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
        // freeSegNode();
        // printf("%s\n",argv);
        // printf("%d\n",strlen(argv));
        if(Yacc()!=RESULT_NORMAL)
            printf("Error\n");
        else
        {
            if(write(connect_fd,&the_pack,sizeof(the_pack))==-1)
            {
                fprintf(stderr,"Send Cmd Failed\n");
            }

            if(read(connect_fd,buff,8192)==-1)
            {
                
                fprintf(stderr,"Receive Message Failed\n");
            }
            else
            {
                // printf("received feedback\n");
                fprintf(stdout,"%s\n",buff);
            }
            
            // printf("Action: %s\n",the_pack.act);
            // printf("PreObj: %s\n",the_pack.preobj);
            // printf("Object: %s\n",the_pack.obj);
            // printf("Exec: %s\n",the_pack.exec);
            // printf("Condi: %s\n",the_pack.condi);
            // printf("Complete.\n");
        }
}

    exit(RESULT_NORMAL);
}

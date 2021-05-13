#include<iostream>
#include<string.h>
#include<string>
// #include<dirent.h>
#include<unistd.h>
#include<pwd.h>
#include<stack>

// #include<stdio.h>
using namespace std;

#define BUF_SZ 256

const char* ACTION_CREATE="create";
const char* ACTION_SHOW="show";
const char* ACTION_INSERT="insert";
const char* ACTION_UPDATE="update";
const char* ACTION_DROP="drop";

const char* PREOBJ_TABLE="table";
const char* PREOBJ_COLUMNS="columns";
const char* PREOBJ_INDEX="index";
const char* PREOBJ_STATUS="status";
const char* PREOBJ_INTO="into";

const char* CONDI_WHERE="where";
const char* CONDI_FROM="from";
// const char* CONDI_EQUAL="=";

const char* EXEC_SET="set";
const char* EXEC_VALUES="values";
// const char* EXEC_EQUAL="=";

const char SYM_EQUAL='=';
const char SYM_SPACE=' ';
const char SYM_COMMA=',';
const char SYM_OPEN_PAREN='(';
const char SYM_CLOSE_PAREN=')';
// const char SYM_COLON=':';
const char SYM_SEMICOLON=';';


enum{
    RESULT_NORMAL,
    ERROR_INVALIDCHAR,
    ERROR_SYSTEM,
    ERROR_MISSING_PARAMETER,
    ERROR_TOO_MANY_PARAMETER,
    ERROR_WRONG_PARAMETER,
    ERROR_OTHER
};


typedef struct cmd_pack{
    char *act;
    char *obj;
    char *exec;
    char *condi;
}cmd_pack;

typedef struct seg_node{
    char segment[BUF_SZ];
    struct seg_node * next;
}seg_node;

char username[BUF_SZ];
char hostname[BUF_SZ];
char cwd[BUF_SZ];
seg_node *seg_header;

int CurrentWorkDir();
void GetUsername();
void GetHostname();
cmd_pack * init_pack(int len_act, int len_obj, int len_exec, int len_condi);
char ctoLower(char c);
int stoLower(char *str);
void Lexical(char *cmd);
seg_node* allocSegNode(seg_node *p);
void freeSegNode();
void trSegNode();


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
        // printf("%s\n",argv);
        // printf("%d\n",strlen(argv));
    }

    exit(RESULT_NORMAL);
}

int CurrentWorkDir(){
    char *result = getcwd(cwd,BUF_SZ);
    if(result==NULL){
        return ERROR_SYSTEM;
    }else
        return RESULT_NORMAL;
}

void GetUsername(){
    struct passwd* pwd=getpwuid(getuid());
    strcpy(username,pwd->pw_name);
}

void GetHostname(){
    gethostname(hostname,BUF_SZ);
}

cmd_pack * init_pack(int len_act, int len_obj, int len_exec, int len_condi);
char ctoLower(char c){
    if(c>=65&&c<91)
    {
        c+=32;
    }
    else if(c>=97&&c<123)
    {}
    else if(c==SYM_SPACE||c==SYM_SEMICOLON||
    c==SYM_COMMA||c==SYM_EQUAL||c==SYM_OPEN_PAREN||
    c==SYM_CLOSE_PAREN)
    {}
    else
    {
        c=ERROR_INVALIDCHAR;
    }
    return c;
}
int stoLower(char *str){
    int res=0;
    for(int i=0;i<strlen(str);i++){
        res=ctoLower(str[i]);
        if(res==ERROR_INVALIDCHAR){
            return ERROR_INVALIDCHAR;
        }else{
            str[i]=res;
        }
    }
    return RESULT_NORMAL;
}

void Lexical(char *cmd){
    seg_header=(seg_node*)malloc(sizeof(seg_node));
    seg_header->next=NULL;
    seg_node * p;
    p=seg_header;
    int last_start=0;
    int if_space=1;
    for(int i=0;i<=strlen(cmd);i++){
        if(cmd[i]==' '||cmd[i]=='\0'){
            if(if_space==1){
                last_start=i+1;
                continue;
            }else{
                for(int j=0;j<i-last_start;j++){
                    p->segment[j]=cmd[j+last_start];
                }
                p->segment[i-last_start]='\0';
                p=allocSegNode(p);
                last_start=i+1;
                if_space=1;
            }
        }else if(cmd[i]==SYM_EQUAL||cmd[i]==SYM_SPACE||
        cmd[i]==SYM_COMMA||cmd[i]==SYM_OPEN_PAREN||
        cmd[i]==SYM_CLOSE_PAREN||cmd[i]==SYM_SEMICOLON)
        {
            if(if_space==1){
                p->segment[0]=cmd[i];
                p->segment[1]='\0';
                p=allocSegNode(p);
                last_start=i+1;
                if_space=1;           
            }else{
                for(int j=0;j<i-last_start;j++){
                    p->segment[j]=cmd[j+last_start];
                }
                p->segment[i-last_start]='\0';
                p=allocSegNode(p);
                p->segment[0]=cmd[i];
                p->segment[1]='\0';
                p=allocSegNode(p);
                last_start=i+1;
                if_space=1; 
            }

        }
        else
        {
            if_space=0;
            continue;
        }
    }
}

seg_node* allocSegNode(seg_node *p)
{
    seg_node* tmp=(seg_node*)malloc(sizeof(seg_node));
    p->next=tmp;
    tmp->next=NULL;
    return tmp;
}
void freeSegNode(){
    stack<seg_node*> route;
    seg_node *p,*q;
    p=seg_header;
    while(p->next!=NULL){
        route.push(p);
        p=p->next;
    }
    while(!route.empty()){
        p=route.top();
        free(p->next);
    }
}
void trSegNode(){
    seg_node *p;
    p=seg_header;
    while(p->next!=NULL){
        printf("%s\n",p->segment);
        p=p->next;

    }
}
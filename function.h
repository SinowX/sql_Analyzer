#include"definition.h"
// #include<
#include<string.h>
#include<unistd.h>
#include<stack>
#include<queue>
#include<pwd.h>
#include<stdlib.h>
#include<stdio.h>

using namespace std;



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

// cmd_pack * init_pack(int len_act, int len_obj, int len_exec, int len_condi);

void set_pack(char *act,char *obj,char *exec,char *condi)
{
    strcpy(the_pack.act,act);
    strcpy(the_pack.obj,obj);
    strcpy(the_pack.exec,exec);
    strcpy(the_pack.condi,condi);
}


char ctoLower(char c){
    if(c>=65&&c<91)
    {
        c+=32;
    }
    else if((c>=97&&c<123)||c>=48&&c<58)
    {}
    else if(c==SYM_SPACE[0]||c==SYM_SEMICOLON[0]||
    c==SYM_COMMA[0]||c==SYM_EQUAL[0]||c==SYM_OPEN_PAREN[0]||
    c==SYM_CLOSE_PAREN[0])
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
        }else if(cmd[i]==SYM_EQUAL[0]||cmd[i]==SYM_SPACE[0]||
        cmd[i]==SYM_COMMA[0]||cmd[i]==SYM_OPEN_PAREN[0]||
        cmd[i]==SYM_CLOSE_PAREN[0]||cmd[i]==SYM_SEMICOLON[0])
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
        // cout<<"Now Free "<<p->segment<<endl;
        free(p->next);
        route.pop();
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


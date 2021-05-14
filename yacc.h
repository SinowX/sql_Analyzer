#include"definition.h"
// #include<
#include<string.h>
#include<unistd.h>
#include<stack>
#include<queue>
#include<pwd.h>
#include<stdlib.h>
#include<stdio.h>
#include"definition.h"
using namespace std;

int handle_act(char *action, seg_node *p)
{
    if(p->segment==ACTION_CREATE||p->segment==ACTION_SHOW||
    p->segment==ACTION_INSERT||p->segment==ACTION_UPDATE||
    p->segment==ACTION_DROP)
    {
        strcpy(action,p->segment);
        return RESULT_NORMAL;
    }
    else
        return ERROR_WRONG_PARAMETER;
}

int handle_preobj(char *action,char *preobj, seg_node *p)
{
    if(
        (action==ACTION_CREATE&&p->segment==PREOBJ_TABLE)||
        (action==ACTION_SHOW&&p->segment==PREOBJ_TABLES)||
        (action==ACTION_SHOW&&p->segment==PREOBJ_COLUMNS)||
        (action==ACTION_SHOW&&p->segment==PREOBJ_INDEX)||
        (action==ACTION_SHOW&&p->segment==PREOBJ_STATUS)||
        (action==ACTION_INSERT&&p->segment==PREOBJ_INTO)||
        (action==ACTION_UPDATE&&p->segment==PREOBJ_TABLE)||
        (action==ACTION_DROP&&p->segment==PREOBJ_TABLE)||
        (action==ACTION_DROP)&&p->segment==PREOBJ_ROW
    )
    {
        strcpy(preobj,p->segment);
        return RESULT_NORMAL;
    }
    else
        return ERROR_WRONG_PARAMETER;
}

int handle_obj(char *object ,seg_node *p)
{

    strcpy(object,p->segment);
    return RESULT_NORMAL;
    // if(
    //     action==ACTION_CREATE||
    //     action==ACTION_SHOW||
    //     action==ACTION_INSERT||
    //     action==ACTION_UPDATE||
    //     action==ACTION_DROP
    // )
    // {
    //     strcpy(object,p->segment);
    //     return RESULT_NORMAL;
    // }
    // else
    //     return ERROR_WRONG_PARAMETER;
}

int handle_exec(char *action,char *exec, seg_node **p)
{
    queue<char [BUF_SZ]> keyq,valueq;
    char buff[BUF_SZ]="-1";
    if(action==ACTION_CREATE)
    {
        // (	[$columnname	$attributes]	)

        if((*p)->segment==SYM_OPEN_PAREN)
        {
            while(
                (*p)->next!=NULL&&
                (*p)->next->segment!=SYM_COMMA&&
                (*p)->next->segment!=SYM_CLOSE_PAREN
            )
            {
                // if((*p)->next==NULL)
                //     return ERROR_WRONG_PARAMETER;
                (*p)=(*p)->next;
                keyq.push((*p)->segment);
                while(
                    (*p)->next!=NULL&&
                    // (*p)->next->segment!=""&&
                    (*p)->segment!=SYM_COMMA&&
                    (*p)->segment!=SYM_CLOSE_PAREN
                )
                {
                    // if((*p)->next==NULL)
                    //     return ERROR_WRONG_PARAMETER;
                    if(buff=="-1")
                        strcpy(buff,""); //清零
                    (*p)=(*p)->next;
                    strcat(buff,(*p)->segment);
                    strcat(buff,":");
                }
                if(buff!="-1") // 指令中有 attributes 
                    valueq.push(buff);
                
                strcpy(buff,"-1");
            } // 每个 column attributes 至少会有一个字段指明类型
            
            if(keyq.size()==0)
                return ERROR_WRONG_PARAMETER;

            if(keyq.size()==valueq.size())
            {
                while(!keyq.empty())
                {
                    strcat(exec,keyq.front());
                    strcat(exec,"=");
                    strcat(exec,valueq.front());
                    strcat(exec,";");
                    keyq.pop();
                    valueq.pop();
                }
                return RESULT_NORMAL;
            }
            else
                return(ERROR_KEY_VALUE);
        }
        else
            return ERROR_WRONG_PARAMETER;

    }
    else if(action==ACTION_INSERT)
    {
        // (	[$columnname]	)		values	(	[$columnvalue]	)

        if((*p)->segment==SYM_OPEN_PAREN)
        {
            while(
                (*p)->next!=NULL&&
                // (*p)->next->segment!=SYM_COMMA&&
                (*p)->next->segment!=SYM_CLOSE_PAREN
            )
            {
                (*p)=(*p)->next;
                if((*p)->segment!=SYM_COMMA)
                    continue;
                keyq.push((*p)->segment);
            }
            if((*p)->next==NULL||(*p)->segment!=EXEC_VALUES)
                return ERROR_WRONG_PARAMETER;
            
            (*p)=(*p)->next;

            while(
                (*p)->next!=NULL&&
                // (*p)->next->segment!=SYM_COMMA&&
                (*p)->next->segment!=SYM_CLOSE_PAREN
            )
            {
                (*p)=(*p)->next;
                if((*p)->segment!=SYM_COMMA)
                    continue;
                valueq.push((*p)->segment);
            }

            if(keyq.size()==0)
                return ERROR_WRONG_PARAMETER;

            if(keyq.size()==valueq.size())
            {
                while(!keyq.empty())
                {
                    strcat(exec,keyq.front());
                    strcat(exec,"=");
                    strcat(exec,valueq.front());
                    strcat(exec,";");
                    keyq.pop();
                    valueq.pop();
                }
                return RESULT_NORMAL;
            }
            else
                return(ERROR_KEY_VALUE);
        }
        else
            return ERROR_WRONG_PARAMETER;
    }
    else if(action==ACTION_UPDATE)
    {
        if((*p)->segment==EXEC_SET)
        {
            if(
                (*p)->next!=NULL&&
                (*p)->next->segment!=SYM_COMMA&&
                (*p)->next->segment!=SYM_SEMICOLON
            )
            {
                (*p)=(*p)->next;
                strcat(exec,(*p)->segment);
                
                if(
                (*p)->next!=NULL&&
                (*p)->next->segment==EXEC_VALUES
                )
                    (*p)=(*p)->next;
                else
                    return ERROR_WRONG_PARAMETER;

                if(
                    (*p)->next!=NULL&&
                    (*p)->next->segment!=SYM_COMMA&&
                    (*p)->next->segment!=SYM_SEMICOLON
                )
                {
                    (*p)=(*p)->next;
                    strcat(exec,"=");
                    strcat(exec,(*p)->segment);
                    return RESULT_NORMAL;
                }
                else
                {
                    return ERROR_WRONG_PARAMETER;
                }
            }
            else
                return ERROR_WRONG_PARAMETER;
        }
        //set	[$columnname]	=	[$columnvalue]	
    }
}

int handle_condi(char *action, char *condi, seg_node** p)
{
    if((action==ACTION_SHOW||action==ACTION_DROP)&&(*p)->segment==CONDI_FROM)
    {
        if(
            (*p)->next!=NULL&&
            (*p)->next->segment!=SYM_SEMICOLON
        )
        {
            (*p)=(*p)->next;
            strcat(condi,(*p)->segment);
            return RESULT_NORMAL;
        }
    }
    else if(action==ACTION_UPDATE&&(*p)->segment==CONDI_WHERE)
    {
        if(
            (*p)->next!=NULL&&
            (*p)->next->segment!=SYM_SEMICOLON
        )
        {
            (*p)=(*p)->next;
            if(
                (*p)->next!=NULL&&
                (*p)->next->segment!=SYM_SEMICOLON
            )
            {
                (*p)=(*p)->next;
                strcat(condi,(*p)->segment);
                
                if(
                    (*p)->next!=NULL&&
                    (*p)->next->segment!=SYM_EQUAL
                )
                {
                    (*p)=(*p)->next;
                    strcat(condi,(*p)->segment);
                    
                    if(
                        (*p)->next!=NULL&&
                        (*p)->next->segment!=SYM_SEMICOLON
                    )
                    {
                        (*p)=(*p)->next;
                        strcat(condi,(*p)->segment);
                        return RESULT_NORMAL;
                    }
                    else
                        return ERROR_WRONG_PARAMETER;
                        
                }
                else
                    return ERROR_WRONG_PARAMETER;

            }
            else
                return ERROR_WRONG_PARAMETER;
        }
        else
            return ERROR_WRONG_PARAMETER;
    }
}

int Yacc(){
    queue<seg_node *> route;
    char Action[BUF_SZ]="";
    char PreObject[BUF_SZ]="";
    char Object[BUF_SZ]="";
    char Exec[BUF_SZ]="";
    char Condition[BUF_SZ]=""; 

    seg_node *p;
    p=seg_header;


    // stage Action
    if(p->segment=="")
        return(ERROR_TOO_FEW_PARAMETER);

    if(handle_act(Action,p)==ERROR_WRONG_PARAMETER)
        return(ERROR_WRONG_PARAMETER);
    
    // stage PreObject
    if(p->next==NULL||p->next->segment=="")
        return(ERROR_TOO_FEW_PARAMETER);
    else
        p=p->next;

    if(handle_preobj(Action,PreObject,p)==ERROR_WRONG_PARAMETER)
        return(ERROR_WRONG_PARAMETER);

    // stage Object

    if(Action!=ACTION_SHOW)
    {
        if(p->next==NULL||p->next->segment=="")
            return(ERROR_TOO_FEW_PARAMETER);
        else
            p=p->next;

        if(handle_obj(Object,p)==ERROR_WRONG_PARAMETER)
            return(ERROR_WRONG_PARAMETER);
    }

    // stage Exec

    if(Action!=ACTION_SHOW&&Action!=ACTION_DROP)
    {
        if(p->next==NULL||p->next->segment=="")
            return(ERROR_TOO_FEW_PARAMETER);
        else
            p=p->next;

        if(handle_exec(Action,Exec,&p)!=RESULT_NORMAL)
            return ERROR_WRONG_PARAMETER;
    }

    // stage condition


    if(
        (Action==ACTION_SHOW&&PreObject!=PREOBJ_TABLES)||
        Action==ACTION_UPDATE||
        (Action==ACTION_DROP&&PreObject==PREOBJ_ROW)
    )
    {
        if(p->next==NULL||p->next->segment=="")
            return(ERROR_TOO_FEW_PARAMETER);
        else
            p=p->next;
        
        if(handle_condi(Action,Condition,&p)!=RESULT_NORMAL)
            return ERROR_WRONG_PARAMETER;
    }

    // stage end

    if(p->next==NULL||p->next->segment==SYM_SEMICOLON)
        set_pack(Action,Object,Exec,Condition);
    else
        return ERROR_TOO_MANY_PARAMETER;
    
    return RESULT_NORMAL;
    


    // if (p->segment==ACTION_CREATE){
    //     strcpy(Action,p->segment);
    //     p=p->next;
    //     if(p->segment==PREOBJ_TABLE){
    //         strcpy(PreObject,p->segment);
    //         p=p->next;
    //         strcpy(Object,p->segment);
    //         p=p->next;
    //         if(p->segment==SYM_OPEN_PAREN)
    //         {
    //             while(p->segment!=SYM_CLOSE_PAREN)
    //             {
    //                 route.push(p);
    //                 p=p->next;
    //                 // route.push(p);
    //             }
    //             route.push(p);
    //             while(!route.empty())
    //             {
    //                 p=route.front();
    //                 strcat(Exec,p->segment);
    //                 // sprintf(Exec,"%s%s",Exec,p->segment);
    //                 route.pop();
    //             }
    //             if(p->next==NULL || p->next->segment==SYM_SEMICOLON)
    //             {
    //                 set_pack(Action,Object,Exec,Condition);
    //                 return RESULT_NORMAL;
    //             }
    //             else
    //                 return ERROR_TOO_MANY_PARAMETER;
    //         }
    //         else
    //             return ERROR_WRONG_PARAMETER;
    //         // 处理 column 对应 attributes
    //     }else
    //         return ERROR_WRONG_PARAMETER;
    // }
    // else if(p->segment==ACTION_SHOW){
    //     strcpy(Action,p->segment);
    //     p=p->next;
    //     if(p->segment==PREOBJ_TABLES){
    //         strcpy(PreObject,PREOBJ_TABLES);
    //         if(p->next==NULL || p->next->segment==SYM_SEMICOLON)
    //         {
    //             set_pack(Action,Object,Exec,Condition);
    //             return RESULT_NORMAL;
    //         }
    //     }
    //     else if(p->segment==PREOBJ_COLUMNS){
    //         strcpy(PreObject,p->segment);
    //         p=p->next;
    //         if(p->segment==CONDI_FROM)
    //         {
    //             strcpy(Condition,p->segment);
    //             p=p->next;
    //             if(p->segment==SYM_SPACE||p->segment==SYM_SEMICOLON||
    //             p->segment==SYM_COMMA||p->segment==SYM_EQUAL||
    //             p->segment==SYM_OPEN_PAREN||p->segment==SYM_CLOSE_PAREN)
    //                 return ERROR_WRONG_PARAMETER;
    //             else
    //             {
    //                 strcat(Condition,":");
    //                 strcat(Condition,p->segment);
    //                 if(p->next==NULL || p->next->segment==SYM_SEMICOLON)
    //                 {
    //                     set_pack(Action,Object,Exec,Condition);
    //                     return RESULT_NORMAL;
    //                 }
    //             }
    //         }
    //         else
    //             return ERROR_WRONG_PARAMETER;
    //     }
    //     else if(p->segment==PREOBJ_INDEX){
    //         strcpy(PreObject,p->segment);
    //         p=p->next;
    //         if(p->segment==CONDI_FROM)
    //         {
    //             strcpy(Condition,p->segment);
    //             p=p->next;
    //             if(p->segment==SYM_SPACE||p->segment==SYM_SEMICOLON||
    //             p->segment==SYM_COMMA||p->segment==SYM_EQUAL||
    //             p->segment==SYM_OPEN_PAREN||p->segment==SYM_CLOSE_PAREN)
    //                 return ERROR_WRONG_PARAMETER;
    //             else
    //             {
    //                 strcat(Condition,":");
    //                 strcat(Condition,p->segment);
    //                 if(p->next==NULL || p->next->segment==SYM_SEMICOLON)
    //                 {
    //                     set_pack(Action,Object,Exec,Condition);
    //                     return RESULT_NORMAL;
    //                 }
    //             }
    //         }
    //         else
    //             return ERROR_WRONG_PARAMETER;
    //     }
    //     else if(p->segment==PREOBJ_STATUS){
    //         strcpy(PreObject,p->segment);
    //         p=p->next;
    //         if(p->segment==CONDI_FROM)
    //         {
    //             strcpy(Condition,p->segment);
    //             p=p->next;
    //             if(p->segment==SYM_SPACE||p->segment==SYM_SEMICOLON||
    //             p->segment==SYM_COMMA||p->segment==SYM_EQUAL||
    //             p->segment==SYM_OPEN_PAREN||p->segment==SYM_CLOSE_PAREN)
    //                 return ERROR_WRONG_PARAMETER;
    //             else
    //             {
    //                 strcat(Condition,":");
    //                 strcat(Condition,p->segment);
    //                 if(p->next==NULL || p->next->segment==SYM_SEMICOLON)
    //                 {
    //                     set_pack(Action,Object,Exec,Condition);
    //                     return RESULT_NORMAL;
    //                 }
    //             }
    //         }
    //         else
    //             return ERROR_WRONG_PARAMETER;
    //     }
    //     else{
    //         return ERROR_WRONG_PARAMETER;
    //     }
    // }
    // else if(p->segment==ACTION_INSERT){
    //     strcpy(Action,p->segment);
    //     p=p->next;
    //     if (p->segment==PREOBJ_INTO){
    //         strcpy(PreObject,p->segment);
    //         p=p->next;
    //         strcpy(Object,p->segment);  // table name
    //         p=p->next;
    //         if(p->segment==SYM_OPEN_PAREN)
    //         {
    //             while(p->segment!=SYM_CLOSE_PAREN)
    //             {
    //                 route.push(p);
    //                 p=p->next;
    //             }
    //             route.push(p);
    //             while(!route.empty())
    //             {
    //                 p=route.front();
    //                 strcat(Exec,p->segment);
    //                 route.pop();
    //             }
    //             p=p->next;
    //             if(p->segment==EXEC_VALUES)
    //             {
    //                 strcat(Exec,p->segment); // values
    //                 p=p->next;
    //                 if(p->segment==SYM_OPEN_PAREN)
    //                 {
    //                     while(p->segment!=SYM_CLOSE_PAREN)
    //                     {
    //                         route.push(p);
    //                         p=p->next;
    //                     }
    //                     route.push(p);
    //                     while(!route.empty())
    //                     {
    //                         p=route.front();
    //                         strcat(Exec,p->segment);
    //                         route.pop();
    //                     }
    //                     if(p->next==NULL || p->next->segment==SYM_SEMICOLON)
    //                     {
    //                         set_pack(Action,Object,Exec,Condition);
    //                         return RESULT_NORMAL;
    //                     }
    //                     else
    //                         return ERROR_TOO_MANY_PARAMETER;
    //                 }
    //             }
    //         }
    //         else
    //             return ERROR_WRONG_PARAMETER;
    //     }
    //     else
    //         return ERROR_WRONG_PARAMETER;
    // }
    // else if(p->segment==ACTION_UPDATE){
    //     strcpy(Action,p->segment);
    //     p=p->next;    
    //     strcpy(Object,p->segment); //table name
    //     p=p->next;
    //     if(p->segment==EXEC_SET)
    //     {
    //         strcpy(Exec,p->segment); // set
    //         p=p->next;
    //         strcat(Exec,":");
    //         strcat(Exec,p->segment); // column name
    //         p=p->next;
    //         strcat(Exec,":");
    //         if(p->segment==SYM_EQUAL)
    //         {
    //             strcat(Exec,p->segment); // =
    //             p=p->next;
    //             strcat(Exec,":");
    //             strcat(Exec,p->segment); // column value
    //             p=p->next;
    //             if(p->segment==CONDI_WHERE)
    //             {
    //                 strcpy(Exec,p->segment); // where
    //                 p=p->next;
    //                 strcat(Exec,":");
    //                 strcat(Exec,p->segment); // column name
    //                 p=p->next;
    //                 strcat(Exec,":");
    //                 if(p->segment==SYM_EQUAL)
    //                 {
    //                     strcat(Exec,p->segment); // =
    //                     p=p->next;
    //                     strcat(Exec,":");
    //                     strcat(Exec,p->segment); // column value
    //                     if(p->next==NULL || p->next->segment==SYM_SEMICOLON)
    //                     {
    //                         set_pack(Action,Object,Exec,Condition);
    //                         return RESULT_NORMAL;
    //                     }
    //                     else
    //                         return ERROR_TOO_MANY_PARAMETER;
    //                 }
    //             }
    //             else
    //                 return ERROR_WRONG_PARAMETER;
    //         }
    //         else
    //             return ERROR_WRONG_PARAMETER;
    //     }
    // }
    // else if(p->segment==ACTION_DROP){
    //     strcpy(Action,p->segment);
    //     p=p->next;
    //     if(p->segment==PREOBJ_TABLE){
    //         strcpy(PreObject,p->segment);
    //         p=p->next;
    //         strcpy(Exec,p->segment); // shoud be table name
    //         if(p->next==NULL||p->next->segment==SYM_SEMICOLON)
    //             {
    //                 set_pack(Action,Object,Exec,Condition);
    //                 return RESULT_NORMAL;
    //             }
    //             else
    //                 return ERROR_TOO_MANY_PARAMETER;
    //     }
    //     else
    //     {
    //         strcpy(Exec,p->segment);
    //         p=p->next;
    //         if(p->segment==CONDI_FROM)
    //         {
    //             strcpy(Condition,p->segment);
    //             strcat(Condition,":");
    //             p=p->next;
    //             strcat(Condition,p->segment);
    //             if(p->next==NULL||p->next->segment==SYM_SEMICOLON)
    //             {
    //                 set_pack(Action,Object,Exec,Condition);
    //                 return RESULT_NORMAL;
    //             }
    //             else
    //                 return ERROR_TOO_MANY_PARAMETER;
    //         }
    //     }
    // }
    // else
    // {
    //     return ERROR_WRONG_PARAMETER;
    // }

}

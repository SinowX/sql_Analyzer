#include"definition.h"
// #include<
#include<string.h>
#include<unistd.h>
// #include<stack>
#include<queue>
#include<pwd.h>
#include<stdlib.h>
#include<stdio.h>
// #include"definition.h"
// using namespace std;

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
    std::queue<char [BUF_SZ]> keyq,valueq;
    char buff[BUF_SZ]="-1";
    // 0 wait for segment, 1 wait for ';'
    int switcher=0;
    if(action==ACTION_CREATE)
    {
        // (	[$columnname	$attributes]	)

        if((*p)->segment==SYM_OPEN_PAREN)
        {
            while(
                (*p)->next!=NULL&&
                (*p)->next->segment!=""&&
                (*p)->next->segment!=SYM_CLOSE_PAREN
            )
            {
                if(
                    (*p)->next->segment==SYM_SEMICOLON||
                    (*p)->next->segment==SYM_OPEN_PAREN||
                    (*p)->next->segment==SYM_EQUAL
                    )
                    return ERROR_WRONG_PARAMETER;

                (*p)=(*p)->next;
                //一个 column name
                keyq.push((*p)->segment);
                while(
                    (*p)->next!=NULL&&
                    // (*p)->next->segment!=""&&
                    (*p)->next->segment!=SYM_COMMA&&
                    (*p)->next->segment!=SYM_CLOSE_PAREN
                )
                {

                    if(
                        (*p)->next->segment==SYM_SEMICOLON||
                        (*p)->next->segment==SYM_EQUAL
                        )
                        return ERROR_WRONG_PARAMETER;

                    // if((*p)->next==NULL)
                    //     return ERROR_WRONG_PARAMETER;
                    //清空 buffer
                    if(buff=="-1")
                        strcpy(buff,""); 
                    (*p)=(*p)->next;
                    strcat(buff,(*p)->segment);
                    strcat(buff,":");
                }
                if(buff!="-1") // 指令中有 attributes 
                    valueq.push(buff);
                else //若不存在 attributes 则错误
                    return ERROR_WRONG_PARAMETER;
                
                strcpy(buff,"-1");
            } // 每个 column attributes 至少会有一个字段用于指明类型
            
            //如果没有 列 则错误
            if(keyq.size()==0)
                return ERROR_WRONG_PARAMETER;

            //如果 列的数量 与 列属性的数量 不对应，则错误
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
                (*p)->next->segment!=""&&
                (*p)->next->segment!=SYM_CLOSE_PAREN
            )
            {
                if(
                    (*p)->next->segment==SYM_SEMICOLON||
                    (*p)->next->segment==SYM_OPEN_PAREN||
                    (*p)->next->segment==SYM_EQUAL
                    )
                    return ERROR_WRONG_PARAMETER;


                (*p)=(*p)->next;
                // if((*p)->segment!=SYM_COMMA)
                //     //括号内首个字段为 逗号
                //     if(keyq.size()==0)
                //         return ERROR_WRONG_PARAMETER;
                //     else
                //         continue;

                if(switcher==0&&(*p)->segment!=SYM_COMMA)
                {
                    keyq.push((*p)->segment);
                    switcher=1;
                }
                else if(switcher==1&&(*p)->segment==SYM_COMMA)
                {
                    switcher=0;
                    continue;
                }
                else
                    return ERROR_WRONG_PARAMETER;
            }

            // values
            if((*p)->next==NULL||(*p)->next->segment!=EXEC_VALUES)
                return ERROR_WRONG_PARAMETER;
            
            (*p)=(*p)->next;

            switcher=0;

            // 下一个字段要由 左圆括号开始
            if(
                (*p)->next==NULL||
                (*p)->next->segment!=SYM_OPEN_PAREN
            )
                return ERROR_WRONG_PARAMETER;
            
            (*p)=(*p)->next;

            while(
                (*p)->next!=NULL&&
                (*p)->next->segment!=""&&
                (*p)->next->segment!=SYM_CLOSE_PAREN
            )
            {
                if(
                    (*p)->next->segment==SYM_SEMICOLON||
                    (*p)->next->segment==SYM_OPEN_PAREN||
                    (*p)->next->segment==SYM_EQUAL
                    )
                    return ERROR_WRONG_PARAMETER;

                (*p)=(*p)->next;
                // if((*p)->segment!=SYM_COMMA)
                //     //括号内首个字段为 逗号
                //     if(keyq.size()==0)
                //         return ERROR_WRONG_PARAMETER;
                //     else
                //         continue;

                if(switcher==0&&(*p)->segment!=SYM_COMMA)
                {
                    valueq.push((*p)->segment);
                    switcher=1;
                }
                else if(switcher==1&&(*p)->segment==SYM_COMMA)
                {
                    switcher=0;
                    continue;
                }
                else
                    return ERROR_WRONG_PARAMETER;

                // (*p)=(*p)->next;
                // if((*p)->segment!=SYM_COMMA)
                //     continue;
                // valueq.push((*p)->segment);
            }

            // 如果没有列 则错误
            if(keyq.size()==0)
                return ERROR_WRONG_PARAMETER;

            // 如果 列数 与 值的数量 不同 则错误
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
                (*p)->next->segment!=""&&
                (*p)->next->segment!=SYM_SEMICOLON&&
                (*p)->next->segment!=SYM_OPEN_PAREN&&
                (*p)->next->segment!=SYM_CLOSE_PAREN&&
                (*p)->next->segment!=SYM_EQUAL
            )
            {
                (*p)=(*p)->next;
                strcat(exec,(*p)->segment);
                
                if(
                    (*p)->next!=NULL&&
                    (*p)->next->segment==SYM_EQUAL
                )
                    (*p)=(*p)->next;
                else
                    return ERROR_WRONG_PARAMETER;

                if(
                    (*p)->next!=NULL&&
                    (*p)->next->segment!=""&&
                    (*p)->next->segment!=SYM_SEMICOLON&&
                    (*p)->next->segment!=SYM_OPEN_PAREN&&
                    (*p)->next->segment!=SYM_CLOSE_PAREN&&
                    (*p)->next->segment!=SYM_EQUAL
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
        }else
            return ERROR_WRONG_PARAMETER;
        //set	[$columnname]	=	[$columnvalue]	
    }
}

int handle_condi(char *action, char *condi, seg_node** p)
{   //若是 show 非 tables命令 或 drop row 命令，且 此字段为 CONDI_FROM
    if((action==ACTION_SHOW||action==ACTION_DROP)&&(*p)->segment==CONDI_FROM)
    {
        //不对 from 关键字 进行保存，只需要保存 from 之后的 $tablename
        //检查是否存在下一个字段 且 下一个字段是否为空串或分号或圆括号或逗号或等号
        if(
            (*p)->next==NULL||
            (*p)->next->segment==""||
            (*p)->next->segment==SYM_SEMICOLON||
            (*p)->next->segment==SYM_OPEN_PAREN||
            (*p)->next->segment==SYM_CLOSE_PAREN||
            (*p)->next->segment==SYM_EQUAL
            )
            return(ERROR_WRONG_PARAMETER);
        else
        {
            (*p)=(*p)->next;
            strcat(condi,(*p)->segment);
            return RESULT_NORMAL;
        }
        // if(
        //     (*p)->next!=NULL&&
        //     (*p)->next->segment!=SYM_SEMICOLON&&
        //     (*p)->next->segment!=""
        // )
        // {
        //     (*p)=(*p)->next;
        //     strcat(condi,(*p)->segment);
        //     return RESULT_NORMAL;
        // }else{
        //     return ERROR_WRONG_PARAMETER;
        // }
    }
    else if(action==ACTION_UPDATE&&(*p)->segment==CONDI_WHERE)
    {
        // where
        if(
            (*p)->next!=NULL||
            (*p)->next->segment!=""&&
            (*p)->next->segment!=SYM_SEMICOLON&&
            (*p)->next->segment!=SYM_OPEN_PAREN&&
            (*p)->next->segment!=SYM_CLOSE_PAREN&&
            (*p)->next->segment!=SYM_EQUAL
            )
            {
                (*p)=(*p)->next;
                // $columnname
                strcat(condi,(*p)->segment);
                if(
                    (*p)->next!=NULL||
                    (*p)->next->segment==SYM_EQUAL
                    )
                    {
                        (*p)=(*p)->next;
                        // =
                        strcat(condi,(*p)->segment);
                        
                        if(
                            (*p)->next!=NULL||
                            (*p)->next->segment!=""&&
                            (*p)->next->segment!=SYM_SEMICOLON&&
                            (*p)->next->segment!=SYM_OPEN_PAREN&&
                            (*p)->next->segment!=SYM_CLOSE_PAREN&&
                            (*p)->next->segment!=SYM_EQUAL
                            )
                            {
                                (*p)=(*p)->next;
                                // $columnname2
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
    }else{
        return ERROR_WRONG_PARAMETER;
    }
}

int Yacc(){
    std::queue<seg_node *> route;
    char Action[BUF_SZ]="";
    char PreObject[BUF_SZ]="";
    char Object[BUF_SZ]="";
    char Exec[BUF_SZ]="";
    char Condition[BUF_SZ]=""; 

    seg_node *p;
    p=seg_header;


    // stage Action
    //检查第一个字段是否为空串或分号或圆括号或逗号或等号
    if(
        p->segment==""||
        p->segment==SYM_SEMICOLON||
        p->segment==SYM_OPEN_PAREN||
        p->segment==SYM_CLOSE_PAREN||
        p->segment==SYM_EQUAL
        )
        return(ERROR_TOO_FEW_PARAMETER);

    //处理第一个字段是否是合法 Action
    if(handle_act(Action,p)==ERROR_WRONG_PARAMETER)
        return(ERROR_WRONG_PARAMETER);
    
    // stage PreObject
    //检查是否存在第二个字段 且 第二个字段是否为空串或分号或圆括号或逗号或等号
    //若第二个字段不为空 则 p 指向第二个字段
    if(
        p->next==NULL||
        p->next->segment==""||
        p->next->segment==SYM_SEMICOLON||
        p->next->segment==SYM_OPEN_PAREN||
        p->next->segment==SYM_CLOSE_PAREN||
        p->next->segment==SYM_EQUAL
        )
        return(ERROR_TOO_FEW_PARAMETER);
    else
        p=p->next;

    //处理第二个字段是否时合法 PreObj
    if(handle_preobj(Action,PreObject,p)==ERROR_WRONG_PARAMETER)
        return(ERROR_WRONG_PARAMETER);

    // stage Object
    // Show 命令不需要 Obiect
    if(Action!=ACTION_SHOW)
    {
        //检查是否存在第三个字段 且 第三个字段是否为空串或分号或圆括号或逗号或等号
        //若第三个字段不为空 则 p 指向第三个字段
        if(
            p->next==NULL||
            p->next->segment==""||
            p->next->segment==SYM_SEMICOLON||
            p->next->segment==SYM_OPEN_PAREN||
            p->next->segment==SYM_CLOSE_PAREN||
            p->next->segment==SYM_EQUAL
            )
            return(ERROR_TOO_FEW_PARAMETER);
        else
            p=p->next;
        //处理第三个字段是否为合法 Object
        if(handle_obj(Object,p)==ERROR_WRONG_PARAMETER)
            return(ERROR_WRONG_PARAMETER);
    }

    // stage Exec
    // Show 命令 与 Drop 命令 不需要 Exec 字段
    if(Action!=ACTION_SHOW&&Action!=ACTION_DROP)
    {
        //检查是否存在第四个字段 且 第四个字段是否为空串或分号或圆括号或逗号或等号
        //若第四个字段不为空 则 p 指向第四个字段
        if(
            p->next==NULL||
            p->next->segment==""||
            p->next->segment==SYM_SEMICOLON||
            p->next->segment==SYM_OPEN_PAREN|| //可能是 Create 或 Insert
            p->next->segment==SYM_CLOSE_PAREN||
            p->next->segment==SYM_EQUAL
            )
            return(ERROR_TOO_FEW_PARAMETER);
        else
            p=p->next;
        //处理第四个字段是否为合法 Exec
        if(handle_exec(Action,Exec,&p)!=RESULT_NORMAL)
            return ERROR_WRONG_PARAMETER;
    }

    // stage condition

    // 仅 Show 非 tables 命令 与 Update 命令 与 Drop row 命令 需要 Condition 字段
    if(
        (Action==ACTION_SHOW&&PreObject!=PREOBJ_TABLES)||
        Action==ACTION_UPDATE||
        (Action==ACTION_DROP&&PreObject==PREOBJ_ROW)
    )
    {
        //检查是否存在第五个字段 且 第五个字段是否为空串或分号或圆括号或逗号或等号
        //若第五个字段不为空 则 p 指向第五个字段
        if(
            p->next==NULL||
            p->next->segment==""||
            p->next->segment==SYM_SEMICOLON||
            p->next->segment==SYM_OPEN_PAREN||
            p->next->segment==SYM_CLOSE_PAREN||
            p->next->segment==SYM_EQUAL
            )
            return(ERROR_TOO_FEW_PARAMETER);
        else
            p=p->next;

        //处理第五个字段是否为合法 Condition
        if(handle_condi(Action,Condition,&p)!=RESULT_NORMAL)
            return ERROR_WRONG_PARAMETER;
    }

    // stage end
    // 若不存在下一个字段 或 下一个字段为分号 则视为命令结束，打包命令结构
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

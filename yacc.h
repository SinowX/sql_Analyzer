// #include"definition.h"
// #include<
#include<string.h>
#include<unistd.h>
// #include<stack>
#include<queue>
#include<pwd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string>
// #include"definition.h"
// using namespace std;

int handle_act(char action[BUF_SZ])
{
    if(!strcmp(p->segment,ACTION_CREATE)||!strcmp(p->segment,ACTION_SHOW)||
    !strcmp(p->segment,ACTION_INSERT)||!strcmp(p->segment,ACTION_UPDATE)||
    !strcmp(p->segment,ACTION_DROP)||!strcmp(p->segment,ACTION_SELECT)) // for select
    {
        strcpy(action,p->segment);
        return RESULT_NORMAL;
    }
    else
        return ERROR_WRONG_PARAMETER;
}

int handle_preobj(char action[BUF_SZ],char preobj[BUF_SZ])
{
    if(
        (!strcmp(action,ACTION_CREATE)&&!strcmp(p->segment,PREOBJ_TABLE))||
        (!strcmp(action,ACTION_SHOW)&&!strcmp(p->segment,PREOBJ_TABLES))||
        (!strcmp(action,ACTION_SHOW)&&!strcmp(p->segment,PREOBJ_COLUMNS))||
        (!strcmp(action,ACTION_SHOW)&&!strcmp(p->segment,PREOBJ_INDEX))||
        (!strcmp(action,ACTION_SHOW)&&!strcmp(p->segment,PREOBJ_STATUS))||
        (!strcmp(action,ACTION_INSERT)&&!strcmp(p->segment,PREOBJ_INTO))||
        (!strcmp(action,ACTION_UPDATE)&&!strcmp(p->segment,PREOBJ_VALUE))
        // (!strcmp(action,ACTION_DROP)&&!strcmp(p->segment,PREOBJ_TABLE))||
        // (!strcmp(action,ACTION_DROP)&&!strcmp(p->segment,PREOBJ_ROW))
    )
    {
        strcpy(preobj,p->segment);
        return RESULT_NORMAL;
    }
    else
        return ERROR_WRONG_PARAMETER;
}

int handle_obj(char action[BUF_SZ],char object[BUF_SZ])
{
    
    if(strcmp(action,ACTION_SELECT))
        strcpy(object,p->segment);
    else
    {
        bool ifShouldCOMMA=false;
        bool ifFirstRow=true;
        while(true)
        {
            if(
                p->next==NULL||
                !strcmp(p->next->segment,"")||
                !strcmp(p->next->segment,SYM_EQUAL)||
                !strcmp(p->next->segment,SYM_SPACE)||
                !strcmp(p->next->segment,SYM_OPEN_PAREN)||
                !strcmp(p->next->segment,SYM_CLOSE_PAREN)||
                !strcmp(p->next->segment,SYM_SEMICOLON)
                )
                return ERROR_WRONG_PARAMETER;
            else if(!ifFirstRow)
            {
                p=p->next;
            }
            ifFirstRow=false;

            if(!strcmp(p->segment,SYM_STAR))
            {
                //如果是 * ，则 obj 中只有 *
                strcat(object,p->segment);
                p=p->next;
                break;
            }
            if(!ifShouldCOMMA&&strcmp(p->segment,SYM_COMMA))
            {
                strcat(object,p->segment);
                strcat(object,":");
                ifShouldCOMMA=!ifShouldCOMMA;
            }
            else if(ifShouldCOMMA&&!strcmp(p->segment,SYM_COMMA))
            {
                ifShouldCOMMA=!ifShouldCOMMA;
                continue;
            }
            else if(ifShouldCOMMA&&strcmp(p->segment,SYM_COMMA))
            {
                ifShouldCOMMA=!ifShouldCOMMA;
                break;
            }
        }
    }
    
    return RESULT_NORMAL;

}

int handle_exec(char action[BUF_SZ],char exec[BUF_SZ])
{
    // for select
    if(!strcmp(ACTION_SELECT,action)||!strcmp(ACTION_DROP,action))
    {
        if(
            p->next==NULL||
            !strcmp(p->next->segment,"")||
            !strcmp(p->next->segment,SYM_EQUAL)||
            !strcmp(p->next->segment,SYM_SPACE)||
            !strcmp(p->next->segment,SYM_OPEN_PAREN)||
            !strcmp(p->next->segment,SYM_CLOSE_PAREN)||
            !strcmp(p->next->segment,SYM_SEMICOLON)||
            !strcmp(p->next->segment,SYM_STAR)||
            !strcmp(p->next->segment,SYM_COMMA)
            )
            return ERROR_WRONG_PARAMETER;
        else
        {
            // p=p->next;
            if(!strcmp(p->segment,EXEC_FROM))    
            {
                // strcat(exec,p->segment);
                
                if(
                    p->next==NULL||
                    !strcmp(p->next->segment,"")||
                    !strcmp(p->next->segment,SYM_EQUAL)||
                    !strcmp(p->next->segment,SYM_SPACE)||
                    !strcmp(p->next->segment,SYM_OPEN_PAREN)||
                    !strcmp(p->next->segment,SYM_CLOSE_PAREN)||
                    !strcmp(p->next->segment,SYM_SEMICOLON)||
                    !strcmp(p->next->segment,SYM_STAR)||
                    !strcmp(p->next->segment,SYM_COMMA)
                    )
                    return ERROR_WRONG_PARAMETER;
                else
                {
                    p=p->next;
                    strcat(exec,p->segment);
                    return RESULT_NORMAL;
                }                
            }
            else
                return ERROR_WRONG_PARAMETER;
        }
    }
    // std::queue<char [BUF_SZ]> keyq,valueq;
    std::string tmpstr;
    std::queue<std::string> keyq,valueq;
    char buff[BUF_SZ]="-1";
    // 0 wait for segment, 1 wait for ';'
    int switcher=0;
    if(!strcmp(action,ACTION_CREATE))
    {
        // (	[$columnname	$attributes]	)

        if(!strcmp(p->segment,SYM_OPEN_PAREN))
        {
            while(
                p->next!=NULL&&
                strcmp(p->next->segment,"")&&
                strcmp(p->next->segment,SYM_CLOSE_PAREN)&&
                strcmp(p->segment,SYM_CLOSE_PAREN)
            )
            {
                if(
                    !strcmp(p->next->segment,SYM_SEMICOLON)||
                    !strcmp(p->next->segment,SYM_OPEN_PAREN)||
                    !strcmp(p->next->segment,SYM_EQUAL)
                    )
                    return ERROR_WRONG_PARAMETER;

                p=p->next;
                //一个 column name
                keyq.push(p->segment);
                while(
                    p->next!=NULL&&
                    // strcmp(p->next->segment,"")&&
                    strcmp(p->next->segment,SYM_COMMA)&&
                    strcmp(p->next->segment,SYM_CLOSE_PAREN)&&
                    strcmp(p->segment,SYM_CLOSE_PAREN)
                )
                {

                    if(
                        !strcmp(p->next->segment,SYM_SEMICOLON)||
                        !strcmp(p->next->segment,SYM_EQUAL)
                        )
                        return ERROR_WRONG_PARAMETER;

                    // if(p->next==NULL)
                    //     return ERROR_WRONG_PARAMETER;
                    //清空 buffer
                    if(!strcmp(buff,"-1"))
                        strcpy(buff,""); 
                    p=p->next;
                    strcat(buff,p->segment);
                    strcat(buff,":");
                }
                if(strcmp(buff,"-1")) // 指令中有 attributes
                {
                    tmpstr=buff;
                    valueq.push(tmpstr);
                }
                else //若不存在 attributes 则错误
                    return ERROR_WRONG_PARAMETER;
                
                strcpy(buff,"-1");
                
                if(
                    p->next!=NULL&&
                    strcmp(p->next->segment,"")
                    )
                    p=p->next;

            } // 每个 column attributes 至少会有一个字段用于指明类型
            
            //如果没有 列 则错误
            if(keyq.size()==0)
                return ERROR_WRONG_PARAMETER;

            //如果 列的数量 与 列属性的数量 不对应，则错误
            if(keyq.size()==valueq.size())
            {
                while(!keyq.empty())
                {
                    strcat(exec,keyq.front().c_str());
                    strcat(exec,"=");
                    strcat(exec,valueq.front().c_str());
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
    else if(!strcmp(action,ACTION_INSERT))
    {
        // (	[$columnname]	)		values	(	[$columnvalue]	)

        if(!strcmp(p->segment,SYM_OPEN_PAREN))
        {
            int choose=0;
            while(
                p->next!=NULL&&
                strcmp(p->next->segment,"")&&
                strcmp(p->next->segment,SYM_CLOSE_PAREN)
            )
            {
                if(
                    !strcmp(p->next->segment,SYM_SEMICOLON)||
                    (choose==1&&!strcmp(p->next->segment,SYM_OPEN_PAREN))||
                    !strcmp(p->next->segment,SYM_EQUAL)
                    )
                    return ERROR_WRONG_PARAMETER;

                choose=1;

                p=p->next;
                // if(strcmp(p->segment,SYM_COMMA))
                //     //括号内首个字段为 逗号
                //     if(keyq.size()==0)
                //         return ERROR_WRONG_PARAMETER;
                //     else
                //         continue;

                if(switcher==0&&strcmp(p->segment,SYM_COMMA))
                {
                    tmpstr=p->segment;
                    keyq.push(tmpstr);
                    switcher=1;
                }
                else if(switcher==1&&!strcmp(p->segment,SYM_COMMA))
                {
                    switcher=0;
                    continue;
                }
                else
                    return ERROR_WRONG_PARAMETER;
            }

            if(p->next!=NULL&&!strcmp(p->next->segment,SYM_CLOSE_PAREN))
                p=p->next;
            else
                return ERROR_WRONG_PARAMETER;

            // values
            if(p->next==NULL||strcmp(p->next->segment,EXEC_VALUES))
                return ERROR_WRONG_PARAMETER;
            
            p=p->next;

            switcher=0;

            // 下一个字段要由 左圆括号开始
            if(
                p->next==NULL||
                strcmp(p->next->segment,SYM_OPEN_PAREN)
            )
                return ERROR_WRONG_PARAMETER;
            
            p=p->next;

            while(
                p->next!=NULL&&
                strcmp(p->next->segment,"")&&
                strcmp(p->next->segment,SYM_CLOSE_PAREN)
            )
            {
                if(
                    !strcmp(p->next->segment,SYM_SEMICOLON)||
                    !strcmp(p->next->segment,SYM_OPEN_PAREN)||
                    !strcmp(p->next->segment,SYM_EQUAL)
                    )
                    return ERROR_WRONG_PARAMETER;

                p=p->next;
                // if(strcmp(p->segment,SYM_COMMA))
                //     //括号内首个字段为 逗号
                //     if(keyq.size()==0)
                //         return ERROR_WRONG_PARAMETER;
                //     else
                //         continue;

                if(switcher==0&&strcmp(p->segment,SYM_COMMA))
                {
                    tmpstr=p->segment;
                    valueq.push(tmpstr);
                    switcher=1;
                }
                else if(switcher==1&&!strcmp(p->segment,SYM_COMMA))
                {
                    switcher=0;
                    continue;
                }
                else
                    return ERROR_WRONG_PARAMETER;

                // p=p->next;
                // if(strcmp(p->segment,SYM_COMMA))
                //     continue;
                // valueq.push(p->segment);
            }

            // 如果没有列 则错误
            if(keyq.size()==0)
                return ERROR_WRONG_PARAMETER;

            // 如果 列数 与 值的数量 不同 则错误
            if(keyq.size()==valueq.size())
            {
                while(!keyq.empty())
                {
                    strcat(exec,keyq.front().c_str());
                    strcat(exec,"=");
                    strcat(exec,valueq.front().c_str());
                    strcat(exec,";");
                    keyq.pop();
                    valueq.pop();
                }
                if(p->next!=NULL&&strcmp(p->next->segment,""))
                {
                    p=p->next;
                    return RESULT_NORMAL;
                }
                else
                    return ERROR_WRONG_PARAMETER;
                    

                // return RESULT_NORMAL;
            }
            else
                return(ERROR_KEY_VALUE);
        }
        else
            return ERROR_WRONG_PARAMETER;
    }
    else if((action,ACTION_UPDATE))
    {
        if(!strcmp(p->segment,EXEC_SET))
        {
            if(
                p->next!=NULL&&
                strcmp(p->next->segment,"")&&
                strcmp(p->next->segment,SYM_SEMICOLON)&&
                strcmp(p->next->segment,SYM_OPEN_PAREN)&&
                strcmp(p->next->segment,SYM_CLOSE_PAREN)&&
                (p->next->segment,SYM_EQUAL)
            )
            {
                p=p->next;
                strcat(exec,p->segment);
                
                if(
                    p->next!=NULL&&
                    !strcmp(p->next->segment,SYM_EQUAL)
                )
                    p=p->next;
                else
                    return ERROR_WRONG_PARAMETER;

                if(
                    p->next!=NULL&&
                    strcmp(p->next->segment,"")&&
                    strcmp(p->next->segment,SYM_SEMICOLON)&&
                    strcmp(p->next->segment,SYM_OPEN_PAREN)&&
                    strcmp(p->next->segment,SYM_CLOSE_PAREN)&&
                    strcmp(p->next->segment,SYM_EQUAL)
                )
                {
                    p=p->next;
                    strcat(exec,"=");
                    strcat(exec,p->segment);
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
    }else
        return ERROR_WRONG_PARAMETER;
}


int handle_condi(char action[BUF_SZ], char condi[BUF_SZ])
{   //若是 show 非 tables命令 或 drop row 命令，且 此字段为 CONDI_FROM
    if((!strcmp(action,ACTION_SHOW)||!strcmp(action,ACTION_DROP))&&!strcmp(p->segment,CONDI_FROM))
    {
        //不对 from 关键字 进行保存，只需要保存 from 之后的 $tablename
        //检查是否存在下一个字段 且 下一个字段是否为空串或分号或圆括号或逗号或等号
        if(
            p->next==NULL||
            !strcmp(p->next->segment,"")||
            !strcmp(p->next->segment,SYM_SEMICOLON)||
            !strcmp(p->next->segment,SYM_OPEN_PAREN)||
            !strcmp(p->next->segment,SYM_CLOSE_PAREN)||
            !strcmp(p->next->segment,SYM_EQUAL)
            )
            return(ERROR_WRONG_PARAMETER);
        else
        {
            p=p->next;
            strcat(condi,p->segment);
            return RESULT_NORMAL;
        }
        // if(
        //     p->next!=NULL&&
        //     strcmp(p->next->segment,SYM_SEMICOLON)&&
        //     strcmp(p->next->segment,"")
        // )
        // {
        //     p=p->next;
        //     strcat(condi,p->segment);
        //     return RESULT_NORMAL;
        // }else{
        //     return ERROR_WRONG_PARAMETER;
        // }
    }
    else if((!strcmp(action,ACTION_UPDATE)||!strcmp(action,ACTION_SELECT)||!strcmp(action,ACTION_DROP))
    &&!strcmp(p->segment,CONDI_WHERE))
    {
        // where
        if(
            p->next!=NULL||
            strcmp(p->next->segment,"")&&
            strcmp(p->next->segment,SYM_SEMICOLON)&&
            strcmp(p->next->segment,SYM_OPEN_PAREN)&&
            strcmp(p->next->segment,SYM_CLOSE_PAREN)&&
            strcmp(p->next->segment,SYM_EQUAL)
            )
            {
                p=p->next;
                // $columnname
                strcat(condi,p->segment);
                if(
                    p->next!=NULL||
                    !strcmp(p->next->segment,SYM_EQUAL)
                    )
                    {
                        p=p->next;
                        // =
                        strcat(condi,p->segment);
                        
                        if(
                            p->next!=NULL||
                            strcmp(p->next->segment,"")&&
                            strcmp(p->next->segment,SYM_SEMICOLON)&&
                            strcmp(p->next->segment,SYM_OPEN_PAREN)&&
                            strcmp(p->next->segment,SYM_CLOSE_PAREN)&&
                            (p->next->segment,SYM_EQUAL)
                            )
                            {
                                p=p->next;
                                // $columnname2
                                strcat(condi,p->segment);
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

    // seg_node *p;
    p=seg_header;
    p->next=seg_header->next;

    // stage Action
    //检查第一个字段是否为空串或分号或圆括号或逗号或等号
    if(
        !strcmp(p->segment,"")||
        !strcmp(p->segment,SYM_SEMICOLON)||
        !strcmp(p->segment,SYM_OPEN_PAREN)||
        !strcmp(p->segment,SYM_CLOSE_PAREN)||
        !strcmp(p->segment,SYM_EQUAL)
        )
        return(ERROR_TOO_FEW_PARAMETER);

    //处理第一个字段是否是合法 Action
    if((handle_act(Action)==ERROR_WRONG_PARAMETER))
        return(ERROR_WRONG_PARAMETER);
    
    // stage PreObject
    //检查是否存在第二个字段 且 第二个字段是否为空串或分号或圆括号或逗号或等号
    //若第二个字段不为空 则 p 指向第二个字段
    if(
        p->next==NULL||
        !strcmp(p->next->segment,"")||
        !strcmp(p->next->segment,SYM_SEMICOLON)||
        !strcmp(p->next->segment,SYM_OPEN_PAREN)||
        !strcmp(p->next->segment,SYM_CLOSE_PAREN)||
        !strcmp(p->next->segment,SYM_EQUAL)
        )
        return(ERROR_TOO_FEW_PARAMETER);
    else if(strcmp(Action,ACTION_SELECT)&&strcmp(Action,ACTION_DROP)) //for select ,select 没有 preObj
        p=p->next;

    //处理第二个字段是否时合法 PreObj
    if(strcmp(Action,ACTION_SELECT)&&strcmp(Action,ACTION_DROP))
        if(handle_preobj(Action,PreObject)==ERROR_WRONG_PARAMETER)
            return(ERROR_WRONG_PARAMETER);

    // stage Object
    // Show 命令不需要 Obiect
    if(strcmp(Action,ACTION_SHOW))
    {
        //检查是否存在第三个字段 且 第三个字段是否为空串或分号或圆括号或逗号或等号
        //若第三个字段不为空 则 p 指向第三个字段
        if(
            p->next==NULL||
            !strcmp(p->next->segment,"")||
            !strcmp(p->next->segment,SYM_SEMICOLON)||
            !strcmp(p->next->segment,SYM_OPEN_PAREN)||
            !strcmp(p->next->segment,SYM_CLOSE_PAREN)||
            !strcmp(p->next->segment,SYM_EQUAL)
            )
            return(ERROR_TOO_FEW_PARAMETER);
        else
            p=p->next;
        //处理第三个字段是否为合法 Object
        if(handle_obj(Action,Object)==ERROR_WRONG_PARAMETER)
            return(ERROR_WRONG_PARAMETER);
    }

    // stage Exec=
    // Show 命令 不需要 Exec 字段
    if(strcmp(Action,ACTION_SHOW))
    {
        //检查是否存在第四个字段 且 第四个字段是否为空串或分号或圆括号或逗号或等号
        //若第四个字段不为空 则 p 指向第四个字段
        if(
            p->next==NULL||
            !strcmp(p->next->segment,"")||
            !strcmp(p->next->segment,SYM_SEMICOLON)||
            // !strcmp(p->next->segment,SYM_OPEN_PAREN)|| //可能是 Create 或 Insert
            !strcmp(p->next->segment,SYM_CLOSE_PAREN)||
            !strcmp(p->next->segment,SYM_EQUAL)
            )
            return(ERROR_TOO_FEW_PARAMETER);
        else if(strcmp(Action,ACTION_SELECT))
            p=p->next;
        //处理第四个字段是否为合法 Exec
        if(handle_exec(Action,Exec)!=RESULT_NORMAL)
            return ERROR_WRONG_PARAMETER;
    }

    // if(
    //     p->next!=NULL&&
    //     strcmp(p->next->segment,"")
    //     )
    //     p=p->next;
    // stage condition

    // 仅 Show 非 tables 命令 与 Update 命令 与 Drop row 命令 需要 Condition 字段
    if(
        (!strcmp(Action,ACTION_SHOW)&&strcmp(PreObject,PREOBJ_TABLES))||
        !strcmp(Action,ACTION_UPDATE)||!strcmp(Action,ACTION_SELECT)||
        (!strcmp(Action,ACTION_DROP))
    )
    {
        //检查是否存在第五个字段 且 第五个字段是否为空串或分号或圆括号或逗号或等号
        //若第五个字段不为空 则 p 指向第五个字段
        if(
            p->next==NULL||
            !strcmp(p->next->segment,"")||
            !strcmp(p->next->segment,SYM_SEMICOLON)||
            !strcmp(p->next->segment,SYM_OPEN_PAREN)||
            !strcmp(p->next->segment,SYM_CLOSE_PAREN)||
            !strcmp(p->next->segment,SYM_EQUAL)
            )
            return(ERROR_TOO_FEW_PARAMETER);
        else
            p=p->next;

        //处理第五个字段是否为合法 Condition
        if(handle_condi(Action,Condition)!=RESULT_NORMAL)
            return ERROR_WRONG_PARAMETER;
    }

    // stage end
    // 若不存在下一个字段 或 下一个字段为分号 则视为命令结束，打包命令结构
    if(p->next==NULL||!strcmp(p->next->segment,SYM_SEMICOLON))
    {
        set_pack(Action,PreObject,Object,Exec,Condition);
        return RESULT_NORMAL;
    }
    else
        return ERROR_TOO_MANY_PARAMETER;

    
}

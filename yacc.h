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
    !strcmp(p->segment,ACTION_DROP)||!strcmp(p->segment,ACTION_SELECT)||
    !strcmp(p->segment,ACTION_SESSION_ON)||!strcmp(p->segment,ACTION_SESSION_OFF)) // for select
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
                //????????? * ?????? obj ????????? *
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
                //?????? column name
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
                    //?????? buffer
                    if(!strcmp(buff,"-1"))
                        strcpy(buff,""); 
                    p=p->next;
                    strcat(buff,p->segment);
                    strcat(buff,":");
                }
                if(strcmp(buff,"-1")) // ???????????? attributes
                {
                    tmpstr=buff;
                    valueq.push(tmpstr);
                }
                else //???????????? attributes ?????????
                    return ERROR_WRONG_PARAMETER;
                
                strcpy(buff,"-1");
                
                if(
                    p->next!=NULL&&
                    strcmp(p->next->segment,"")
                    )
                    p=p->next;

            } // ?????? column attributes ??????????????????????????????????????????
            
            //???????????? ??? ?????????
            if(keyq.size()==0)
                return ERROR_WRONG_PARAMETER;

            //?????? ???????????? ??? ?????????????????? ?????????????????????
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
                //     //???????????????????????? ??????
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

            // ????????????????????? ??????????????????
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
                //     //???????????????????????? ??????
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

            // ??????????????? ?????????
            if(keyq.size()==0)
                return ERROR_WRONG_PARAMETER;

            // ?????? ?????? ??? ???????????? ?????? ?????????
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
                    strcat(exec,";");
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
{   //?????? show ??? tables?????? ??? drop row ???????????? ???????????? CONDI_FROM
    if((!strcmp(action,ACTION_SHOW)||!strcmp(action,ACTION_DROP))&&!strcmp(p->segment,CONDI_FROM))
    {
        //?????? from ????????? ?????????????????????????????? from ????????? $tablename
        //????????????????????????????????? ??? ?????????????????????????????????????????????????????????????????????
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
                                strcat(condi,";");
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
    //???????????????????????????????????????????????????????????????????????????
    if(
        !strcmp(p->segment,"")||
        !strcmp(p->segment,SYM_SEMICOLON)||
        !strcmp(p->segment,SYM_OPEN_PAREN)||
        !strcmp(p->segment,SYM_CLOSE_PAREN)||
        !strcmp(p->segment,SYM_EQUAL)
        )
        return(ERROR_TOO_FEW_PARAMETER);

    //???????????????????????????????????? Action
    if((handle_act(Action)==ERROR_WRONG_PARAMETER))
        return(ERROR_WRONG_PARAMETER);
    
    if(!strcmp(Action,ACTION_SESSION_ON)||!strcmp(Action,ACTION_SESSION_OFF))
    {
        set_pack(Action,PreObject,Object,Exec,Condition);
        return RESULT_NORMAL;
    }
    // stage PreObject
    //????????????????????????????????? ??? ?????????????????????????????????????????????????????????????????????
    //??????????????????????????? ??? p ?????????????????????
    if(
        p->next==NULL||
        !strcmp(p->next->segment,"")||
        !strcmp(p->next->segment,SYM_SEMICOLON)||
        !strcmp(p->next->segment,SYM_OPEN_PAREN)||
        !strcmp(p->next->segment,SYM_CLOSE_PAREN)||
        !strcmp(p->next->segment,SYM_EQUAL)
        )
        return(ERROR_TOO_FEW_PARAMETER);
    else if(strcmp(Action,ACTION_SELECT)&&strcmp(Action,ACTION_DROP)) //for select ,select ?????? preObj
        p=p->next;

    //???????????????????????????????????? PreObj
    if(strcmp(Action,ACTION_SELECT)&&strcmp(Action,ACTION_DROP))
        if(handle_preobj(Action,PreObject)==ERROR_WRONG_PARAMETER)
            return(ERROR_WRONG_PARAMETER);

    // stage Object
    // Show ??????????????? Obiect
    if(strcmp(Action,ACTION_SHOW))
    {
        //????????????????????????????????? ??? ?????????????????????????????????????????????????????????????????????
        //??????????????????????????? ??? p ?????????????????????
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
        //???????????????????????????????????? Object
        if(handle_obj(Action,Object)==ERROR_WRONG_PARAMETER)
            return(ERROR_WRONG_PARAMETER);
    }

    // stage Exec=
    // Show ?????? ????????? Exec ??????
    if(strcmp(Action,ACTION_SHOW))
    {
        //????????????????????????????????? ??? ?????????????????????????????????????????????????????????????????????
        //??????????????????????????? ??? p ?????????????????????
        if(
            p->next==NULL||
            !strcmp(p->next->segment,"")||
            !strcmp(p->next->segment,SYM_SEMICOLON)||
            // !strcmp(p->next->segment,SYM_OPEN_PAREN)|| //????????? Create ??? Insert
            !strcmp(p->next->segment,SYM_CLOSE_PAREN)||
            !strcmp(p->next->segment,SYM_EQUAL)
            )
            return(ERROR_TOO_FEW_PARAMETER);
        else if(strcmp(Action,ACTION_SELECT))
            p=p->next;
        //???????????????????????????????????? Exec
        if(handle_exec(Action,Exec)!=RESULT_NORMAL)
            return ERROR_WRONG_PARAMETER;
    }

    // if(
    //     p->next!=NULL&&
    //     strcmp(p->next->segment,"")
    //     )
    //     p=p->next;
    // stage condition

    // ??? Show ??? tables ?????? ??? Update ?????? ??? Drop row ?????? ?????? Condition ??????
    if(
        (!strcmp(Action,ACTION_SHOW)&&strcmp(PreObject,PREOBJ_TABLES))||
        !strcmp(Action,ACTION_UPDATE)||!strcmp(Action,ACTION_SELECT)||
        (!strcmp(Action,ACTION_DROP))
    )
    {
        //????????????????????????????????? ??? ?????????????????????????????????????????????????????????????????????
        //??????????????????????????? ??? p ?????????????????????
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

        //???????????????????????????????????? Condition
        if(handle_condi(Action,Condition)!=RESULT_NORMAL)
            return ERROR_WRONG_PARAMETER;
    }

    // stage end
    // ??????????????????????????? ??? ???????????????????????? ??????????????????????????????????????????
    if(p->next==NULL||!strcmp(p->next->segment,SYM_SEMICOLON))
    {
        set_pack(Action,PreObject,Object,Exec,Condition);
        return RESULT_NORMAL;
    }
    else
        return ERROR_TOO_MANY_PARAMETER;

    
}

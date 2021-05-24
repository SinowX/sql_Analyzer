int handle_condi(char action[BUF_SZ], char condi[BUF_SZ], seg_node* p)
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
    else if((action,ACTION_UPDATE)&&p->segment==CONDI_WHERE)
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
int handle_exec(char action[BUF_SZ],char exec[BUF_SZ], seg_node **p)
{
    // std::queue<char [BUF_SZ]> keyq,valueq;
    std::queue<std::string> keyq,valueq;
    char buff[BUF_SZ]="-1";
    // 0 wait for segment, 1 wait for ';'
    int switcher=0;
    if(!strcmp(action,ACTION_CREATE))
    {
        // (	[$columnname	$attributes]	)

        if(!strcmp((*p)->segment,SYM_OPEN_PAREN))
        {
            while(
                (*p)->next!=NULL&&
                strcmp((*p)->next->segment,"")&&
                strcmp((*p)->next->segment,SYM_CLOSE_PAREN)
            )
            {
                if(
                    !strcmp((*p)->next->segment,SYM_SEMICOLON)||
                    !strcmp((*p)->next->segment,SYM_OPEN_PAREN)||
                    !strcmp((*p)->next->segment,SYM_EQUAL)
                    )
                    return ERROR_WRONG_PARAMETER;

                (*p)=(*p)->next;
                //一个 column name
                keyq.push((*p)->segment);
                while(
                    (*p)->next!=NULL&&
                    // strcmp((*p)->next->segment,"")&&
                    strcmp((*p)->next->segment,SYM_COMMA)&&
                    strcmp((*p)->next->segment,SYM_CLOSE_PAREN)
                )
                {

                    if(
                        !strcmp((*p)->next->segment,SYM_SEMICOLON)||
                        !strcmp((*p)->next->segment,SYM_EQUAL)
                        )
                        return ERROR_WRONG_PARAMETER;

                    // if((*p)->next==NULL)
                    //     return ERROR_WRONG_PARAMETER;
                    //清空 buffer
                    if(!strcmp(buff,"-1"))
                        strcpy(buff,""); 
                    (*p)=(*p)->next;
                    strcat(buff,(*p)->segment);
                    strcat(buff,":");
                }
                if(strcmp(buff,"-1")) // 指令中有 attributes 
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

        if(!strcmp((*p)->segment,SYM_OPEN_PAREN))
        {
            while(
                (*p)->next!=NULL&&
                strcmp((*p)->next->segment,"")&&
                strcmp((*p)->next->segment,SYM_CLOSE_PAREN)
            )
            {
                if(
                    !strcmp((*p)->next->segment,SYM_SEMICOLON)||
                    !strcmp((*p)->next->segment,SYM_OPEN_PAREN)||
                    !strcmp((*p)->next->segment,SYM_EQUAL)
                    )
                    return ERROR_WRONG_PARAMETER;


                (*p)=(*p)->next;
                // if(strcmp((*p)->segment,SYM_COMMA))
                //     //括号内首个字段为 逗号
                //     if(keyq.size()==0)
                //         return ERROR_WRONG_PARAMETER;
                //     else
                //         continue;

                if(switcher==0&&strcmp((*p)->segment,SYM_COMMA))
                {
                    keyq.push((*p)->segment);
                    switcher=1;
                }
                else if(switcher==1&&!strcmp((*p)->segment,SYM_COMMA))
                {
                    switcher=0;
                    continue;
                }
                else
                    return ERROR_WRONG_PARAMETER;
            }

            // values
            if((*p)->next==NULL||strcmp((*p)->next->segment,EXEC_VALUES))
                return ERROR_WRONG_PARAMETER;
            
            (*p)=(*p)->next;

            switcher=0;

            // 下一个字段要由 左圆括号开始
            if(
                (*p)->next==NULL||
                strcmp((*p)->next->segment,SYM_OPEN_PAREN)
            )
                return ERROR_WRONG_PARAMETER;
            
            (*p)=(*p)->next;

            while(
                (*p)->next!=NULL&&
                strcmp((*p)->next->segment,"")&&
                strcmp((*p)->next->segment,SYM_CLOSE_PAREN)
            )
            {
                if(
                    !strcmp((*p)->next->segment,SYM_SEMICOLON)||
                    !strcmp((*p)->next->segment,SYM_OPEN_PAREN)||
                    !strcmp((*p)->next->segment,SYM_EQUAL)
                    )
                    return ERROR_WRONG_PARAMETER;

                (*p)=(*p)->next;
                // if(strcmp((*p)->segment,SYM_COMMA))
                //     //括号内首个字段为 逗号
                //     if(keyq.size()==0)
                //         return ERROR_WRONG_PARAMETER;
                //     else
                //         continue;

                if(switcher==0&&strcmp((*p)->segment,SYM_COMMA))
                {
                    valueq.push((*p)->segment);
                    switcher=1;
                }
                else if(switcher==1&&!strcmp((*p)->segment,SYM_COMMA))
                {
                    switcher=0;
                    continue;
                }
                else
                    return ERROR_WRONG_PARAMETER;

                // (*p)=(*p)->next;
                // if(strcmp((*p)->segment,SYM_COMMA))
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
    else if((action,ACTION_UPDATE))
    {
        if((*p)->segment==EXEC_SET)
        {
            if(
                (*p)->next!=NULL&&
                strcmp((*p)->next->segment,"")&&
                strcmp((*p)->next->segment,SYM_SEMICOLON)&&
                strcmp((*p)->next->segment,SYM_OPEN_PAREN)&&
                strcmp((*p)->next->segment,SYM_CLOSE_PAREN)&&
                ((*p)->next->segment,SYM_EQUAL)
            )
            {
                (*p)=(*p)->next;
                strcat(exec,(*p)->segment);
                
                if(
                    (*p)->next!=NULL&&
                    !strcmp((*p)->next->segment,SYM_EQUAL)
                )
                    (*p)=(*p)->next;
                else
                    return ERROR_WRONG_PARAMETER;

                if(
                    (*p)->next!=NULL&&
                    strcmp((*p)->next->segment,"")&&
                    strcmp((*p)->next->segment,SYM_SEMICOLON)&&
                    strcmp((*p)->next->segment,SYM_OPEN_PAREN)&&
                    strcmp((*p)->next->segment,SYM_CLOSE_PAREN)&&
                    ((*p)->next->segment,SYM_EQUAL)
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
    }else
        return ERROR_WRONG_PARAMETER;
}
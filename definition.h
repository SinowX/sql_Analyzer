#ifndef _DEFINATION
#define _DEFINATION

#define BUF_SZ 256

const char* ACTION_CREATE="create";
const char* ACTION_SHOW="show";
const char* ACTION_INSERT="insert";
const char* ACTION_UPDATE="update";
const char* ACTION_DROP="drop";

const char* PREOBJ_TABLE="table";
const char* PREOBJ_TABLES="tables";
const char* PREOBJ_COLUMNS="columns";
const char* PREOBJ_INDEX="index";
const char* PREOBJ_STATUS="status";
const char* PREOBJ_INTO="into";

const char* CONDI_WHERE="where";
const char* CONDI_FROM="from";

const char* EXEC_SET="set";
const char* EXEC_VALUES="values";

const char* SYM_EQUAL="=";
const char* SYM_SPACE=" ";
const char* SYM_COMMA=",";
const char* SYM_OPEN_PAREN="(";
const char* SYM_CLOSE_PAREN=")";
const char* SYM_SEMICOLON=";";


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

int CurrentWorkDir();
void GetUsername();
void GetHostname();
void set_pack(char *act,char *obj,char *exec,char *condi);
char ctoLower(char c);
int stoLower(char *str);
void Lexical(char *cmd);
seg_node* allocSegNode(seg_node *p);
void freeSegNode();
void trSegNode();
int Yacc();

#endif
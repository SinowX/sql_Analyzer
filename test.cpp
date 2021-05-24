#include<iostream>
#include<string.h>
using namespace std;

int main(){
    char str[]="zhaoqifan";
    // strcmp(str,"sinow");
    if(!strcmp(str,"zhaoqifan")){
        printf("Yes\n");
    }else{
        printf("No\n");
    }
    return 0;
}
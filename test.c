#include "isa.h"
#include "parser.h"
#include <stdio.h>
void check_SearchByName(){
    
    char *names[] = {"data", "ldc", "adc", "ldl", "stl", "ldnl", "stnl", "add", "sub", "shl", "shr", "adj", "a2sp", "sp2a", "call", "return", "brz", "brlz", "br", "HALT", "mul", "div", "SET"};
    struct Instruction output;
    for(int i=0;i<23;i++){
        output=SearchByName(names[i]);
        printf("\n%s",output.name);
        printf("\t%d",output.opcode);
        printf("\t%d",output.status);
    }
}

void check_parse(FILE* fp){
    if(fp==NULL){
        perror("Couldn't open file");return;
    }
    char*** data=parse(fp);
    if(data==NULL) {printf("%s","no data in file");return;}
    

    int i=0,j=0;
    while(data[i]!=NULL){
        j=0;
        while(data[i][j]!=NULL){
            printf("%s\n",data[i][j]);
            j++;
        }
        i++;
        printf("\n");
    }


}

int main(){
    //check_SearchByName();
  

    FILE* fp=fopen("input.txt","r");
    check_parse(fp);
    fclose(fp);
    return 0;
}
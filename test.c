#include "isa.h"
#include "parser.h"
#include <stdio.h>
#include <stdbool.h>
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
    struct LineInfo* array;
    parse(fp,&array);
    if(array==NULL) {printf("%s","no data in file");return;}
    

    int i=0;
    while(array[i].lineNo!=-1){
        //printf("hehe");
        fprintf(stdout,"line no: %d\t label: %s\t mnemonic: %s\t operand: %s\t has_numeric_operand: %d\t numeric_operand %ld\t bad_line: %d\n",array[i].lineNo,array[i].label,array[i].mnemonic,array[i].operand,array[i].has_numeric_operand,array[i].numeric_operand,array[i].bad_line);
        i++;
    }


}

// int main(){
//     //check_SearchByName();
  

//     FILE* fp=fopen("input.txt","r");
//     check_parse(fp);

//     if(fp!=NULL) fclose(fp);
//     return 0;
// }
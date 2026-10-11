#include "isa.h"
#include "parser.h"
#include "symtab.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

void pass1(struct LineInfo* array,int* error){
    int i=0;
    int pc=0;
    int count=0;
    int max_size=1000;
    while(array[i].lineNo!=-1){
        int status=1;
        bool isset=!strcmp(array[i].mnemonic,"SET");
        array[i].address = pc;
        if (array[i].label[0] != '\0'){
             status=add(pc,array[i].label,isset,array[i].lineNo,&count,&max_size,(int)array[i].numeric_operand);
        
        }
        else{
            if(isset) {
                fprintf(stderr,"\nline %d : SET instruction requires a label",array[i].lineNo);
                snprintf(array[i].error_message,sizeof(array[i].error_message),"\nline %d : SET instruction requires a label",array[i].lineNo);
                (*error)++;
            }
        }
        if(!status){
            fprintf(stderr,"\nline %d : duplicate label '%s' (found on line %d)",array[i].lineNo, array[i].label, findLineByName(array[i].label));
            snprintf(array[i].error_message, sizeof(array[i].error_message), "line %d : duplicate label '%s' (found on line %d)", array[i].lineNo, array[i].label, findLineByName(array[i].label));
            (*error)++;
        }
        
        if(strcmp(array[i].mnemonic,"")!=0 && !isset) pc++;
        i++;
    }
}

void pass2(struct LineInfo* array,int* error){

}

int main(){
    FILE* fp=fopen("input.txt","r");
    
    if(fp==NULL){perror("Couldn't open file");return 0;}

    struct LineInfo* array=NULL; 
    int error=0;
    parse(fp,&array,&error);
    if(fp!=NULL) fclose(fp);
    if(array==NULL) {printf("%s","no data in file");return 0;}
    pass1(array,&error);


    return 0;
}




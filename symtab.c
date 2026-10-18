/*
  Author: Adarsh Jha
  Roll No: 2501AI03
  Declaration of Authorship:
  I hereby declare that this code is written by me.
 */
#include <stdlib.h>
#include<stdio.h>
#include <stdbool.h>
#include <string.h>
#include "symtab.h"
#define MAX_SYMBOLS 1000

static struct symbol* symbolTable=NULL;

void init(){
    symbolTable=(struct symbol*)malloc(MAX_SYMBOLS*sizeof(struct symbol));
}
bool add(int pc, char* name,bool is_set,int lineNo,int* count,int* max_size,int mnemonic){
    int foundLine=findLineByName(name);
    if(foundLine!=-1) {
        
        //snprintf(, sizeof(message), "line %d : duplicate label '%s' (found on line %d)", lineNo, name, foundLine);
        return false;
    }
    if(symbolTable==NULL) init();
    
    struct symbol value;
    value.pc = is_set ? mnemonic : pc;
    value.is_set = is_set;
    value.lineNo = lineNo;
    value.used = 0;
    strncpy(value.label, name, sizeof(value.label));value.label[sizeof(value.label) - 1] = '\0';

    if(*count>=(*max_size)-1){
        (*max_size)*=2;
        struct symbol* symbolTable_temp=(struct symbol*)realloc(symbolTable,(*max_size)*sizeof(struct symbol));
        symbolTable=symbolTable_temp;
    }
    symbolTable[*count]=value;
    (*count)++;

    struct symbol stop={-10,"",-1,-1,-1};
    symbolTable[*count]=stop;
    return true;
}

int findLineByName(char* name){
    if(symbolTable==NULL) return -1;
    int i=0;
    while(symbolTable[i].used!=-1){
        if (strcmp(symbolTable[i].label,name)==0) {return symbolTable[i].lineNo;break;}
        i++;
    }
    return -1;
}

int findPcByName(char* name){
    if(symbolTable==NULL) return -10;
    int i=0;
    while(symbolTable[i].used!=-1){
        if (strcmp(symbolTable[i].label,name)==0) {symbolTable[i].used=1;return symbolTable[i].pc;break;}
        i++;
    }
    return -10;
}



char* pcToName(int pc){
    if(symbolTable==NULL) return NULL;
    int i=0;
    while(symbolTable[i].used!=-1){
        if(!symbolTable[i].is_set && symbolTable[i].pc==pc) {return symbolTable[i].label;break;}
        i++;
    }
    return NULL;
}

void symtab_warn_unused(FILE* log_file)
{
    int i=0;
    if(symbolTable==NULL) return;
    while(symbolTable[i].used!=-1){

        if (!symbolTable[i].used)
            fprintf(log_file, "line %d : warning: label '%s' is never used\n",
                    symbolTable[i].lineNo, symbolTable[i].label);
        i++;
    }
}



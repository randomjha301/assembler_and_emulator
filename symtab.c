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
#define MAX_SYMBOLS 1000        //maximum label names that can be there in our table (maximum label count which this system can support)

static struct symbol* symbolTable=NULL;

//initialises the symbolTable
void init(){
    symbolTable=(struct symbol*)malloc(MAX_SYMBOLS*sizeof(struct symbol));
}

//this function adds the label in the symbolTable
bool add(int pc, char* name,bool is_set,int lineNo,int* count,int* max_size,int mnemonic){

    //finding the line number of the label 
    int foundLine=findLineByName(name);
    if(foundLine!=-1) {
        return false;
    }
    if(symbolTable==NULL) init();
    
    //creating the struct symbol which will be added in the symbolTable
    struct symbol value;
    value.pc = is_set ? mnemonic : pc;      //handling the SET label
    value.is_set = is_set;
    value.lineNo = lineNo;
    value.used = 0;
    strncpy(value.label, name, sizeof(value.label));value.label[sizeof(value.label) - 1] = '\0';

    //handling whether if the label size is more than max_size
    if(*count>=(*max_size)-1){
        (*max_size)*=2;
        struct symbol* symbolTable_temp=(struct symbol*)realloc(symbolTable,(*max_size)*sizeof(struct symbol));
        symbolTable=symbolTable_temp;
    }

    //assignign the label to the table
    symbolTable[*count]=value;
    (*count)++;

    //assignign the stop symbol which helps in stopping the traversal loop
    struct symbol stop={-10,"",-1,-1,-1};
    symbolTable[*count]=stop;
    return true;
}


//finds the line number of the label given its name 
int findLineByName(char* name){
    if(symbolTable==NULL) return -1;
    int i=0;
    while(symbolTable[i].used!=-1){
        if (strcmp(symbolTable[i].label,name)==0) {return symbolTable[i].lineNo;break;}
        i++;
    }
    return -1;
}

//finds the pc count of the label given its name
int findPcByName(char* name){
    if(symbolTable==NULL) return -10;
    int i=0;
    while(symbolTable[i].used!=-1){
        if (strcmp(symbolTable[i].label,name)==0) {symbolTable[i].used=1;return symbolTable[i].pc;break;}
        i++;
    }
    return -10;
}


//returns label name give its pc count
char* pcToName(int pc){
    if(symbolTable==NULL) return NULL;
    int i=0;
    while(symbolTable[i].used!=-1){
        if(!symbolTable[i].is_set && symbolTable[i].pc==pc) {return symbolTable[i].label;break;}
        i++;
    }
    return NULL;
}


//function to print all the unused label warnings 
void symtab_warn_unused(FILE* log_file)
{
    int i=0;
    if(symbolTable==NULL) return;
    while(symbolTable[i].used!=-1){

        //using the used variable to decide whether a label is left unused
        if (!symbolTable[i].used)
            fprintf(log_file, "line %d : warning: label '%s' is never used\n",
                    symbolTable[i].lineNo, symbolTable[i].label);
        i++;
    }
}



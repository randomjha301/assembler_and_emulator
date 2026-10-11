#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <errno.h> 
#include "parser.h"
#include "isa.h"
#define MAX_TOKEN_SIZE 32

static bool label_check(char* label,int line_count,struct LineInfo* array){
    char* allowed="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
    if(strlen(label)==0){snprintf(array[line_count].error_message, sizeof(array[line_count].error_message), "line %d : ':' needs to be accompanied with a label name\n",array[line_count].lineNo);return false;}
    if(!isalpha((unsigned char)label[0])) {snprintf(array[line_count].error_message, sizeof(array[line_count].error_message),"line %d : label name must start with a letter\n",array[line_count].lineNo);return false;}
    if(strspn(label, allowed) != strlen(label)) {snprintf(array[line_count].error_message, sizeof(array[line_count].error_message),"line %d : characters other than alphabet or numbers are not permitted in label name\n",array[line_count].lineNo);return false;}
    return true;
}

static struct Instruction mnemonic_check(char* mnemonic,int line_count,struct LineInfo* array){
    struct Instruction result=SearchByName(mnemonic);
    if (strcmp(result.name,"no_such_instruction")==0){snprintf(array[line_count].error_message, sizeof(array[line_count].error_message),"line %d : '%s' instruction not present in instruction set\n",array[line_count].lineNo,mnemonic);}
    return result;
}

static  bool operand_check(char* operand,int line_count,struct LineInfo* array){
    int c=operand[0];
    if(!isalpha(c)){
        char* ptr;
        errno = 0;         
        long int value=strtol(operand,&ptr,0);
        if(*ptr!='\0'){snprintf(array[line_count].error_message, sizeof(array[line_count].error_message),"line %d : '%s' is not a valid operand\n",array[line_count].lineNo,operand);return false;}
        else if(strcmp(array[line_count].mnemonic,"SET")==0 || strcmp(array[line_count].mnemonic,"data")==0){
            if (errno == ERANGE || value != (long)(int)value) {
                snprintf(array[line_count].error_message, sizeof(array[line_count].error_message),"line %d : operand size cannot fit in 32 bits(OVERFLOW)\n",array[line_count].lineNo);
                return false;
            }
        }
        else if(value>8388607 || value<-8388608){snprintf(array[line_count].error_message, sizeof(array[line_count].error_message),"line %d : operand size cannot fit in 24 bits(OVERFLOW)\n",array[line_count].lineNo);return false;}
        array[line_count].has_numeric_operand=true;
        array[line_count].numeric_operand=value;
        return true;
    }
    else{
        char* allowed="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
        if(strspn(operand, allowed) == strlen(operand)){
            array[line_count].has_numeric_operand=false;
            if(strlen(operand)>MAX_TOKEN_SIZE-1){
                snprintf(array[line_count].error_message, sizeof(array[line_count].error_message),"line %d : operand cannot be greater than 32 characters,accepting first 32 characters as the operand name\n",array[line_count].lineNo);
                strncpy((array)[line_count].operand,operand,MAX_TOKEN_SIZE-1);
                array[line_count].operand[MAX_TOKEN_SIZE-1]='\0';
                return false;
            }
            else{
                strcpy((array)[line_count].operand,operand);
                return true;
            }
        }
        else{
            snprintf(array[line_count].error_message, sizeof(array[line_count].error_message),"line %d : invalid operand format,needs to be either an word or a number\n",line_count);
            array[line_count].bad_line=true;return false;
        }
    }

    
    
}

static  struct LineInfo* initialise(int line_count){
    struct LineInfo* result=(struct LineInfo*)malloc(sizeof(struct LineInfo));
    result->lineNo=line_count;
    result->address=0;
    result->machine_code=0;
    strcpy(result->lst_text,"");
    result->has_machine_code=false;
    result->numeric_operand=-1;
    result->bad_line=false;
    result->has_numeric_operand=false;
    strcpy(result->label,"");
    strcpy(result->mnemonic,"");
    strcpy(result->operand,"");
    result->error_message[0]='\0'; 
    return result;

}

void report_error(struct LineInfo *cur, int* error,int line_no){
    int i=0;
    while(cur[i].lineNo!=-1){
        if(cur[i].error_message[0]=='\0') {i++;continue;}
        if (cur[i].bad_line) continue;       
        fprintf(stderr, "line %d : %s\n", line_no, cur[i].error_message);
        cur[i].bad_line = true;
        (*error)++;i++;
    }
}


void parse(FILE* fp,struct LineInfo** lines,int* error){
    
    *lines=NULL;
    if(fp==NULL) {return ;}
    int line_count=0;
    
    int line_capacity=256;
    
    struct LineInfo* array=(struct LineInfo* )malloc((line_capacity)*sizeof(struct LineInfo));
    
    char* line=(char*)malloc(256*sizeof(char));
    int line_no=1;
    

    while(fgets(line,256,fp)!=NULL){
        
        if(line_count>=(line_capacity-1)){
                (line_capacity)*=2;
                struct LineInfo*  array_temp=(struct LineInfo* )realloc(array,(line_capacity)*sizeof(struct LineInfo));
                array=array_temp;
        }
        

        char* semicolon=strchr(line,';');
        if(semicolon!=NULL) *semicolon='\0';
        if(strlen(line)==0) {line_no++;continue;}

        char* p=line;
        while (isspace((unsigned char)*p)) {
            p++;
        }
        if(strlen(p)==0){line_no++;continue;}

        array[line_count]=*initialise(line_no);
        //assigning labels and checking for their validity
        char* colon=strchr(p,':');
        char* rest;
        if(colon!=NULL){
            *colon='\0';
            if(label_check(p,line_count,array)){
                if(strlen(p)>MAX_TOKEN_SIZE-1){
                    snprintf(array[line_count].error_message, sizeof(array[line_count].error_message),"line %d : label cannot be greater than 32 characters,accepting first 32 characters as the label name\n",line_no);
                    strncpy((array)[line_count].label,line,MAX_TOKEN_SIZE-1);
                    (array)[line_count].label[MAX_TOKEN_SIZE-1]='\0';
                    
                }
                else{
                    strcpy((array)[line_count].label,line);
                }
            }
            rest=colon+1;
        }
        else{
            rest=line;
        }

        
        char* token=strtok(rest," \t\r\n");
        int count=0;
        struct Instruction result;
        while(token!=NULL){
            if (count==0){
                result=mnemonic_check(token,line_count,array);
                if(strcmp(result.name,"no_such_instruction")!=0){
                    strcpy(array[line_count].mnemonic,result.name);
                }
                else{
                    break;
                }
            }
            else if(count==1){
                if(result.status==NO_OPERAND){
                    snprintf(array[line_count].error_message, sizeof(array[line_count].error_message),"line %d : this instruction doesnt accept an operand\n",line_no);break;
                }
                if(!operand_check(token,line_count,array)){break;}}
            else {
                snprintf(array[line_count].error_message, sizeof(array[line_count].error_message),"line %d : no more than 1 operand is acceptable\n",line_no);break;
            }

      
            token=strtok(NULL," \t\r\n");
            count++;
        }
        if(strcmp(array[line_count].error_message,"")==0){
            if(count==1){
                if(result.status!=NO_OPERAND){
                    snprintf(array[line_count].error_message, sizeof(array[line_count].error_message),"line %d : operand not provided\n",line_no);array[line_count].has_numeric_operand=false;
                }
            }
        }
        array[line_count].lineNo=line_no;line_no++;
        line_count++;
    }
  
    

    
     if(line_count==0) {free(array);return ;} 
    array[line_count]=*initialise(-1);
    *lines=array;
}
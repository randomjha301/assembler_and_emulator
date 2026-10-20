/*
  Author: Adarsh Jha
  Roll No: 2501AI03
  Declaration of Authorship:
  I hereby declare that this code is written by me.
 */
#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <errno.h> 
#include "parser.h"
#include "isa.h"
#define MAX_TOKEN_SIZE 32

//checks if a label is according to the rules or not
static bool label_check(char* label,int line_count,struct LineInfo* array){
    const char* allowed="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
    //raises error if : is written without a label name 
    if(strlen(label)==0){snprintf(array[line_count].error_message, sizeof(array[line_count].error_message), "line %d : ':' needs to be accompanied with a label name",array[line_count].lineNo);return false;}
    //raises error if label name starign from something else other than letter
    if(!isalpha((unsigned char)label[0])) {snprintf(array[line_count].error_message, sizeof(array[line_count].error_message),"line %d : label name must start with a letter",array[line_count].lineNo);return false;}
    //raises error if if characters other than alphanumeric char are used 
    if(strspn(label, allowed) != strlen(label)) {snprintf(array[line_count].error_message, sizeof(array[line_count].error_message),"line %d : characters other than alphabet or numbers are not permitted in label name",array[line_count].lineNo);return false;}
    return true;
}

//checks if a mnemonic is according to the rules or not
static struct Instruction mnemonic_check(char* mnemonic,int line_count,struct LineInfo* array){
    struct Instruction result=SearchByName(mnemonic);
    //raises error if instruction not present in instruciton set
    if (strcmp(result.name,"no_such_instruction")==0){snprintf(array[line_count].error_message, sizeof(array[line_count].error_message),"line %d : '%s' instruction not present in instruction set",array[line_count].lineNo,mnemonic);}
    return result;
}

//checks if a operand is according to the rules or not
static  bool operand_check(char* operand,int line_count,struct LineInfo* array){
    int c=(unsigned char)operand[0];
    //if the operand is numeric
    if(!isalpha(c)){
        char* ptr;
        errno = 0;         
        long int value=strtol(operand,&ptr,0);
        //raises error if nnumeric operand is not fully numeric 
        if(*ptr!='\0'){snprintf(array[line_count].error_message, sizeof(array[line_count].error_message),"line %d : '%s' is not a valid operand",array[line_count].lineNo,operand);return false;}
        
        else if(strcmp(array[line_count].mnemonic,"SET")==0 || strcmp(array[line_count].mnemonic,"data")==0){
            //raises error if operand size is vvery large for SET command
            if (errno == ERANGE || value != (long)(int)value) {
                snprintf(array[line_count].error_message, sizeof(array[line_count].error_message),"line %d : operand size cannot fit in 32 bits(OVERFLOW)",array[line_count].lineNo);
                return false;
            }
        }
        //raises error if ooperand size is large (overflow)
        else if(value>8388607 || value<-8388608){snprintf(array[line_count].error_message, sizeof(array[line_count].error_message),"line %d : operand size cannot fit in 24 bits(OVERFLOW)",array[line_count].lineNo);return false;}
        
        array[line_count].has_numeric_operand=true;
        array[line_count].numeric_operand=value;
        return true;
    }
    //if a label is presnet as an operand 
    else{
        const char* allowed="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
        if(strspn(operand, allowed) == strlen(operand)){
            array[line_count].has_numeric_operand=false;
            if(strlen(operand)>MAX_TOKEN_SIZE-1){
                //raises error if label name is very large , accepting first 32 char as operand name
                snprintf(array[line_count].error_message, sizeof(array[line_count].error_message),"line %d : operand cannot be greater than 32 characters,accepting first 32 characters as the operand name",array[line_count].lineNo);
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
            //raises error if netiher a word nor a numebr is provided 
            snprintf(array[line_count].error_message, sizeof(array[line_count].error_message),"line %d : invalid operand format,needs to be either an word or a number",array[line_count].lineNo);
            return false;
        }
    }

    
    
}


//initialises a Lineinfo struct
static  struct LineInfo* initialise(int line_count){
    struct LineInfo* result=(struct LineInfo*)malloc(sizeof(struct LineInfo));
    result->lineNo=line_count;
    result->address=0;
    result->machine_code=0;
    result->write_to_bin=false;
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

//writes all the errors by seeing all error_messages in the LineInfo array
void report_error(struct LineInfo *cur, int* error,FILE* log_file){
    int i=0;
    while(cur[i].lineNo!=-1){
        if(cur[i].error_message[0]!='\0'){
            fprintf(log_file, "%s\n",cur[i].error_message);
            cur[i].bad_line = true;
            (*error)++;
        }
        i++;
    }
}


//fucntion which parser the document and writes the lines in the lines array
void parse(FILE* fp,struct LineInfo** lines){
    
    *lines=NULL;
    if(fp==NULL) {return ;}
    int line_count=0;
    
   
    int line_capacity=256;
    
    struct LineInfo* array=(struct LineInfo* )malloc((line_capacity)*sizeof(struct LineInfo));
    
    char* line=(char*)malloc(256*sizeof(char));
    if(array==NULL || line==NULL){free(array);free(line);return;}
    int line_no=1;
    

    while(fgets(line,256,fp)!=NULL){
        //increasing the array size by double if max size is reached 
        if(line_count>=(line_capacity-1)){
                (line_capacity)*=2;
                struct LineInfo*  array_temp=(struct LineInfo* )realloc(array,(line_capacity)*sizeof(struct LineInfo));
                if(array_temp==NULL){free(array);free(line);return;} 
                array=array_temp;
        }
        
        //stripping out all the comments
        char* semicolon=strchr(line,';');
        if(semicolon!=NULL) *semicolon='\0';
        if(strlen(line)==0) {line_no++;continue;}

        char* p=line;
        while (isspace((unsigned char)*p)) {
            p++;
        }
        if(strlen(p)==0){line_no++;continue;}

        struct LineInfo* fresh=initialise(line_no);
        array[line_count]=*fresh;
        free(fresh);
        
        //assigning labels and checking for their validity
        char* colon=strchr(p,':');
        char* rest;
        if(colon!=NULL){
            *colon='\0';
            if(label_check(p,line_count,array)){
                //raises error if label size is more than 32 char
                if(strlen(p)>MAX_TOKEN_SIZE-1){
                    snprintf(array[line_count].error_message, sizeof(array[line_count].error_message),"line %d : label cannot be greater than 32 characters,accepting first 32 characters as the label name",line_no);
                    strncpy((array)[line_count].label,p,MAX_TOKEN_SIZE-1);
                    (array)[line_count].label[MAX_TOKEN_SIZE-1]='\0';
                    
                }
                else{
                    strcpy((array)[line_count].label,p);
                }
            }
            rest=colon+1;
        }
        else{
            rest=p;
        }

        
        char* token=strtok(rest," \t\r\n");
        int count=0;
        struct Instruction result={"",0,NO_OPERAND};

        //converting each line to tokens and analysing each token one by one
        while(token!=NULL){
            if (count==0){
                //first value will be mnemonic 
                result=mnemonic_check(token,line_count,array);
                if(strcmp(result.name,"no_such_instruction")!=0){
                    strcpy(array[line_count].mnemonic,result.name);
                }
                else{
                    break;
                }
            }
            else if(count==1){
                //seconod will be operand
                if(result.status==NO_OPERAND){
                    snprintf(array[line_count].error_message, sizeof(array[line_count].error_message),"line %d : this instruction doesnt accept an operand",line_no);break;
                }
                if(!operand_check(token,line_count,array)){break;}}
            else {
                snprintf(array[line_count].error_message, sizeof(array[line_count].error_message),"line %d : no more than 1 operand is acceptable",line_no);break;
            }

      
            token=strtok(NULL," \t\r\n");
            count++;
        }

        //handling operand not provided case
        if(strcmp(array[line_count].error_message,"")==0){
            if(count==1){
                if(result.status!=NO_OPERAND){
                    snprintf(array[line_count].error_message, sizeof(array[line_count].error_message),"line %d : operand not provided",line_no);array[line_count].has_numeric_operand=false;
                }
            }
        }
        array[line_count].lineNo=line_no;line_no++;
        line_count++;
    }
  
    
    free(line);
    
    if(line_count==0) {free(array);return ;} 
    struct LineInfo* stop=initialise(-1);
    array[line_count]=*stop;
    free(stop);
    *lines=array;
}
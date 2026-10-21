/*
  Author: Adarsh Jha
  Roll No: 2501AI03
  Declaration of Authorship:
  I hereby declare that this code is written by me.
 */
#include "isa.h"
#include "parser.h"
#include "symtab.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>    
#include <stdbool.h>

//pass 1
void pass1(struct LineInfo* array){
    int i=0;
    int pc=0;
    int count=0; //
    int max_size=1000;

    //traversing through the array containing the lines of assembly file
    while(array[i].lineNo!=-1){
        int status=1;
        bool isset=!strcmp(array[i].mnemonic,"SET");
        array[i].address = pc;
        if (array[i].label[0] != '\0'){
             // adding the label in the symbolTable
             status=add(pc,array[i].label,isset,array[i].lineNo,&count,&max_size,(int)array[i].numeric_operand);
        
        }
        else{
            if(isset) {
                //raising error if SET written without a label
                snprintf(array[i].error_message,sizeof(array[i].error_message),"line %d : SET instruction requires a label",array[i].lineNo);
            }
        }
        if(!status && array[i].error_message[0]=='\0'){
            //raising an error if a duplicate label is found
            snprintf(array[i].error_message, sizeof(array[i].error_message), "line %d : duplicate label '%s' (found on line %d)", array[i].lineNo, array[i].label, findLineByName(array[i].label));
        }
        if(isset && array[i].error_message[0]=='\0' && !array[i].has_numeric_operand){
            //raising an error if set doesnt get a numeric value
            snprintf(array[i].error_message, sizeof(array[i].error_message), "line %d : SET needs a numeric value",array[i].lineNo);
        }
        
        //increasing the pc count if a mnemonic is present 
        if(strcmp(array[i].mnemonic,"")!=0 && !isset) pc++;
        i++;
    }
}

//pass 2
void pass2(struct LineInfo* array){
    int i=0;

    //traversing through the array containing the lines of assembly file
    while(array[i].lineNo!=-1){
        //not converting the line into machine or list code if there is any error in it 
        if(array[i].error_message[0]!='\0'){i++;continue;}

        //if mnemonic is not there it is jsut a label so just write it to list file and no machine code will be generated 
        if(strcmp(array[i].mnemonic,"")==0){
            snprintf(array[i].lst_text,sizeof(array[i].lst_text),
            "%08X %s:",array[i].address,array[i].label);
            array[i].write_to_bin=false;
        }
        //if mnemnoic is SET , just write it to list file and no machine code will be generated 
        else if(strcmp(array[i].mnemonic,"SET")==0){
            snprintf(array[i].lst_text,sizeof(array[i].lst_text),
            "%08X %s:\n%08X          SET %ld",array[i].address,array[i].label,array[i].address,array[i].numeric_operand);
            array[i].write_to_bin=false;
        }
        //if mnemoic is data , it will be written to both mchiane code and list file
        else if(strcmp(array[i].mnemonic, "data") == 0){
            //case when label is there with data mnemoinc 
            if(strcmp(array[i].label,"")!=0){
                //handling numeric operand
                if(array[i].has_numeric_operand){
                    array[i].machine_code=array[i].numeric_operand;
                    snprintf(array[i].lst_text,sizeof(array[i].lst_text),
                    "%08X %s:\n%08X %08X data %ld",array[i].address,array[i].label,array[i].address,array[i].machine_code,array[i].numeric_operand);
                    
                    array[i].write_to_bin=true;
                }
                //handling non numeric opearand
                else{
                     int ans=findPcByName(array[i].operand);
                     //label not present ,raise error
                    if(ans==-10){
                        array[i].bad_line=true;
                        snprintf(array[i].error_message, sizeof(array[i].error_message),"line %d : Undefined label '%s'",array[i].lineNo,array[i].operand);}
                    //wirting to list file and machine code generating
                    else{
                        array[i].machine_code=ans;
                        snprintf(array[i].lst_text,sizeof(array[i].lst_text),
                        "%08X %s:\n%08X %08X data %s",array[i].address,array[i].label,array[i].address,array[i].machine_code,array[i].operand);
                        array[i].write_to_bin=true;
                       
                    }
                    
                }
            }
            //case when label is not there in data mnemonci
            else{
                //handling numeric operand
                if(array[i].has_numeric_operand){
                    array[i].machine_code=array[i].numeric_operand;
                    snprintf(array[i].lst_text,sizeof(array[i].lst_text),
                    "%08X %08X data %ld",array[i].address,array[i].machine_code,array[i].numeric_operand);
                    
                    array[i].write_to_bin=true;
                }
                //handling non numeric opearand
                else{
                     int ans=findPcByName(array[i].operand);
                     //label not present ,raise error
                    if(ans==-10){
                        array[i].bad_line=true;
                        snprintf(array[i].error_message, sizeof(array[i].error_message),"line %d : Undefined label '%s'",array[i].lineNo,array[i].operand); }
                    //wirting to list file and machine code generating
                    else{
                        array[i].machine_code=ans;
                        snprintf(array[i].lst_text,sizeof(array[i].lst_text),
                        "%08X %08X data %s",array[i].address,array[i].machine_code,array[i].operand);
                        array[i].write_to_bin=true;
                       
                    }
                    
                }
            }
            
           
        }
        //handling the case when mnemonic is there other than data or SET
        else{
            unsigned int operand_value=0;
            unsigned int opcode=0;
            char opbuf[40];     
            //searching for instruction struct by mnemonic name
            struct Instruction result=SearchByName(array[i].mnemonic);
            opcode=result.opcode;
            if(array[i].has_numeric_operand) snprintf(opbuf,sizeof(opbuf),"%ld",array[i].numeric_operand);
            else strcpy(opbuf,array[i].operand);  
            
            //handling case when it needs  value operand or offset operand 
            if(result.status!=NO_OPERAND){
                //numeric operand 
                if(array[i].has_numeric_operand) operand_value=array[i].numeric_operand;
                else{
                    //label operand 
                    int target=findPcByName(array[i].operand);
                    if(target==-10){
                        //lable not present , raise error
                        array[i].bad_line=true;
                        snprintf(array[i].error_message, sizeof(array[i].error_message),"line %d : Undefined label '%s'",array[i].lineNo,array[i].operand); }
                    else{
                        //label present ,offset operand case
                        if(result.status==OFFSET_OPERAND) operand_value=target-(array[i].address+1);
                        else operand_value=target;
                        // snprintf(array[i].lst_text,sizeof(array[i].lst_text),
                        // "%08X %s:\n%08X %08X %s %s",array[i].address,array[i].label,array[i].address,array[i].machine_code,array[i].mnemonic,array[i].operand);
                    }
                    
                }
            }

            //generating maachine code 
            array[i].machine_code=((operand_value & 0xFFFFFF) << 8) | (opcode & 0xFF);
            array[i].write_to_bin=true;

            //handling differnet outputs if label is there and if it is not for the list file
            if(strcmp(array[i].label,"")!=0){
                 snprintf(array[i].lst_text,sizeof(array[i].lst_text),
                        "%08X %s:\n%08X %08X %s %s",array[i].address,array[i].label,array[i].address,array[i].machine_code,array[i].mnemonic,opbuf);
                    
            }
            else{
                 snprintf(array[i].lst_text,sizeof(array[i].lst_text),
                        "%08X %08X %s %s",array[i].address,array[i].machine_code,array[i].mnemonic,opbuf);
                    
            }
        }
        i++;
    }
}


//this function writes the machine code in binary file provided and listing code in listing file provided
void write_file(const char *bin_filename,const char *lst_filename,struct LineInfo* array) {
    FILE *bin_file = fopen(bin_filename, "wb"); 
    FILE *lst_file = fopen(lst_filename, "w");  

    //handling file no operning case
    if(bin_file==NULL || lst_file==NULL){
        fprintf(stderr,"Couldn't open output files\n");
        if(bin_file!=NULL) fclose(bin_file);
        if(lst_file!=NULL) fclose(lst_file);
        return;
    }

    int i=0;
    //writing in the files
    while(array[i].lineNo!=-1){
        fprintf(lst_file, "%s\n", array[i].lst_text);
        if (array[i].write_to_bin) {
            fwrite(&array[i].machine_code, sizeof(unsigned int), 1, bin_file);
        }
        i++;
    }

    //closing the files
    fclose(bin_file);
    fclose(lst_file);
}

int main(int argc,char* argv[]){
    char* input_file=argv[1];

    //handling wrong command in terminal case
    if (argc < 2) {
        printf("Error: Missing file name.\n");
        printf("Usage: %s <filename>\n", argv[0]);
        return 1; 
    }

    //creating log_file name
    char* log=(char*)(malloc(strlen(input_file)+1));
    strcpy(log,input_file);
    memcpy(log + strlen(input_file) - 3, "log", 3);

    

    //creating binary file name 
    char *binary = malloc(strlen(input_file) );
    if (binary != NULL) {
        strncpy(binary, input_file, strlen(input_file) - 3);
        binary[strlen(input_file) - 3] = '\0';
        strcat(binary, "o");
    }

    //creating listing file name
    char* lst=(char*)(malloc(strlen(input_file)+1));
    strcpy(lst,input_file);
    memcpy(lst + strlen(input_file)-3, "lst", strlen("lst"));

    FILE* fp=fopen(input_file,"r");
    FILE* log_file=fopen(log,"w");
    const char* bin_file=binary;
    const char* lst_file=lst;

    //handling file not opening case
    if(log_file==NULL){perror("Couldn't open result.log");if(fp!=NULL) fclose(fp);return 1;}
    if(fp==NULL){fprintf(log_file,"Couldn't open file");return 0;}

    struct LineInfo* array=NULL; 
    int error=0;

    //parsing the asm file
    parse(fp,&array);
    if(fp!=NULL) fclose(fp);
    if(array==NULL) {fprintf(log_file,"%s","no data in file");return 0;}

    //passing through both the passes
    pass1(array);
    pass2(array);

    //writning the warnings if any
    fprintf(log_file,"WARNINGS\n");
    symtab_warn_unused(log_file);
    //warning for a 'br' that jumps to itself (offset -1 = 0xFFFFFF11), it can never stop
    for(int w=0;array[w].lineNo!=-1;w++){
        if(array[w].write_to_bin && array[w].machine_code==0xFFFFFF11u)
            fprintf(log_file,"line %d : warning: possible infinite loop ('br' jumps to itself)\n",array[w].lineNo);
    }
    //writing the errors if any in the log file
    fprintf(log_file,"ERRORS\n");
    report_error(array,&error,log_file);        
    fprintf(log_file,"%d error(s)\n",error);

    if(error>0){
        free(array);
        fclose(log_file);
        return 1;                                
    }

    //finally writing to the binary file and lst file if no errors are present 
    write_file(bin_file,lst_file,array);       
    free(array);                                
    fclose(log_file);
    return 0;
}




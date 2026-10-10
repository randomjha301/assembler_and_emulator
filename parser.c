#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"



char*** parse(FILE* fp){
    if(fp==NULL) {return NULL;}
    int line_count=0;
    
    int line_capacity=256;
    
    char*** data=(char***)malloc((line_capacity)*sizeof(char**));
    
    char* line=(char*)malloc(256*sizeof(char));

    

    while(fgets(line,256,fp)!=NULL){
        if(line_count>=(line_capacity-1)){
                (line_capacity)*=2;
                char*** data_temp=(char***)realloc(data,(line_capacity)*sizeof(char**));
                data=data_temp;
        }

        int count=0;
        int capacity=256;

        char*p=strchr(line,';');
        if(p!=NULL) *p='\0';
        if(strlen(line)==0) continue;
        
        char* token=strtok(line," \t\n");
        char** one_line=(char**)malloc((capacity)*sizeof(char*));
        
        
        while(token!=NULL){
            if(count>=(capacity-1)){
                (capacity)*=2;
                char** temp=(char**)realloc(one_line,(capacity)*sizeof(char*));
                one_line=temp;
            }

            
            one_line[count]=strdup(token);
            token=strtok(NULL," \t\n");
            count++;

        }
        one_line[count]=NULL;
        
        
        data[line_count]=one_line;
        line_count++;
    }
    data[line_count]=NULL;
    

    
    if(line_count==0) return NULL;
    return data;
}
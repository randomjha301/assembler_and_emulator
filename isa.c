/*
  Author: Adarsh Jha
  Roll No: 2501AI03
  Declaration of Authorship:
  I hereby declare that this code is written by me.
 */
#include "isa.h"
#include <string.h>
#include <ctype.h>


//this is the instruction struct which stores information of all the mnemonic
static const struct Instruction Instructions[] = {
    {"data",-1,VALUE_OPERAND},  
    {"ldc",0,VALUE_OPERAND},
    {"adc",1,VALUE_OPERAND},
    {"ldl",2,VALUE_OPERAND},
    {"stl",3,VALUE_OPERAND},
    {"ldnl",4,VALUE_OPERAND},
    {"stnl",5,VALUE_OPERAND},
    {"add",6,NO_OPERAND},
    {"sub",7,NO_OPERAND},
    {"shl",8,NO_OPERAND},
    {"shr",9,NO_OPERAND},
    {"adj",10,VALUE_OPERAND},
    {"a2sp",11,NO_OPERAND},
    {"sp2a",12,NO_OPERAND},
    {"call",13,OFFSET_OPERAND},
    {"return",14,NO_OPERAND},
    {"brz",15,OFFSET_OPERAND},
    {"brlz",16,OFFSET_OPERAND},
    {"br",17,OFFSET_OPERAND},
    {"HALT",18,NO_OPERAND},
    {"mul",19,NO_OPERAND}, 
    {"div",20,NO_OPERAND}, 
    {"SET",-2,VALUE_OPERAND}  
};


//returns a struct Instruction taking the name of mnemonic as input
struct Instruction SearchByName(const char* name){
    int arraySize=sizeof(Instructions)/sizeof(Instructions[0]);
    for(int i=0;i<arraySize;i++){
        const char* a=Instructions[i].name; const char* b=name;
        while(*a && tolower((unsigned char)*a)==tolower((unsigned char)*b)){a++;b++;}
        if(*a=='\0' && *b=='\0') return Instructions[i];
    }

    struct Instruction no_instruction={"no_such_instruction",-3,OFFSET_OPERAND};

    return no_instruction;
}
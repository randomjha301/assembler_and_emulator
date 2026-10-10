#include "isa.h"
#include <string.h>

static const struct Instruction Instructions[] = {
    {"data",-1,VALUE_OPERAND},  
    {"ldc",0,VALUE_OPERAND},
    {"adc",1,VALUE_OPERAND},
    {"ldl",2,OFFSET_OPERAND},
    {"stl",3,OFFSET_OPERAND},
    {"ldnl",4,OFFSET_OPERAND},
    {"stnl",5,OFFSET_OPERAND},
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

struct Instruction SearchByName(char* name){
    int arraySize=sizeof(Instructions)/sizeof(Instructions[0]);
    for(int i=0;i<arraySize;i++){
        if(strcmp(Instructions[i].name,name)==0) return Instructions[i];
    }

    struct Instruction no_instruction={"no_such_instruction",-3,OFFSET_OPERAND};

    return no_instruction;
}
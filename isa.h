#ifndef ISA_H
#define ISA_H


enum opcode_status {NO_OPERAND,VALUE_OPERAND,OFFSET_OPERAND};

struct Instruction{
    char* name;
    int opcode;
    enum opcode_status status;
};

struct Instruction SearchByName(char* );

#endif
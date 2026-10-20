/*
  Author: Adarsh Jha
  Roll No: 2501AI03
  Declaration of Authorship:
  I hereby declare that this code is written by me.
 */

#ifndef ISA_H
#define ISA_H

//three types of operand status
enum opcode_status {NO_OPERAND,VALUE_OPERAND,OFFSET_OPERAND};

//the data in an instruction 
struct Instruction{
    const char* name;
    int opcode;
    enum opcode_status status;
};

//returns a struct Instruction taking the name of mnemonic as input
struct Instruction SearchByName(const char* );

#endif
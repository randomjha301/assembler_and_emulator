#include <stdio.h>
#include <stdbool.h>
#ifndef PARSER_H
#define PARSER_H

struct LineInfo{
    int lineNo;
    int address;
    unsigned int machine_code;
    char lst_text[128];
    int has_machine_code;
    char label[32];
    char mnemonic[32];
    char operand[32];
    bool has_numeric_operand;
    long int numeric_operand;
    bool bad_line;
    char error_message[1000];
};
void parse(FILE*,struct LineInfo**,int*);
void report_error(struct LineInfo *, int* , int );

#endif

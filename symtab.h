#include <stdbool.h>
#include <stdio.h>
#ifndef SYMTAB_H
#define SYMTAB_H

struct symbol{
    int pc;
    char label[32];
    bool is_set;
    int lineNo;
    int used;
};


void init(void);
bool add(int , char* ,bool ,int ,int* ,int* ,int);
int findLineByName(char*);
int findPcByName(char*);
char* pcToName(int);
void symtab_warn_unused(FILE*);


#endif
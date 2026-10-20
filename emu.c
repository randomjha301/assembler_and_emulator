/*
  Author: Adarsh Jha
  Roll No: 2501AI03
  Declaration of Authorship:
  I hereby declare that this code is written by me.
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#define MAX_STEPS 1000000   //maximum instructions executed before we assume an infinite loop

//created an array to help in writing executinf statements
const char *mnemonics[] = {
    "ldc", "adc", "ldl", "stl", "ldnl", "stnl", "add", "sub", 
    "shl", "shr", "adj", "a2sp", "sp2a", "call", "return", 
    "brz", "brlz", "br", "HALT", "mul", "div"
};

//memory array
int memory[65536]={0};
//Register A and B and stack pointer register
int A=0,B=0,sp=0;

int main(int argc,char* argv[]){
    int PC=0;
    bool is_running = true;
    long steps=0;

    char* input_file=argv[1];
    //handling wrong command in terminal case
    if (argc < 2) {
        printf("Error: Missing file name.\n");
        printf("Usage: %s <filename>\n", argv[0]);
        return 1; 
    }

    //creating log file
    int len = strlen(input_file);
    char* log=malloc(len+6+1);
    strncpy(log,input_file,len-2);
    log[len - 2] = '\0';
    strcat(log, "_emu.log");
    FILE* log_file = fopen(log, "w");
    if (log_file == NULL) {
        printf("Error: Could not create emu.log\n");
        return 1;
    }

    //creating binary file
    FILE* bin_file = fopen(input_file, "rb");
    if (bin_file == NULL) {
        printf("Error: Could not open output.o for reading\n");
        fclose(log_file);
        return 1;
    }
    fread(memory, sizeof(int), 65536, bin_file);
    fclose(bin_file);

    //executing the instructions
    while (is_running) {
        if(++steps>MAX_STEPS){fprintf(log_file,"Emulator Error: more than %d steps executed, possible infinite loop\n",MAX_STEPS);break;}
        //if pc exceeds the limits ,raise error
        if (PC < 0 || PC >= 65536) {
            fprintf(log_file, "Emulator Error: PC out of bounds (%d)\n", PC);
            break;
        }

        //decoding the operand and operand from the instruction
        int instruction = memory[PC];
        int opcode = instruction & 0xFF;
        int operand = instruction >> 8;
        if (operand & 0x00800000) {
            operand |= 0xFF000000;
        }
        PC++;
        //'br' with offset -1 jumps to itself forever, stop immediately
        if(opcode==17 && operand==-1){fprintf(log_file,"Emulator Error: infinite loop detected ('br' jumps to itself) at PC = %d\n",PC-1);break;}

        //writing the executing statements accordingly 
        if (opcode >= 0 && opcode <= 20) {
            if ((opcode >= 6 && opcode <= 9) || (opcode == 11) || (opcode == 12) || opcode == 14 || (opcode >= 18 && opcode <= 20)) { fprintf(log_file,"PC: %08X | A: %08X | B: %08X | SP: %08X | Executing: %s\n", PC, A, B, sp, mnemonics[opcode]);} 
            else {fprintf(log_file,"PC: %08X | A: %08X | B: %08X | SP: %08X | Executing: %s %d\n",PC, A, B, sp, mnemonics[opcode], operand);}
        } 
        else fprintf(log_file,"PC: %08X | A: %08X | B: %08X | SP: %08X | Executing: DATA (0x%08X)\n", PC, A, B, sp, instruction);
        
        //handling all the differnet operations
        switch (opcode) {
            case 0:  /* ldc: Load Constant */
                B = A;
                A = operand;
                break;
                
            case 1:  /* adc: Add Constant */
                A = A + operand;
                break;
                
            case 2:  /* ldl: Load Local */
                if(sp+operand>=65536 ||  sp+operand<0 ) {fprintf(log_file, "(Segmentation fault)Memory access out of bounds\n");is_running=false;break;}
                B = A;
                A = memory[sp + operand];
                break;
                
            case 3:  /* stl: Store Local */
                if(sp+operand>=65536 || sp+operand<0) {fprintf(log_file, "(Segmentation fault)Memory access out of bounds\n");is_running=false;break;}
              
                memory[sp + operand] = A;
                A = B;
                break;
                
            case 4:  /* ldnl: Load Non-Local */
                if(A+operand>=65536 || A+operand<0) {fprintf(log_file, "(Segmentation fault)Memory access out of bounds\n");is_running=false;break;}
                
                A = memory[A + operand];
                break;
                
            case 5:  /* stnl: Store Non-Local */
                if(A+operand>=65536 || A+operand<0) {fprintf(log_file, "(Segmentation fault)Memory access out of bounds\n");is_running=false;break;}
                memory[A + operand] = B;
                break;
                
            case 6:  /* add */
                A = B + A;
                break;
                
            case 7:  /* sub */
                A = B - A;
                break;
                
            case 8:  /* shl: Shift Left */
                A = B << A;
                break;
                
            case 9:  /* shr: Shift Right */
                A = B >> A;
                break;
                
            case 10: /* adj: Adjust Stack Pointer */
                sp = sp + operand;
                break;
                
            case 11: /* a2sp: Transfer A to sp */
                sp = A;
                A = B;
                break;
                
            case 12: /* sp2a: Transfer sp to A */
                B = A;
                A = sp;
                break;
                
            case 13: /* call */
                B = A;
                A = PC;
                PC = PC + operand;
                break;
                
            case 14: /* return */
                PC = A;
                A = B;
                break;
                
            case 15: /* brz: Branch if Zero */
                if (A == 0) {
                    PC = PC + operand;
                }
                break;
                
            case 16: /* brlz: Branch if Less than Zero */
                if (A < 0) {
                    PC = PC + operand;
                }
                break;
                
            case 17: /* br: Unconditional Branch */
                PC = PC + operand;
                break;
                
            case 18: /* HALT */
                is_running = false;
                break;
                
            case 19: /* mul */
                A = B * A;
                break;
                
            case 20: /* div */
                if (A == 0) {fprintf(log_file, "Emulator Error: Division by zero at PC = %d\n", PC - 1);is_running = false; } 
                else A = B / A;
                break;
                
            default:
                fprintf(log_file, "Emulator Error: Unknown opcode %d at PC = %d\n", opcode, PC - 1);
                is_running = false;
                break;
        }
    }

    //writing final memory dump and register states
    fprintf(log_file,"\n--Final Register state--\nPC: %0X\nSP: %0X\nA: %0X\nB: %0X\n",PC,sp,A,B);
    fprintf(log_file,"\nFinal Memory Dump (Non-zero addresses):\n");
    for (int i = 0; i < 65536; i++) {
        if (memory[i] != 0) {
            fprintf(log_file,"[%08X] : %08X\n", i, memory[i]);
        }
    }
    fclose(log_file);
}
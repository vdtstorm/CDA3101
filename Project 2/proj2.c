#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

int convertNum(int num);
void printState(stateType *);

int
main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc != 2) {
	printf("error: usage: %s <machine-code file>\n", argv[0]);
	exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
	printf("error: can't open file %s", argv[1]);
	perror("fopen");
	exit(1);
    }

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
	state.numMemory++) {
	if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
	    printf("error in reading address %d\n", state.numMemory);
	    exit(1);
	}
	printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }

	// Simulation Code
	// add for loop
	state.pc = 0;
	int count = 0;
	int temp  = 0;
	int offset = 0;
	bool beq = false;
	for(int i = 0; i< 8; i++)
        	state.reg[i] = 0;
	while((state.mem[state.pc] >> 22)!=6) // Goes until halt is seen
	{
		int regA = 0, regB = 0, regC = 0;
		count++;
		printState(&state);
		if((state.mem[state.pc] >> 22) == 0) // Check for ADD instruction
		{
			regA = ((state.mem[state.pc] >> 19) & 7);
			regB = ((state.mem[state.pc] >> 16) & 7);
			regC = (state.mem[state.pc] & 7);
			state.reg[regC] = regA + regB;
		}
		if((state.mem[state.pc] >> 22) == 1) // Check for NAND instruction
        	{
                	regA = ((state.mem[state.pc] >> 19) & 7);
                	regB = ((state.mem[state.pc] >> 16) & 7);
                	regC = (state.mem[state.pc] & 7);
			state.reg[regC] = ~(regA & regB);
        	}
		
		if((state.mem[state.pc] >> 22) == 2) // Check for LW instruction
                {
                        regA = ((state.mem[state.pc] >> 19) & 7);
                        regB = ((state.mem[state.pc] >> 16) & 7);
                        regC = (state.mem[state.pc] & 0xFFFF);
			temp = convertNum(regC);
			offset = temp + state.reg[regA];
			state.reg[regB] = state.mem[offset];
                }
		
		if((state.mem[state.pc] >> 22) == 3) // Check for SW instruction
                {
                	regA = ((state.mem[state.pc] >> 19) & 7);
                        regB = ((state.mem[state.pc] >> 16) & 7);
                        regC = (state.mem[state.pc] & 0xFFFF);
                        temp = convertNum(regC);
                        offset = temp + state.reg[regA];
                        state.mem[offset] = state.reg[regB];
                }
		/*
		if((state.mem[state.pc] >> 22) == 4) // Check for BEQ instruction
                {
                        regA = ((state.mem[state.pc]  >> 19) & 7);
                        regB = ((state.mem[state.pc]  >> 16) & 7);
                        regC = (state.mem[state.pc] & 0xFFFF);
			temp = convertNum(regC);
                        if(regA == regB)
			{
				state.pc = state.pc + 1 + temp;
				beq = true;	
			}
                }
		
		if((state.mem[state.pc] >> 22) == 5) // Check for JALR instruction
                {
                        regA = ((state.mem[state.pc] >> 19) & 7);

                        regB = ((state.mem[state.pc] >> 16) & 7);
			if(regA == regB)
                        {
                                state.pc = state.pc + regA;

                        }
                    
                }
		*/
		if(beq==false)
			state.pc++;
	}
	printf("machine halted\n");
	printf("total of %d instructions executed\n", count);
	printf("final state of machine:\n");
	printState(&state);
    return(0);
}

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
	for (i=0; i<statePtr->numMemory; i++) {
	    printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
	}
    printf("\tregisters:\n");
	for (i=0; i<NUMREGS; i++) {
	    printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	}
    printf("end state\n");
}

int convertNum(int num)
{	/* convert a 16-bit number into a 32-bit Sun integer */
	if (num & (1<<15) ) 
	{    
		num -= (1<<16);
	}
	return(num);
}

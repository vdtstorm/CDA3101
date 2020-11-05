#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define NUMMEMORY 65536 /* maximum number of data words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

#define ADD 0
#define NAND 1
#define LW 2
#define SW 3
#define BEQ 4
#define JALR 5 /* JALR will not implemented for Project 3 */
#define HALT 6
#define NOOP 7

#define NOOPINSTRUCTION 0x1c00000

typedef struct IFIDStruct {
    int instr;
    int pcPlus1;
} IFIDType;

typedef struct IDEXStruct {
    int instr;
    int pcPlus1;
    int readRegA;
    int readRegB;
    int offset;
} IDEXType;

typedef struct EXMEMStruct {
    int instr;
    int branchTarget;
    int aluResult;
    int readRegB;
} EXMEMType;

typedef struct MEMWBStruct {
    int instr;
    int writeData;
} MEMWBType;

typedef struct WBENDStruct {
    int instr;
    int writeData;
} WBENDType;

typedef struct stateStruct {
    int pc;
    int instrMem[NUMMEMORY];
    int dataMem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
    IFIDType IFID;
    IDEXType IDEX;
    EXMEMType EXMEM;
    MEMWBType MEMWB;
    WBENDType WBEND;
    int cycles; /* number of cycles run so far */
} stateType;

void printState(stateType *statePtr);
int field0(int instruction);
int field1(int instruction);
int field2(int instruction);
int opcode(int instruction);
int convertNum(int num);
void printInstruction(int instr);
void ID(const stateType, stateType*);
void EX(const stateType, stateType*);
void MEM(const stateType, stateType*);
void WB(const stateType, stateType*);


int main(int argc, const char *argv[])
{
	// Code from reading in file comes from project 2
	stateType state, newState;
	char line[MAXLINELENGTH];
    
    	FILE *filePtr;

    	if (argc != 2) 
	{
        	printf("error: usage: %s <machine-code file>\n", argv[0]);
        	exit(1);
    	}

    	filePtr = fopen(argv[1], "r");
    	if (filePtr == NULL) 
	{
        	printf("error: can't open file %s", argv[1]);
        	perror("fopen");
        	exit(1);
    	}

    	/* read in the entire machine-code file into memory */
    	for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;	
	state.numMemory++) 
	{
        	if (sscanf(line, "%d", state.instrMem+state.numMemory) != 1) 
		{
            		printf("error in reading address %d\n", state.numMemory);
            		exit(1);
        	}
        	printf("memory[%d]=%d\n", state.numMemory, state.instrMem[state.numMemory]);
	}
	printf( "%d memory words\n", state.numMemory );
	printf( "\tinstruction memory:\n" );
	for(int i = 0; i < state.numMemory; i++)
	{
		printf( "\t\tinstrMem[ %d ] ", i);
		printInstruction(state.instrMem[i]);
	}	
	

	// Setting pipline instructions to Noop instructions
	state.pc = 0;
	state.cycles = 0;

	
	for(int i = 0; i < NUMREGS; i++) 
	{
		state.reg[i] = 0;
	}

	
	state.IFID.instr = NOOPINSTRUCTION;
	state.IDEX.instr = NOOPINSTRUCTION;
	state.EXMEM.instr = NOOPINSTRUCTION;
	state.MEMWB.instr = NOOPINSTRUCTION;
	state.WBEND.instr = NOOPINSTRUCTION;
	int regA = 0, regB = 0, offset = 0, temp = 0;
	while (1) 
	{

		printState(&state);

		/* check for halt */
		if (opcode(state.MEMWB.instr) == HALT) 
		{
	    		printf("machine halted\n");
	    		printf("total of %d cycles executed\n", state.cycles);
	    		exit(0);
		}

		newState = state;
		newState.cycles++;

		/* --------------------- IF stage --------------------- */
		// Im grabbing the instruction and incrementing the PC+1
		newState.IFID.instr = state.instrMem[state.pc];
		newState.IFID.pcPlus1 = state.pc + 1;
		newState.pc = state.pc + 1;
		/* --------------------- ID stage --------------------- */
		// Passing the instruction to the ID/EX stage
		newState.IDEX.instr = state.IFID.instr;
		// Transferring the PC + 1 also even if not needed
		newState.IDEX.pcPlus1 = state.IFID.pcPlus1;
		
		newState.IDEX.readRegA = state.reg[field0(state.IFID.instr)];
		newState.IDEX.readRegB = state.reg[field1(state.IFID.instr)];
		newState.IDEX.offset = field2(state.IFID.instr);
		temp = convertNum(newState.IDEX.offset);
		regA = newState.IDEX.readRegA; 
		regB = newState.IDEX.readRegB;
		offset = temp;
		/* --------------------- EX stage --------------------- */
		newState.EXMEM.instr = state.IDEX.instr;
		if(opcode(newState.EXMEM.instr) == ADD) // Check for ADD instruction
		{
			 newState.EXMEM.aluResult = regA + regB;  
			
		}
		if(opcode(newState.EXMEM.instr) == NAND) // Check for NAND instruction
                {
                         newState.EXMEM.aluResult = ~(regA & regB);

                }
		if(opcode(newState.EXMEM.instr) == LW) // Check for LW instruction
                {
                         newState.EXMEM.aluResult = regA + offset;

                }
		if(opcode(newState.EXMEM.instr) == SW) // Check for SW instruction
                {
                         newState.EXMEM.aluResult = regA + offset;

                }
		if(opcode(newState.EXMEM.instr) == BEQ) // Check for BEQ instruction
                {
                         newState.EXMEM.aluResult = (regA == regB);

                }
		/*
		if(opcode(newState.EXMEM.instr) == JALR) // Check for BEQ instruction
                {
                         newState.EXMEM.aluResult = (regA == regB);

                }
		*/

		/* --------------------- MEM stage --------------------- */
		newState.MEMWB.instr = state.EXMEM.instr;
		if(opcode(newState.MEMWB.instr) == ADD || opcode(newState.MEMWB.instr) == NAND) // Check for ADD or NAND instruction
                {
                         newState.MEMWB.writeData = state.EXMEM.aluResult;

                }
		if(opcode(newState.MEMWB.instr) == LW) // Check for LW instruction
                {
                         newState.MEMWB.writeData = state.dataMem[state.EXMEM.aluResult];

                }
		if(opcode(newState.MEMWB.instr) == SW) // Check for SW instruction                                          
                {
                         newState.dataMem[state.EXMEM.aluResult] = state.EXMEM.readRegB;

                }

		/* --------------------- WB stage --------------------- */
		newState.WBEND.instr = state.MEMWB.instr;
		newState.WBEND.writeData = state.MEMWB.writeData;
		if(opcode(newState.WBEND.instr) == ADD || opcode(newState.WBEND.instr) == NAND) // Check for ADD or NAND instruction
                {
                         newState.reg[field2(state.MEMWB.instr)] = state.MEMWB.writeData;

                }
                if(opcode(newState.MEMWB.instr) == LW) // Check for LW instruction                                          
                {
                         newState.reg[field1(state.MEMWB.instr)] = state.MEMWB.writeData;

                }

		state = newState; /* this is the last statement before end of the loop.
			    		It marks the end of the cycle and updates the
			    		current state with the values calculated in this
			    		cycle 
				*/
    	}

	return 0;
}

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate before cycle %d starts\n", statePtr->cycles);
    printf("\tpc %d\n", statePtr->pc);

    printf("\tdata memory:\n");
	for (i=0; i<statePtr->numMemory; i++) {
	    printf("\t\tdataMem[ %d ] %d\n", i, statePtr->dataMem[i]);
	}
    printf("\tregisters:\n");
	for (i=0; i<NUMREGS; i++) {
	    printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	}
    printf("\tIFID:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->IFID.instr);
	printf("\t\tpcPlus1 %d\n", statePtr->IFID.pcPlus1);
    printf("\tIDEX:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->IDEX.instr);
	printf("\t\tpcPlus1 %d\n", statePtr->IDEX.pcPlus1);
	printf("\t\treadRegA %d\n", statePtr->IDEX.readRegA);
	printf("\t\treadRegB %d\n", statePtr->IDEX.readRegB);
	printf("\t\toffset %d\n", statePtr->IDEX.offset);
    printf("\tEXMEM:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->EXMEM.instr);
	printf("\t\tbranchTarget %d\n", statePtr->EXMEM.branchTarget);
	printf("\t\taluResult %d\n", statePtr->EXMEM.aluResult);
	printf("\t\treadRegB %d\n", statePtr->EXMEM.readRegB);
    printf("\tMEMWB:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->MEMWB.instr);
	printf("\t\twriteData %d\n", statePtr->MEMWB.writeData);
    printf("\tWBEND:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->WBEND.instr);
	printf("\t\twriteData %d\n", statePtr->WBEND.writeData);
}

int field0(int instruction)
{
    return( (instruction>>19) & 0x7);
}

int field1(int instruction)
{
    return( (instruction>>16) & 0x7);
}

int field2(int instruction)
{
    return(instruction & 0xFFFF);
}

int opcode(int instruction)
{
    return(instruction>>22);
}

void printInstruction(int instr)
{
    char opcodeString[10];
    if (opcode(instr) == ADD) {
	strcpy(opcodeString, "add");
    } else if (opcode(instr) == NAND) {
	strcpy(opcodeString, "nand");
    } else if (opcode(instr) == LW) {
	strcpy(opcodeString, "lw");
    } else if (opcode(instr) == SW) {
	strcpy(opcodeString, "sw");
    } else if (opcode(instr) == BEQ) {
	strcpy(opcodeString, "beq");
    } else if (opcode(instr) == JALR) {
	strcpy(opcodeString, "jalr");
    } else if (opcode(instr) == HALT) {
	strcpy(opcodeString, "halt");
    } else if (opcode(instr) == NOOP) {
	strcpy(opcodeString, "noop");
    } else {
	strcpy(opcodeString, "data");
    }

    printf("%s %d %d %d\n", opcodeString, field0(instr), field1(instr),
	field2(instr));
}

int convertNum(int num)
{	/* convert a 16-bit number into a 32-bit Sun integer */
	if (num & (1<<15) ) 
	{    
		num -= (1<<16);
	}
	return(num);
}

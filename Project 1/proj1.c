/*
	Name: Vinson Thomas (vdt16)
	Class: CDA3101
	Project 1: LC3101 Assembler
	Programming Language: C
	Comments
		- This code uses the code fragment given
		- This code must be compiled with -std=c99 to work correctly
		- This will have a warning for the implicit declaration for strdup(), but is needed for my code
		- I have tried other options, but strdup seems to be the way to go when it comes to getting the labels and 
		their corresponding values for my array of strings 
		- strdup() also causes a warning so "#define _GNU_SOURCE" had to be added to the beggining of my file 
		
*/
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>
#define MAXLINELENGTH 1000

typedef struct // Used to get the address value of label
{
        char  key[MAXLINELENGTH];
	char * items[100];
        int val;
        int arg;
}KeyVal;

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
int isOpcode(char * check); // Checks for undefined Opcodes
int isDuplicate(char ** label,int size,char * check); // Checks for duplicate labels
int labelError(char * check); // Checks for undefined labels

int main(int argc, char *argv[])
{
    	char *inFileString, *outFileString;
    	FILE *inFilePtr, *outFilePtr;
    	char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
            	arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];
	
	// atoi to convert int to string;
    	if (argc != 3) 
	{
        	printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",argv[0]);
        	exit(1);
    	}
	
    	inFileString = argv[1];
    	outFileString = argv[2];

    	inFilePtr = fopen(inFileString, "r");
    	if (inFilePtr == NULL) 
	{
        	printf("error in opening %s\n", inFileString);
        	exit(1);
    	}
    	outFilePtr = fopen(outFileString, "w");
    	if (outFilePtr == NULL) 
	{
        	printf("error in opening %s\n", outFileString);
        	exit(1);
    	}
	// This loop grabs all labels and puts them into a character array
	
	
	int c = 0; // increments kv
	
	KeyVal kv[100];
	

	
	
	// Giving labels their address
	// Checks for errors
	// 1st Pass
	while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2))
	{
		
		
		strcpy(kv[c].key,label);
		kv[0].items[c]=strdup(label); // Used for duplicate label checking
		
		if(strcmp(opcode,".fill")!=0)
			kv[c].val = c;
			
		// This is giving the label the value of the argument to the right of .fill
		if(strcmp(opcode,".fill")==0)
		{
			
			if(isNumber(arg0)==1)
                        	kv[c].val = atoi(arg0);
			else if(!isNumber(arg0))
			{
				for(int i = 0; i<c;i++)
				{
					if(strcmp(arg0,kv[i].key)==0)
					{
						kv[c].val = kv[i].val;
					}
				}
			}
						
		}
		c++; // increments to add next label and value
		
		// Check for undefined variables
		char * not = "Assembly File will not be made";
		int result = labelError(label);		
		if(result==1)
		{
			char * error1 = " is a label with longer than 6 characters";
			strcat(label,error1);
			puts(label); 
			puts(not);
			exit(1);
		}
		if(result==2)
		{
			char * error1 = " is an undefined label";
                        strcat(label,error1);
                        puts(label);
			puts(not);
                        exit(1);
		}
	
		
		// Error checking for duplicate labels
		int check = isDuplicate(kv[0].items,c,label);
		if(check > 1)
		{
			char * p = " is a duplicate label";
			strcat(label,p);
			puts(label);
			puts(not);
			exit(1);
		}
		
		// Checks for undefined opcodes
		int check1 = isOpcode(opcode);
        	if(check1==0)
        	{
                	char * er = " is an undefined opcode";
                	strcat(opcode,er);
                	puts(opcode);
			puts(not);
                	exit(1);
        	}
		
		
	}
	
	
	rewind(inFilePtr);
	
	/* This just prints out the labels and their values
	for(int i = 0; i<c;i++)
	{	if(strcmp(kv[i].key,"\0")!=0)
			fprintf(outFilePtr,"%d)Label: %s\tAddress: %d\n",i,kv[i].key,kv[i].val);
	}
	*/

	


	
	
	int unused = 0;
	int unused1 =0 ;
	unused = unused << 31;
	unused1  = unused1 << 15; 
	int result = 0;
		
	// 2nd Pass
	// This loop looks through instructions and prints out corresponding value
	int PC = 0; // Program Counter
    	while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) 
	{
		
		// add r1 r2 dest
		// dest = r1 + r2
		// op 24-22, regA 21-19, regB 18-16, unused 15-3, dest 15-0
		
		
		
		
		int regA;
		int regB;
		int regC;
		int dest;
        	if(strcmp(opcode,"add")==0)
		{
			int add = 0;
			add = add << 22;
			regA = atoi(arg0);
			regB = atoi(arg1);
			dest = atoi(arg2);
			
			regA = regA << 19;
			regB = regB << 16;
			
			dest = dest << 0;
			
			result = add + regA + regB + unused + dest;
			fprintf(outFilePtr,"Address %d: %d\n",PC,result);
			PC++;
		}
		if(strcmp(opcode,"nand")==0)
		{
			int nand = 1;
			nand = nand << 22;
			regA = atoi(arg0);
                        regB = atoi(arg1);
                        dest = atoi(arg2);
			regA = regA << 19;
                        regB = regB << 16;

                        dest = dest << 0;

                        result = nand + regA + regB + unused + dest;
                        fprintf(outFilePtr,"Address %d: %d\n",PC,result);
			PC++;
		}
		if(strcmp(opcode,"lw")==0)
		{
			int lw = 2;
			
			
			if(!isNumber(arg0))
			{
				for(int i = 0; i < c;i++)
				{
					if(strcmp(arg0,kv[i].key)==0)
					{
						regA = kv[i].val;
					}
				}
			}
			else	
                        	regA = atoi(arg0);
			if(!isNumber(arg1))
                        {
                                for(int i = 0; i < c;i++)
                                {
                                        if(strcmp(arg1,kv[i].key)==0)
                                        {
                                                regB = kv[i].val;
                                        }
                                }
                        }
			else
                        	regB = atoi(arg1);
			if(!isNumber(arg2))
                        {
                                for(int i = 0; i < c;i++)
                                {
                                        if(strcmp(arg2,kv[i].key)==0)
                                        {
                                                regC = kv[i].val;
                                        }
                                }
                        }
			else
                        	regC = atoi(arg2);
                        
			lw = lw <<22;
			regA = regA << 19;
                        regB = regB << 16;
			if(regC < 0)
				regC = ~(regC + 2);
			else
				regC = regC + 2;
                        regC = regC << 0;
			result = lw + regA + regB + regC;
                        fprintf(outFilePtr,"Address %d: %d\n",PC,result);
			PC++;
		 
		}
		if(strcmp(opcode,"sw")==0)
		{
			int sw = 3;
			if(!isNumber(arg0))
                        {
                                for(int i = 0; i < c;i++)
                                {
                                        if(strcmp(arg0,kv[i].key)==0)
                                        {
                                                regA = kv[i].val;
                                        }
                                }
                        }
                        else
                                regA = atoi(arg0);
			if(!isNumber(arg1))
                        {
                                for(int i = 0; i < c;i++)
                                {
                                        if(strcmp(arg1,kv[i].key)==0)
                                        {
                                                regB = kv[i].val;
                                        }
                                }
                        }
                        else
                                regB = atoi(arg1);
			if(!isNumber(arg2))
                        {
                                for(int i = 0; i < c;i++)
                                {
                                        if(strcmp(arg2,kv[i].key)==0)
                                        {
                                                regC = kv[i].val;
                                        }
                                }
                        }
                        else
                                regC = atoi(arg2);
			sw = sw << 22;
                        regA = regA << 19;
                        regB = regB << 16;
                        regC = regC << 0;
                        result = sw + regA + regB + regC;
                        fprintf(outFilePtr,"Address %d: %d\n",PC,result);
			PC++;
		}
		if(strcmp(opcode,"beq")==0)
		{
			//int beq = 4;
			PC++;
		}
		if(strcmp(opcode,"jalr")==0)
		{
			PC++;
		}
		if(strcmp(opcode,"halt")==0)
		{
			int halt = 6 << 22;
                        int un = 0 << 0;
			// Printing out indication of HALT
			char * process = "Halt opcode was present";
			puts(process);
			
                        result = halt + un;
                        fprintf(outFilePtr,"Address %d: %d\n",PC,result);
			
			PC++;
		}
		if(strcmp(opcode,"noop")==0)
		{
			int noop = 7 << 22;
			int un = 0 << 0;
			
			result = noop + un;
                        fprintf(outFilePtr,"Address %d: %d\n",PC,result);
			PC++;
		}
		if(strcmp(opcode,".fill")==0)
		{
			if(!isNumber(arg0))
                        {
                                for(int i = 0; i < c;i++)
                                {
                                        if(strcmp(arg0,kv[i].key)==0)
                                        {
                                                regA = kv[i].val;
                                        }
                                }
                        }
                        else
                                regA = atoi(arg0);
			fprintf(outFilePtr,"Address %d: %d\n",PC,regA);
			PC++;
		}
		
    	}
	char * done = "Assembler was successfull";
	puts(done);
	
    /* this is how to rewind the file ptr so that you start reading from the
        beginning of the file */
    //rewind(inFilePtr);
	 
    /* after doing a readAndParse, you may want to do the following to test the
        opcode */
	
    	if (!strcmp(opcode, "add")) 
	{
		
        	/* do whatever you need to do for opcode "add" */
    	}
	fclose(inFilePtr);
         fclose(outFilePtr);
    	return(0);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0, char *arg1, char *arg2)
{
    	char line[MAXLINELENGTH];
    	char *ptr = line;

    	/* delete prior values */
    	label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    	/* read the line from the assembly-language file */
    	if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) 
	{
 		/* reached end of file */
        	return(0);
    	}

    	/* check for line too long (by looking for a \n) */
    	if (strchr(line, '\n') == NULL) 
	{
        	/* line too long */
 		printf("error: line too long\n");
 		exit(1);
    	}

    	/* is there a label? */
	
    	ptr = line;
    	if (sscanf(ptr, "%[^\t\n ]", label)) 
	{
		
 		/* successfully read label; advance pointer over the label */
        	ptr += strlen(label);
	
    	}
	
    	/*
     	* Parse the rest of the line.  Would be nice to have real regular
     	* expressions, but scanf will suffice.
     	*/
    	sscanf(ptr, "%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]",
        opcode, arg0, arg1, arg2);
	
	
    	return(1);
}

int isNumber(char *string)
{
   	/* return 1 if string is a number */
    	int i;
    	return( (sscanf(string, "%d", &i)) == 1);
}

// Returns 0 if opcode is undefined
// Returns 1 if opcode is valid 
int isOpcode(char * check)
{
	char * opcodes[9] = {"add","nand","lw","sw","beq","noop",".fill","halt","jalr"};
	char result = 0;
	for(int i = 0; i < 9; i++)
	{
		if(strcmp(check,opcodes[i])==0)
		{
			result = 1;
		}
	}
	return result;
}

// Checks through list
// If a label is checked more than once it is a duplicate 
int isDuplicate(char ** label,int size,char * check)
{

	int dupe = 0;
	for(int i = 0; i < size; i++ )
	{
		if(strcmp(check,label[i])==0)
		{
			if(strcmp(label[i],"\0")!=0)
			{
				dupe++; // Skipping empty spaces
			}
		}
	}
	
	return dupe;
}

// Check if label are undefined
// returns 1 if label is too long
// returns 2 if label is undefined
// returns 0 if correct 
int labelError(char * check)
{
	
	int result = 0;
	int size = strlen(check);
	// checking label length
	if(size > 6)
	{
		result = 1;
		
	}
	// Checks if label begins with a number
	if(isdigit(check[0]))
        {
        	result = 2;
        }
	// Check if label is neither letter or number        
	for(int i = 0; i < size; i++)
	{
		
		if(!isalpha(check[i]))
		{
			if(!isdigit(check[i]))
			{
				
				result = 2;
				break;
			}
			
		}
	}
	return result;
}

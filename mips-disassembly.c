/**********************************************************************
* MIPS - disassembly
* 
* Copyright (C)2015  Manuel Moscoso Dominguez / 
* manuel.moscoso.d - gmail.com
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software Foundation,
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
* 
* *********************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

int binaryToInt(char *binary);
char* intToBinary(uint32_t bigEnd);
char* getInstructionImmediate(char* binary,char* allbinary,int current);
char* getInstructionJump(char* binary,char* allbinary,int current);
char* getInstructionRegister(char* binary,char* allbinary);
char* getRegister(int value);

int main(int argc,char *argv []) {
	
	//Variables
	FILE* fileBinary;
	FILE* fileOutput;
	char LINE[50];
	char* buffer;
	char* binary;
	char head[6];
	char operFunctionBin[6];
		
	uint32_t inst;
	int addMem = 0;
	unsigned int fileLen;
	
	//Parameter validation
	if ( argc != 3 ){
		printf("Not enough parameters: mmoscoso-dis BINARY OUTPUT\n");
		exit(0);
	}
	//File BIN validation
	fileBinary = fopen(argv[1], "rb");
	if( !fileBinary ) {
		printf("The file does not exists (%s)",argv[1]);
		fclose(fileBinary);
		exit(0);
	}
	else {
		//File OUTPUT validation
		fileOutput = fopen(argv[2], "w");
		if ( !fileOutput ) {
			printf("The program can't create output file (%s)",argv[2]);
			fclose(fileBinary);
			fclose(fileOutput);
			exit(0);
		}	
		
		fseek(fileBinary, 0, SEEK_END);
		fileLen=ftell(fileBinary);
		fseek(fileBinary, 0, SEEK_SET);
		buffer=(char *)malloc(fileLen+1);
        
        //MEMORY VALIDATION
        if ( !buffer ) {
			printf("Problem with memory");
			fclose(fileBinary);
			exit(0);
        }
        else {
			char buffer[4];
			
			//Processing the bin file
			while( fread(buffer, 1, 4, fileBinary) == 4 ) {
				//Writing Memory Address
				fprintf(fileOutput,"%08x: ",addMem);

				//Writing HEX int
				fprintf(fileOutput,"%02x",(unsigned char)buffer[0]);
				fprintf(fileOutput,"%02x",(unsigned char)buffer[1]);
				fprintf(fileOutput,"%02x",(unsigned char)buffer[2]);
				fprintf(fileOutput,"%02x ",(unsigned char)buffer[3]);
				
				
				inst = (uint32_t)(unsigned char)buffer[0] << 24 |
					   (uint32_t)(unsigned char)buffer[1] << 16 |
						(uint32_t)(unsigned char)buffer[2] << 8  |
						(uint32_t)(unsigned char)buffer[3];

			
				//Getting Binary from Instruction
				binary = intToBinary(inst);

				
				//If binary starts with 000000 is a Register Inst
				strncpy(head,binary,6);
				
				if ( strcmp("000000",head) == 0 ) {
					strncpy(operFunctionBin,binary+26,6);
					strncpy(LINE,getInstructionRegister(operFunctionBin,binary),50);
					fprintf(fileOutput,"%s\n",LINE);
				}
				else {
					strncpy(operFunctionBin,binary,6);
					strncpy(LINE,getInstructionImmediate(operFunctionBin,binary,addMem),50);
					if ( strcmp(LINE,"unknown") == 0 ) {
						strncpy(LINE,getInstructionJump(operFunctionBin,binary,addMem),50);
					}
					fprintf(fileOutput,"%s\n",LINE);
				}
				addMem = addMem + 4;
			}
		}
		//Closing files
        fclose(fileBinary);
        fclose(fileOutput);
        free(buffer);
	}
	return 0;
}

//Get register according to int
char* getRegister(int value){
	char* reg;
	switch(value){
		case 0:reg = "zero";
			break;
		case 1:reg = "at";
			break;
		case 2:reg = "v0";
			break;
		case 3:reg = "v1";
			break;
		case 4:reg = "a0";
			break;
		case 5:reg = "a1";
			break;
		case 6:reg = "a2";
			break;
		case 7:reg = "a3";
			break;
		case 8:reg = "t0";
			break;
		case 9:reg = "t1";
			break;
		case 10:reg = "t2";
			break;
		case 11:reg = "t3";
			break;
		case 12:reg = "t4";
			break;
		case 13:reg = "t5";
			break;
		case 14:reg = "t6";
			break;
		case 15:reg = "t7";
			break;
		case 16:reg = "s0";
			break;
		case 17:reg = "s1";
			break;
		case 18:reg = "s2";
			break;
		case 19:reg = "s3";
			break;
		case 20:reg = "s4";
			break;
		case 21:reg = "s5";
			break;
		case 22:reg = "s6";
			break;
		case 23:reg = "s7";
			break;
		case 24:reg = "t8";
			break;
		case 25:reg = "t9";
			break;
		case 26:reg = "k0";
			break;
		case 27:reg = "k1";
			break;
		case 28:reg = "gp";
			break;
		case 29:reg = "sp";
			break;
		case 30:reg = "fp";
			break;
		case 31:reg = "ra";
			break;
		default:reg = "unknown";
			break;
	}
	
	return reg;
}

//Transfor uint32_t to Binary String
char* intToBinary(uint32_t bigEnd) {
    uint32_t i;
	char* binary;
	binary = (char*)malloc(32+1);
	char value; 
	int count = 0;
    for( i = 0x80000000 ; i != 0 ; i >>= 1 ){
		value = (bigEnd&i)?'1':'0';
		*(binary+count) = value;
		count++;
	}
	*(binary+count) = '\0';
    return binary;
}

//Transfor Binary String to Int
int binaryToInt(char *binary){
	int sum = 0;
	int i;
	for( i = 0 ; i <= strlen(binary)-1 ; i++ ) {
		if(binary[i] == '1') {
			sum = sum + pow(2,(strlen(binary)-1)-i);
		}
    }
	return sum;
}


// Function for get  the Jump instruction
// [oooooo]ii iiiiiiii iiiiiiii iiiiiiii
char* getInstructionJump(char* binary,char* allbinary,int current){
	char* RESULT;
	int resultFromBinary;
	char inst[50], regi[8], label[8], i[26],pc[4],labelBinary[32];
		
	//Creating Binary for label address
	strncpy(i,allbinary+6,26);
	i[26] = '\0';
	strncpy(pc,intToBinary(current),4);
    pc[4] = '\0';
    strcpy(labelBinary,pc);
    strcat(labelBinary,i);
    strcat(labelBinary,"00");
    labelBinary[32] = '\0';
	
	//Getting i
	resultFromBinary = binaryToInt(i);
	snprintf (regi, sizeof(regi)+1, "%08x",resultFromBinary);
	regi[8] = '\0';
	
	//Getting LABEL
	snprintf (label, sizeof(label)+1, "%08x",binaryToInt(labelBinary));
	
	strcpy(inst,"unknown");
	//Jump Encoding
	//Jump
	//o label
	if ( strcmp("000010",binary) == 0 ) {
		strcpy(inst,"j ");
		strcat(inst,label);
	}
	if ( strcmp("000011",binary) == 0 ) {
		strcpy(inst,"jal ");
		strcat(inst,label);
	}
	//Trap
	//o i
	if ( strcmp("011010",binary) == 0 ) {
		strcpy(inst,"trap ");
		strcat(inst,regi);
	}
	
	RESULT = malloc(strlen(inst) + 1);
	strcpy(RESULT,inst);
	return RESULT;
}

// Function for get  the Immediate instruction 
// [oooooo]ss sssttttt iiiiiiii iiiiiiii
char* getInstructionImmediate(char* binary,char* allbinary,int current){
	char* RESULT;
	int resultFromBinary;
	char inst[50], regs[4], regt[4],regi[6], s[5], t[5], i[16];
	
	s[5] = '\0';
	t[5] = '\0';
	regs[4] = '\0';
	regt[4] = '\0';
	regi[6] = '\0';
	
	//Breaking the binary string 
	strncpy(s,allbinary+6,5);
	strncpy(t,allbinary+11,5);
	strcpy(i,allbinary+16);
	i[16] = '\0';

	int immValue = binaryToInt(i);
	
	char label[8];
	int newInt = immValue * 4 + (current + 4);
	snprintf (label, sizeof(label)+1, "%08x",(newInt));
	
	 
	resultFromBinary = binaryToInt(s);
	strncpy(regs,getRegister(resultFromBinary),4);
	
	resultFromBinary = binaryToInt(t);
	strncpy(regt,getRegister(resultFromBinary),4);
	
	resultFromBinary = binaryToInt(i);
	snprintf (regi, sizeof(regi)+1, "0x%04x",resultFromBinary);
	regi[6] = '\0';
	
	strcpy(inst,"unknown");
	//Immediate Encoding
	//ArithLogI
	//o $t, $s, i
	if ( strcmp("001000",binary) == 0 ) {
		strcpy(inst,"addi $");
		strcat(inst,regt);
		strcat(inst,", $");
		strcat(inst,regs);
		strcat(inst,", ");
		strcat(inst,regi);
	}
	if ( strcmp("001001",binary) == 0 ) {
		strcpy(inst,"addiu $");
		strcat(inst,regt);
		strcat(inst,", $");
		strcat(inst,regs);
		strcat(inst,", ");
		strcat(inst,regi);
	}
	if ( strcmp("001100",binary) == 0 ) {
		strcpy(inst,"andi $");
		strcat(inst,regt);
		strcat(inst,", $");
		strcat(inst,regs);
		strcat(inst,", ");
		strcat(inst,regi);
	}
	if ( strcmp("001101",binary) == 0 ) {
		strcpy(inst,"ori $");
		strcat(inst,regt);
		strcat(inst,", $");
		strcat(inst,regs);
		strcat(inst,", ");
		strcat(inst,regi);
	}
	if ( strcmp("001110",binary) == 0 ) {
		strcpy(inst,"xori $");
		strcat(inst,regt);
		strcat(inst,", $");
		strcat(inst,regs);
		strcat(inst,", ");
		strcat(inst,regi);
	}
	if ( strcmp("001010",binary) == 0 ) {
		strcpy(inst,"slti $");
		strcat(inst,regt);
		strcat(inst,", $");
		strcat(inst,regs);
		strcat(inst,", ");
		strcat(inst,regi);
	}	
	//LoadI
	//o $t, immed32
	if ( strcmp("011001",binary) == 0 ) {
		strcpy(inst,"lhi $");
		strcat(inst,regt);
		strcat(inst,", ");
		strcat(inst,regi);
		
	}
	if ( strcmp("011000",binary) == 0 ) {
		strcpy(inst,"llo $");
		strcat(inst,regt);
		strcat(inst,", ");
		strcat(inst,regi);
		
	}
	//Branch
	//o $s, $t, label
	if ( strcmp("000100",binary) == 0 ) {
		strcpy(inst,"beq $");
		strcat(inst,regs);		
		strcat(inst,", $");
		strcat(inst,regt);
		strcat(inst,", ");
		strcat(inst,label);
		
		
	}
	if ( strcmp("000101",binary) == 0 ) {
		strcpy(inst,"bne $");
		strcat(inst,regs);		
		strcat(inst,", $");
		strcat(inst,regt);
		strcat(inst,", ");
		strcat(inst,label);
	}
	//BranchZ
	//o $s, label
	if ( strcmp("000111",binary) == 0 ) {
		strcpy(inst,"bgtz $");
		strcat(inst,regs);		
		strcat(inst,", ");
		strcat(inst,label);
	}
	if ( strcmp("000110",binary) == 0 ) {
		strcpy(inst,"blez $");
		strcat(inst,regs);		
		strcat(inst,", ");
		strcat(inst,label);
	}
	//LoadStore
	//o $t, i($s)
	if ( strcmp("100000",binary) == 0 ) {
		strcpy(inst,"lb $");
		strcat(inst,regt);
		strcat(inst,", ");
		strcat(inst,regi);
		strcat(inst," ($");
		strcat(inst,regs);
		strcat(inst,")");
	}
	if ( strcmp("100100",binary) == 0 ) {
		strcpy(inst,"lbu $");
		strcat(inst,regt);
		strcat(inst,", ");
		strcat(inst,regi);
		strcat(inst," ($");
		strcat(inst,regs);
		strcat(inst,")");
	}
	if ( strcmp("100001",binary) == 0 ) {
		strcpy(inst,"lh $");
		strcat(inst,regt);
		strcat(inst,", ");
		strcat(inst,regi);
		strcat(inst," ($");
		strcat(inst,regs);
		strcat(inst,")");
	}
	if ( strcmp("100101",binary) == 0 ) {
		strcpy(inst,"lhu $");
		strcat(inst,regt);
		strcat(inst,", ");
		strcat(inst,regi);
		strcat(inst," ($");
		strcat(inst,regs);
		strcat(inst,")");
	}
	if ( strcmp("100011",binary) == 0 ) {
		strcpy(inst,"lw $");
		strcat(inst,regt);
		strcat(inst,", ");
		strcat(inst,regi);
		strcat(inst," ($");
		strcat(inst,regs);
		strcat(inst,")");
	}
	if ( strcmp("101000",binary) == 0 ) {
		strcpy(inst,"sb $");
		strcat(inst,regt);
		strcat(inst,", ");
		strcat(inst,regi);
		strcat(inst," ($");
		strcat(inst,regs);
		strcat(inst,")");
	}
	if ( strcmp("101001",binary) == 0 ) {
		strcpy(inst,"sh $");
		strcat(inst,regt);
		strcat(inst,", ");
		strcat(inst,regi);
		strcat(inst," ($");
		strcat(inst,regs);
		strcat(inst,")");
	}
	if ( strcmp("101011",binary) == 0 ) {
		strcpy(inst,"sw $");
		strcat(inst,regt);
		strcat(inst,", ");
		strcat(inst,regi);
		strcat(inst," ($");
		strcat(inst,regs);
		strcat(inst,")");
	}
	
	RESULT = malloc(strlen(inst) + 1);
	strcpy(RESULT,inst);
	return RESULT;
}



// Function for get  the Register instruction
//  000000ss sssttttt dddddaaa aa[ffffff]
char* getInstructionRegister(char* binary,char* allbinary){

	char* RESULT;
	char inst[100];
	int resultFromBinary;
	char regs[4], regt[4], regd[4], rega[4];
	char s[5], t[5], d[5], a[5];
	
	s[5] = '\0';
	t[5] = '\0';
	d[5] = '\0';
	a[5] = '\0';
	regs[4] = '\0';
	regt[4] = '\0';
	regd[4] = '\0';
	rega[4] = '\0';
	strcpy(inst,"unknown");
	
	//Breaking the binary string 
	strncpy(s,allbinary+6,5);
	strncpy(t,allbinary+11,5);
	strncpy(d,allbinary+16,5);
	strncpy(a,allbinary+21,5);
	

	
	
	resultFromBinary = binaryToInt(s);
	strncpy(regs,getRegister(resultFromBinary),4);
	
	resultFromBinary = binaryToInt(t);
	strncpy(regt,getRegister(resultFromBinary),4);
	
	resultFromBinary = binaryToInt(d);
	strncpy(regd,getRegister(resultFromBinary),4);
	
	resultFromBinary = binaryToInt(a);
	snprintf (rega, sizeof(rega), "%d",resultFromBinary);
	
	
	
	//Register Encoding
	//ArithLog
	//f $d, $s, $t
	if ( strcmp("100000",binary) == 0) {
		strcpy(inst,"add $");
		strcat(inst,regd);
		strcat(inst,", $");
		strcat(inst,regs);
		strcat(inst,", $");
		strcat(inst,regt);
	}
	if ( strcmp("100001",binary) == 0 ) {
		strcpy(inst,"addu $");
		strcat(inst,regd);
		strcat(inst,", $");
		strcat(inst,regs);
		strcat(inst,", $");
		strcat(inst,regt);
	}
	if ( strcmp("100100",binary) == 0 ) {
		strcpy(inst,"and $");
		strcat(inst,regd);
		strcat(inst,", $");
		strcat(inst,regs);
		strcat(inst,", $");
		strcat(inst,regt);
	}
	if ( strcmp("100111",binary) == 0 ) {
		strcpy(inst,"nor $");
		strcat(inst,regd);
		strcat(inst,", $");
		strcat(inst,regs);
		strcat(inst,", $");
		strcat(inst,regt);
	}
	if ( strcmp("100101",binary) == 0 ) {
		strcpy(inst,"or $");
		strcat(inst,regd);
		strcat(inst,", $");
		strcat(inst,regs);
		strcat(inst,", $");
		strcat(inst,regt);
	}
	if ( strcmp("100010",binary) == 0 ) {
		strcpy(inst,"sub $");
		strcat(inst,regd);
		strcat(inst,", $");
		strcat(inst,regs);
		strcat(inst,", $");
		strcat(inst,regt);
	}
	if ( strcmp("100011",binary) == 0 ) {
		strcpy(inst,"subu $");
		strcat(inst,regd);
		strcat(inst,", $");
		strcat(inst,regs);
		strcat(inst,", $");
		strcat(inst,regt);
	}
	if ( strcmp("100110",binary) == 0 ) {
		strcpy(inst,"xor $");
		strcat(inst,regd);
		strcat(inst,", $");
		strcat(inst,regs);
		strcat(inst,", $");
		strcat(inst,regt);
	}
	if ( strcmp("101010",binary) == 0 ) {
		strcpy(inst,"slt $");
		strcat(inst,regd);
		strcat(inst,", $");
		strcat(inst,regs);
		strcat(inst,", $");
		strcat(inst,regt);
	}
	if ( strcmp("101001",binary) == 0 ) {
		strcpy(inst,"sltu $");
		strcat(inst,regd);
		strcat(inst,", $");
		strcat(inst,regs);
		strcat(inst,", $");
		strcat(inst,regt);
	}
	//DivMult
	//f $s, $t
	if ( strcmp("011010",binary) == 0 ) {
		strcpy(inst,"div $");
		strcat(inst,regs);
		strcat(inst,", $");
		strcat(inst,regt);
		
	}
	if ( strcmp("011011",binary) == 0 ) {
		strcpy(inst,"divu $");
		strcat(inst,regs);
		strcat(inst,", $");
		strcat(inst,regt);
	}
	if ( strcmp("011000",binary) == 0 ) {
		strcpy(inst,"mult $");
		strcat(inst,regs);
		strcat(inst,", $");
		strcat(inst,regt);
	}
	if ( strcmp("011001",binary) == 0 ) {
		strcpy(inst,"multu $");
		strcat(inst,regs);
		strcat(inst,", $");
		strcat(inst,regt);
	}
	//Shift
	//f $d, $t, a
	if ( strcmp("000000",binary) == 0 ) {
		strcpy(inst,"sll $");
		strcat(inst,regd);
		strcat(inst,", $");
		strcat(inst,regt);
		strcat(inst,", ");
		strcat(inst,rega);
	}
	if ( strcmp("000011",binary) == 0 ) {
		strcpy(inst,"sra $");
		strcat(inst,regd);
		strcat(inst,", $");
		strcat(inst,regt);
		strcat(inst,", ");
		strcat(inst,rega);
	}
	if ( strcmp("000010",binary) == 0 ) {
		strcpy(inst,"srl $");
		strcat(inst,regd);
		strcat(inst,", $");
		strcat(inst,regt);
		strcat(inst,", ");
		strcat(inst,rega);
	}
	//Shift V
	//f $d, $t, $s
	if ( strcmp("000100",binary) == 0 ) {
		strcpy(inst,"sllv $");
		strcat(inst,regd);
		strcat(inst,", $");
		strcat(inst,regt);
		strcat(inst,", $");
		strcat(inst,regs);
	}
	if ( strcmp("000111",binary) == 0 ) {
		strcpy(inst,"srav $");
		strcat(inst,regd);
		strcat(inst,", $");
		strcat(inst,regt);
		strcat(inst,", $");
		strcat(inst,regs);
	}
	if ( strcmp("000110",binary) == 0 ) {
		strcpy(inst,"srlv $");
		strcat(inst,regd);
		strcat(inst,", $");
		strcat(inst,regt);
		strcat(inst,", $");
		strcat(inst,regs);
	}
	//JumpR
	//f $s
	if ( strcmp("001001",binary) == 0 ) {
		strcpy(inst,"jalr $");
		strcat(inst,regs);
	}
	if ( strcmp("001000",binary) == 0 ) {
		strcpy(inst,"jr $");
		strcat(inst,regs);
	}
	//MoveFrom
	//f $d
	if ( strcmp("010000",binary) == 0 ) {
		strcpy(inst,"mfhi $");
		strcat(inst,regd);
	}
	if ( strcmp("010010",binary) == 0 ) {
		strcpy(inst,"mflo $");
		strcat(inst,regd);
	}
	//MoveTo
	//f $s
	if ( strcmp("010001",binary) == 0 ) {
		strcpy(inst,"mthi $");
		strcat(inst,regs);
	}
	if ( strcmp("010011",binary) == 0 ) {
		strcpy(inst,"mtlo $");
		strcat(inst,regs);
	}
	
	RESULT = malloc(strlen(inst) + 1);
	strcpy(RESULT,inst);
	return RESULT;
}


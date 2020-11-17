/*------assembler.c------*/
/*this is the master file for the program, this file includes the declaration
		for all the global variables used across the program and the main function*/

/*---------------------------------------------------*/
/*------HEADER AND GLOBAL VARIABLES DECLARATION------*/
/*---------------------------------------------------*/

#include "header.h"

/*FILE*: pointers for all the needed files (one input and three outputs)
--matrix: global matrix that stores the current sentence being handled divided into a matrix form
--ic: instruction counter, dc: data counter, matR: matrix row index (word), matC: matrix column index (character)
--error: flag (turns on once an error occurs), row: current row number, firstPass: flag (turns off once firsPass is finished)
--symbolPtr: two pointers pointing to the head and tail of the 'symbol list'
--tableObjectPtr: two pointers pointing to the head and tail of the 'table object list' and one to the current object handled*/

FILE* fd, *ent, *ext, *ob;
char matrix[MAX_LINE][MAX_LINE];
int ic = START_IC, dc, matR, matC, error, row, firstPass = 1;
symbolPtr symbolHead = NULL, symbolTail = NULL;
tableObjectPtr tableObjectHead = NULL, tableObjectTail = NULL, tableObjectGlobal = NULL;

/*---------------------*/
/*------FUNCTIONS------*/
/*---------------------*/

/*------main------*/
/*The main function of the program, this function receives the input files as
	arguments and sends them to the openFiles function in order to begin the program
		after work on each file has ended it calls the reset function and continues to the next file*/

int main(int argc, char* argv[])
{
    while(*++argv)
    {
        openFiles(*argv);
        reset();
    }

    return 0;
}
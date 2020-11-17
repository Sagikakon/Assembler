/*------header.h------*/
/*This file includes declarations used across the program and across files
        libraries used, number defines, enumerations, structures and function declarations*/

/*------------------------------*/
/*------LIBRARY INCLUSIONS------*/
/*------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*--------------------------*/
/*------NUMBER DEFINES------*/
/*--------------------------*/

#define MAX_POSITIVE_CODE_NUM 2047
#define MAX_NEGATIVE_CODE_NUM -2048
#define MAX_POSITIVE_DATA_NUM 16383
#define MAX_NEGATIVE_DATA_NUM -16384
#define MAX_LINE 80
#define MAX_LABEL 31
#define RESET_ZERO 0
#define START_IC 100
#define END_OF_STRING '\0'
#define FIRST_CHARACTER 0
#define SECOND_CHARACTER 1
#define THIRD_CHARACTER 2
#define FIRST_WORD 0
#define SECOND_WORD 1
#define FIRST_PRINTABLE_CHAR 32
#define LAST_PRINTABLE_CHAR 127
#define FAIL -1
#define NUM_OF_COMMANDS 16
#define NUM_OF_INSTRUCTIONS 4

/*------------------------*/
/*------ENUMERTAIONS------*/
/*------------------------*/

/*sourceGroups: divides commands into groups by the source operand methods allowed (specified in checkCommand function)
--destinationGroups: divides commands into groups by the destination operand methods allowed (specified in checkCommand function)
--methods: the four addressing methods
--commands: all commands (mov, cmp, add, sub .. etc)
--instructions: the four instructions
--areType: the types of initialization for each object's ARE bits*/

typedef enum {no_source_method, one_source_method, four_source_methods} sourceGroups;
typedef enum {no_destination_method, two_destination_methods, three_destination_methods, four_destination_methods} destinationGroups;
typedef enum {immediate_addressing = 1, direct_addressing = 2,register_indirect_addressing = 4, register_direct_addressing = 8} methods;
typedef enum {mov, cmp, add, sub, lea, clr, not, inc, dec, jmp, bne, red, prn, jsr, rts, stop} commands;
typedef enum {data = 1, entry, myExtern, string} instructions;
typedef enum {external = 1, relocateable = 2, absolute = 4} areType;
typedef enum {false, true} boolean;

/*---------------------------*/
/*------DATA STRUCTURES------*/
/*---------------------------*/

/*------symbol------*/
/*used to create the 'symbol list' and store information for each label entered
--name: the label itself
--address: the IC or DC at the moment the label was declared
--withInstruct: either false (label attached to order) or true (label attached to instruction)
--isExtern: either true (label is external) or false
--isEntry: either true (label was declared with .entry at least once) or false
--next: pointer to the next symbol on the 'symbol list'*/

typedef struct symbol* symbolPtr;
typedef struct symbol
{
    char name[100];
    int address;
    boolean withInstruct;
    boolean isExtern;
    boolean isEntry;
    symbolPtr next;
} symbol;

/*------tableObject------*/
/*used to create the 'table object list' and store information and binary code for each 'memory word'
--name: holds the name of the label assigned to the object (if external) for the '.ext' file
--address: holds the address for the object
--isIC: either false (label attached to order) or true (label attached to instruction)
--wordMemory: union holding the required types of bit fields to store the object's binary code
        orderMemory: divided into 4 bit fields (ARE, destination method code, source method code, opcode)
        infoMemory: divided into 2 bit fields (ARE and 12 bits data)
        regMemory: divided into 4 bit fields (ARE, destination method code, source method code, 6 blank fields) used for registers
        all: 15 bits used for storing binary data for operands attached to instructions
--next: pointer to the next object on the 'table object list'*/

typedef struct tableObject* tableObjectPtr;
typedef struct tableObject
{
    char name[100];
    int address;
    boolean isIC;
    union
    {
        struct 
        {
            unsigned int ARE   :3;
            unsigned int destination:4;
            unsigned int source:4;
            unsigned int opcode:4;
        } orderMemory;

        struct
        {
            unsigned int ARE:3;
            unsigned int num:12;
        } infoMemory;

        struct
        {
            unsigned int ARE:3;
            unsigned int destination:3;
            unsigned int source:3;
            unsigned int blank:6;
        } regMemory;

        unsigned int all:15;
    } wordMemory;

    tableObjectPtr next;
} tableObject;

/*---------------------------------*/
/*------FUNCTION DECLARATIONS------*/
/*---------------------------------*/

/*------functions in file generalFunctions.c------*/

int checkImmediateAddressing(char*);
int checkAsterisk(char*);
int isRegister(char*);
int isVariable(char*);
int isLabel(char*);

/*------functions in file handleInput.c------*/

void splitString(char*);
void checkSentence(int);
void checkCommand(char*);
int isCommand(char*);
int isInstruct(char*);
int isReserved(char*);

/*------functions in file handleLists.c------*/

void addToSymbolList(char*, boolean, boolean);
void addToObjectList(boolean);
int checkList(char*);
int checkExtern(char*);
void freeLists();

/*------functions in file firstPass.c------*/

void openFiles(char*);
void initiatePass(FILE*);
void executeOrder(commands, int, int);
void executeInstruct(instructions);
void reset();

/*------functions in file secondPass.c------*/

void executeOrderSecond(commands, int, int);
void executeInstructSecond(instructions);
void updateAddress();
int returnAddress(char*);
void intoFiles(char*);
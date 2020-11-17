/*------handleLists.c------*/
/*This file includes a variation of functions used to
        create, handle, and eventually free linked lists*/

/*--------------------------------------------------*/
/*------HEADER AND EXTERN FOR GLOBAL VARIABLES------*/
/*--------------------------------------------------*/

#include "header.h"

extern symbolPtr symbolHead, symbolTail;
extern tableObjectPtr tableObjectHead, tableObjectTail, tableObjectGlobal;
extern int ic, dc, error, row;

/*---------------------*/
/*------FUNCTIONS------*/
/*---------------------*/

/*------addToObjectList------*/
/*Function creates the 'table object list' and adds a new object to it when used for the first time
        and from the second time forward adds an object to the that list*/

/*Table Object List: in this list we save the address, data,
        and boolean isIC of each object, data is initialized in the second pass*/

void addToObjectList(boolean isIC)
{
    tableObjectPtr temp =  tableObjectHead; 
    temp = (tableObjectPtr) malloc (sizeof(tableObject));  /*allocates dynamic memory for the new object*/

    if (!temp)
    {
        printf("ERROR IN ROW: %d - FAILED TO ALLOCATE MEMORY FOR OBJECT\n", row);
        exit(0);  /*if memory allocation failed the program will exit*/
    }

    /*if the object created is an order: initialize its address to be current IC (global) and isIC to true
            else (object is instruct): initialize its address to be current DC (global) and isIC to false*/

    if(isIC)
    {
        temp->address = ic;
        temp->isIC = true;
    }

    else
    {
        temp->address = dc;
        temp->isIC = false;
    }

    if (!tableObjectHead)  /*checks if list is empty (first addition to the list)*/
    {
        temp->next = NULL;
        tableObjectHead = temp;
        tableObjectTail = temp;
        tableObjectGlobal = temp;
    }

    else  /*list is not empty (second or later addition to the list)*/
    {
        tableObjectTail->next = temp;
        temp->next = NULL;
        tableObjectTail = temp;
    }
}

/*------addToSymbolList------*/
/*Function creates the 'symbol list' and adds a new symbol it when used for the first time
        and from the second time forward adds a symbol to the that list*/

/*Symbol List: in this list we save the name, address and information of each symbol,
        if the symbol is attached to an instruction (withInstruct) its address will be updated on the second pass*/

void addToSymbolList (char* word, boolean withInstruct, boolean isExtern)
{
    symbolPtr temp = symbolHead;

    word[strcspn(word, ":")] = END_OF_STRING;  /*removes the character ':' from the symbol and replaces it with '\0'*/

    if (checkList(word))  /*uses function checkList to check if the symbol was already exists*/
    {
        printf("ERROR IN ROW: %d - THE SYMBOL ENTERED ALREADY EXISTS IN SYMBOL LIST\n", row); 
        error = 1;
        return;  /*if the given symbol already exists return from function*/
    }

    temp = (symbolPtr) malloc (sizeof(symbol));  /*allocates dynamic memory for the new symbol*/

    if (!temp)
    {
        printf("ERROR IN ROW: %d - FAILED TO ALLOCATE MEMORY FOR SYMBOL\n", row);
        exit(0);  /*if memory allocation failed the program will exit*/
    }

    strcpy(temp->name, word);  /*sets the symbol name to the given word*/

    if (isExtern)
        temp->address = RESET_ZERO;  /*if the symbol is external, sets its address to be '0'*/

    else /*symbol is not external*/
    {
        if (withInstruct)
            temp->address = dc;  /*symbol is attached to an instruction, sets its address to be current DC (global)*/

        else
            temp->address = ic;  /*symbol is attached to an order, sets its address to be current IC (global)*/
    }

    /*initializes symbol information according to the parameters sent to the function
            at start isEntry is set to false for all symbols*/

    temp->withInstruct = withInstruct;
    temp->isExtern = isExtern;
    temp->isEntry = false;

    if (!symbolHead)  /*checks if list is empty (first addition to the list)*/
    {
        temp->next = NULL;
        symbolHead = temp;
        symbolTail = temp;
    }

    else  /*list is not empty (second or later addition to the list)*/
    {
        symbolTail->next = temp;
        temp->next = NULL;
        symbolTail = temp;
    }
}

/*------checkList------*/
/*Function searches through the 'symbol list' and checks if a given symbol exists
        returns true or false accordingly*/

int checkList(char* word)
{
    symbolPtr temp = symbolHead;

    while(temp)  /*loops through list and compares the names to the given word*/
    {
        if (!strcmp(temp->name, word))
            return 1;  /*if symbol is found returns true*/

        temp = temp->next;
    }

    return 0;  /*if symbol is not found returns false*/
}

/*------checkExtern------*/
/*Function searches through the 'symbol list' and check if a given symbol is external
        returns true or false accordingly*/

int checkExtern(char* word)
{
    symbolPtr temp = symbolHead;

    while(temp)  /*loops through list and compares the names to the given word*/
    {
        if (!strcmp(temp->name, word))
            if(temp->isExtern)  /*when the symbol is found checks if it's external*/
                return 1;  /*if the symbol is external returns true*/

        temp = temp->next;
    }

    return 0;  /*if the symbol is not external returns false*/
}

/*------freeLists------*/
/*Function frees the dynamic memory allocation from the 'symbol list'
        and the 'table object list'*/

void freeLists()
{
    symbolPtr symbolTemp = symbolHead;
    tableObjectPtr tableObjectTemp = tableObjectHead;

    while(symbolHead)  /*loops through 'symbol list' and frees each node*/
    {
        symbolHead = symbolHead->next;
        free(symbolTemp);
        symbolTemp = symbolHead;
    }

    while(tableObjectHead)  /*loops through 'table object list' and frees each node*/
    {
        tableObjectHead = tableObjectHead->next;
        free(tableObjectTemp);
        tableObjectTemp = tableObjectHead;
    }
}
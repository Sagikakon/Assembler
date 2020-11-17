/*------secondPass.c------*/
/*This file includes a variation of functions used in the second pass
        all functions in the file will be called only if first pass was finished successfully*/

/*--------------------------------------------------*/
/*------HEADER AND EXTERN FOR GLOBAL VARIABLES------*/
/*--------------------------------------------------*/

#include "header.h"

extern symbolPtr symbolHead;
extern tableObjectPtr tableObjectHead, tableObjectGlobal;
extern FILE* ob, *ext, *ent;
extern char matrix[MAX_LINE][MAX_LINE];
extern int ic, dc, matR, matC, error, row;

/*---------------------*/
/*------FUNCTIONS------*/
/*---------------------*/

/*------updateAddress------*/
/*Function updates the address of all symbols and table objects that received DC as
        their initial address by adding the final IC to them*/

void updateAddress()
{
    tableObjectPtr tableObjectTemp = tableObjectHead;
    symbolPtr symbolTemp = symbolHead;

    while(tableObjectTemp)  /*loops through 'table object list' and updates the address only for instruct objects*/
    {
        if(!(tableObjectTemp->isIC))
            tableObjectTemp->address = tableObjectTemp->address + ic;

        tableObjectTemp = tableObjectTemp->next;
    }

    while(symbolTemp)  /*loops through 'symbol list' and updates the address only for symbol attached to instruction*/
    {
        if(symbolTemp->withInstruct)
            symbolTemp->address = symbolTemp->address + ic;
        symbolTemp = symbolTemp->next;
    }
}

/*------executeOrderSecond------*/
/*One of the two main functions within the second pass, function handles a given command and
        its operands as needed in the second pass and initializes the data*/

/*commands command: used to decide what command is being handled (enum)*/
/*int source: divides each command to a group of legal types of source operands needed to be checked (enum)*/
/*int destination: divides each command to a group of legal types of destination operands needed to be checked (enum)*/

void executeOrderSecond(commands command, int source, int destination)
{
    int firstOperandReg = 0, firstOperand = 0, onlyDestination = 0;

    if(source > no_source_method)  /*if the given command is allowed to receive a source operand*/
    {
        tableObjectGlobal = tableObjectGlobal->next;  /*advances the global 'table object list' pointer to the next object*/
        matR++;  /*advances the global pointer on the matrix to point to the word following the command*/

        if (isVariable(matrix[matR]))  /*if source operand is a legal variable*/
        {
            if(checkList(matrix[matR]))  /*uses checkList to make sure the variable was declared in the first pass*/
            {
                if (checkExtern(matrix[matR]))  /*if the variable is external initializes information accordingly*/
                {
                    tableObjectGlobal->wordMemory.infoMemory.ARE = external;
                    strcpy(tableObjectGlobal->name, matrix[matR]);  /*initializes external variable name for the writeExtern function*/
                }

                else  /*if the variable is not external initializes information accordingly*/
                    tableObjectGlobal->wordMemory.infoMemory.ARE = relocateable;

                /*in any case sets object's binary code to be the address of the variable*/
                tableObjectGlobal->wordMemory.infoMemory.num = returnAddress(matrix[matR]);
            }

            else  /*if source operand is a variable but was not declared in first pass (checkList returns false)*/
            {
                printf("ERROR IN ROW: %d - SOURCE OPERAND IS AN UNDECALRED VARIABLE\n", row);
                error = 1;
                return;
            }

            firstOperand = 1;  /*if source operand was handled successfully turn on the flag*/
        }

        else if(source > one_source_method)  /*if source operand is not a variable checks if there are more options allowed*/
        {
            if(matrix[matR][FIRST_CHARACTER] == '#')  /*if the source operand is an immediate addressing operand*/
            {
                matR++;  /*advances the global pointer on the matrix to the number following the '#'*/

                if (checkImmediateAddressing(matrix[matR]))  /*checks if the given number is legal*/
                {
                    /*checks if the number size is within the allowed range
                            if the number size exceeds in any direction prints an error*/

                    if (atoi(matrix[matR]) > MAX_POSITIVE_CODE_NUM  || atoi(matrix[matR]) < MAX_NEGATIVE_CODE_NUM)
                    {
                        printf("ERROR IN ROW: %d - SOURCE OPERAND SIZE NOT SUPPORTED FOR IMMEDIATE ADDRESSING (EXCEEDS 12 BIT)\n", row);
                        error = 1;
                        return;
                    }

                    /*if the number is legal and within the allowed
                            range initializes information accordingly*/

                    tableObjectGlobal->wordMemory.infoMemory.ARE = absolute;
                    tableObjectGlobal->wordMemory.infoMemory.num = atoi(matrix[matR]);

                    firstOperand = 1;  /*if source operand was handled successfully turn on the flag*/
                }
            }

            else if(matrix[matR][FIRST_CHARACTER] == '*')  /*if the source operand is an indirect register operand*/
            {
                matR++;  /*advances the global pointer on the matrix to the register following the '*'*/

                if (checkAsterisk(matrix[matR]))  /*checks if the given register is legal*/
                {
                    /*if the register is legal
                            initializes information accordingly*/

                    tableObjectGlobal->wordMemory.regMemory.ARE = absolute;
                    tableObjectGlobal->wordMemory.regMemory.blank = RESET_ZERO;
                    tableObjectGlobal->wordMemory.regMemory.source = atoi(&matrix[matR][SECOND_CHARACTER]);

                    firstOperand = 1;  /*if source operand was handled successfully turn on the flag*/
                    firstOperandReg = 1;  /*source operand is a register, turns on the flag*/
                }
            }

            /*if the source operand is a direct register operand
                    and is a legal register initializes information accordingly*/

            else if (isRegister(matrix[matR]))
            {
                tableObjectGlobal->wordMemory.regMemory.ARE = absolute;
                tableObjectGlobal->wordMemory.regMemory.blank = RESET_ZERO;
                tableObjectGlobal->wordMemory.regMemory.source = atoi(&matrix[matR][SECOND_CHARACTER]);

                firstOperand = 1;  /*if source operand was handled successfully turn on the flag*/
                firstOperandReg = 1;  /*source operand is a register, turns on the flag*/
            }
        }
    }

    else  /*if the given command is not allowed to receive any type of source operand*/
        onlyDestination = 1;

    matR++;  /*advances the global pointer on the matrix to point to the word following the source operand*/

    /*if the word following the source operand is ',' and the source operand was handled successfully
            or there is only a destination operand, begins handling the destination operand*/

    if ((matrix[matR][FIRST_CHARACTER] == ',' && firstOperand) || onlyDestination)
    {
        if(destination > no_destination_method)  /*if the given command is allowed to receive a destination operand*/
        {
            if (matrix[matR][FIRST_CHARACTER] == ',')  /*if the current word is ',' advance the pointer to the following word*/
                matR++;

            /*if source operand is a legal variable advances the global 'table object list'
                    pointer to the next object handles it the same way as the source operand*/

            if (isVariable(matrix[matR]))
            {
                tableObjectGlobal = tableObjectGlobal->next;

                if(checkList(matrix[matR]))
                {
                    if (checkExtern(matrix[matR]))
                    {
                        tableObjectGlobal->wordMemory.infoMemory.ARE = external;
                        strcpy(tableObjectGlobal->name, matrix[matR]);
                    }

                    else
                        tableObjectGlobal->wordMemory.infoMemory.ARE = relocateable;
                    
                    tableObjectGlobal->wordMemory.infoMemory.num = returnAddress(matrix[matR]);
                }

                else
                {
                    printf("ERROR IN ROW: %d - DESTINATION OPERAND IS AN UNDECALRED VARIABLE\n", row);
                    error = 1;
                    return;
                }
            }

            /*if the destination operand is an indirect register operand
                    handles it the same way as the source operand*/

            else if (matrix[matR][FIRST_CHARACTER] == '*')
            {
                matR++;

                if (checkAsterisk(matrix[matR]))
                {
                    if (firstOperandReg)  /*if source operand was a register initializes information on the same object*/
                        tableObjectGlobal->wordMemory.regMemory.destination = atoi(&matrix[matR][SECOND_CHARACTER]);

                    else  /*if source operand was not a register*/
                    {
                        /*advances the global 'table object list'
                                pointer initializes information accordingly*/

                        tableObjectGlobal = tableObjectGlobal->next;
                        tableObjectGlobal->wordMemory.regMemory.ARE = absolute;
                        tableObjectGlobal->wordMemory.regMemory.blank = RESET_ZERO;
                        tableObjectGlobal->wordMemory.regMemory.destination = atoi(&matrix[matR][SECOND_CHARACTER]);
                    }
                }
            }

            /*if destination operand is not of the types above
                    checks if there are more options allowed*/

            else if(destination > two_destination_methods)
            {

                /*if the destination operand is a direct register operand
                        handles it the same way as the source operand*/

                if(isRegister(matrix[matR]))
                {
                    if (firstOperandReg)  /*if source operand was a register initializes information on the same object*/
                        tableObjectGlobal->wordMemory.regMemory.destination = atoi(&matrix[matR][SECOND_CHARACTER]);

                    else  /*if source operand was not a register*/
                    {
                        /*advances the global 'table object list'
                                pointer initializes information accordingly*/

                        tableObjectGlobal = tableObjectGlobal->next;
                        tableObjectGlobal->wordMemory.regMemory.ARE = absolute;
                        tableObjectGlobal->wordMemory.regMemory.blank = RESET_ZERO;
                        tableObjectGlobal->wordMemory.regMemory.destination = atoi(&matrix[matR][SECOND_CHARACTER]);
                    }
                }

                /*if destination operand is not of the types above
                        checks if there are more options allowed*/

                else if(destination > three_destination_methods)
                {
                    /*if the destination operand is an immediate addressing operand
                            handles it the same way as the source operand*/

                    if(matrix[matR][FIRST_CHARACTER] == '#')
                    {
                        matR++;

                        if (checkImmediateAddressing(matrix[matR]))
                        {
                            if (atoi(matrix[matR]) > MAX_POSITIVE_CODE_NUM  || atoi(matrix[matR]) < MAX_NEGATIVE_CODE_NUM)
                            {
                                printf("ERROR IN ROW: %d - DESTINATION OPERAND SIZE NOT SUPPORTED FOR IMMEDIATE ADDRESSING (EXCEEDS 12 BIT)\n", row);
                                error = 1;
                                return;
                            }

                            tableObjectGlobal = tableObjectGlobal->next;
                            tableObjectGlobal->wordMemory.infoMemory.ARE = absolute;
                            tableObjectGlobal->wordMemory.infoMemory.num = atoi(matrix[matR]);
                        }
                    }
                }
            }
        }
    }

    tableObjectGlobal = tableObjectGlobal->next;  /*advances the global 'table object list' pointer to the next object*/
}

/*------executeInstructSecond------*/
/*The second main function within the second pass, function handles a given instruction and
        its operands as needed in the second pass and initializes the data*/

void executeInstructSecond(instructions instruct)
{
    matR++;  /*advances the global pointer on the matrix to point to the operand following the instruction*/

    switch(instruct)  /*used to decide which type of instruction is being handled (enum)*/
    {
        case entry:
        {
            symbolPtr temp = symbolHead;

            if (matrix[matR+1][FIRST_CHARACTER] == END_OF_STRING)  /*checks that there is only one operand for entry*/
            {
                while(temp)  /*loops through 'symbol list' and compares the names to the given word*/
                {
                    if (!strcmp(temp->name, matrix[matR]))  /*if symbol was found*/
                    {
                        if (temp->isExtern)  /*if the given symbol is external*/
                        {
                            printf("ERROR IN ROW: %d - ENTRY FOR AN EXTERNAL SYMBOL\n", row);
                            error = 1;
                            return;
                        }

                        /*turns on isEntry for that symbol within the 'symbol list'
                                this is used by the writeEntry function*/

                        if(!temp->isEntry)
                        {
                            temp->isEntry = true;
                            return;
                        }

                        else
                            return;
                    }

                    temp = temp->next;
                }

                /*if the while loop made it all the way through without
                        finding the symbol inside the 'symbol list' prints an error*/

                printf("ERROR IN ROW: %d - ENTRY FOR AN UNDECALRED SYMBOL\n", row);
                error = 1;
                return;
            }

            else  /*if the character following the operand is not '\0' prints an error*/
            {
                printf("ERROR IN ROW: %d - TOO MANY ARGUMENTS FOR ENTRY\n", row);
                error = 1;
                return;
            }

            break;
        }

        case data:
        {
            /*loops through the matrix (by word) and
                    checks if all operands are legal numbers*/

            for(; matrix[matR][FIRST_CHARACTER] != END_OF_STRING; matR++)
            {
                if (matrix[matR][FIRST_CHARACTER] == ',')  /*when encountering ',' skips to the next word (operand)*/
                    continue;
                
                /*checks if the number size is within the allowed range
                        if the number size exceeds in any direction prints an error*/

                if (atoi(matrix[matR]) > MAX_POSITIVE_DATA_NUM || atoi(matrix[matR]) < MAX_NEGATIVE_DATA_NUM)
                {
                    printf("ERROR IN ROW: %d - NUMBER SIZE NOT SUPPORTED FOR DATA (EXCEEDS 15 BIT)\n", row);
                    error = 1;
                    return;
                }

                /*if the number is within the allowed range initializes information accordingly
                        and advances the global 'table object list' pointer to the next object*/

                tableObjectGlobal->wordMemory.all = atoi(matrix[matR]);
                tableObjectGlobal = tableObjectGlobal->next;
            }

            break;
        }

        case string:
        {
            /*loops through the word (by character)
                    and initializes the data accordingly*/

            for(matC = SECOND_CHARACTER; matrix[matR][matC] != END_OF_STRING; matC++)
            {
                if (matrix[matR][matC] == '"')  /*when encountering the second (closing) '"' initializes the object's data to '\0'*/
                    tableObjectGlobal->wordMemory.all = END_OF_STRING;

                else  /*initializes the object's binary data to the current character*/
                    tableObjectGlobal->wordMemory.all = matrix[matR][matC];

                tableObjectGlobal = tableObjectGlobal->next;  /*advances the global 'table object list' pointer to the next object*/
            }

            break;
        }

        default:  /*since extern is not handled within the second pass, a default case was created*/
            break;
    }
}

/*------returnAddress------*/
/*Function searches the 'symbol list' for a given symbol and returns its address*/

int returnAddress(char* word)
{
    symbolPtr temp = symbolHead;

    while(temp)  /*loops through list and compares the names to the given word*/
    {
        if (!strcmp(temp->name, word))
            return temp->address;  /*if the symbol was found returns its address*/

        temp = temp->next;
    }

    return 0;  /*if the symbol was not found returns false*/
}

/*------writeObject------*/
/*Function writes the output from the 'table object list' into the object file ('.ob')
        this functions is called only if the first and second pass were finished successfully*/

void writeObject()
{
    tableObjectPtr temp = tableObjectHead;

    fprintf(ob, "   %d   %d\n", ic - START_IC, dc);  /*prints the final IC and DC at the beginning of the file*/

    /*searches the list for every 'IC-Word' (isIC) and prints their address in decimal
            and their binary data in octal according to the required format*/

    while(temp)
    {
        if (temp->isIC)
            fprintf(ob, "%.4d\t%.5o\n", temp->address, temp->wordMemory.all);

        temp = temp->next;
    }

    temp = tableObjectHead;  /*resets the temp pointer to the start of the list (head)*/


    /*searches the list for every 'DC-Word' (!isIC) and prints their address in decimal
            and their binary data in octal according to the required format*/

    while(temp)
    {
        if (!(temp->isIC))
            fprintf(ob, "%.4d\t%.5o\n", temp->address, temp->wordMemory.all);

        temp = temp->next;
    }
}

/*------writeEntry------*/
/*Function writes the output from the 'symbol list' into the entry file ('.ent')
        this functions is called only if the first and second pass were finished successfully*/

void writeEntry()
{
    symbolPtr symbolTemp = symbolHead;

    /*searches the list for every entry symbol (isEntry) and prints
            their symbol name and address according to the required format*/

    while(symbolTemp)
    {
        if (symbolTemp->isEntry)
            fprintf(ent, "%s\t%d\n", symbolTemp->name, symbolTemp->address);
        
        symbolTemp = symbolTemp->next;
    }
}

/*------writeExtern------*/
/*Function writes the output from the 'table object list' into the extern file ('.ext')
        this functions is called only if the first and second pass were finished successfully*/

void writeExtern()
{
    tableObjectPtr tableObjectTemp = tableObjectHead;

    /*searches the list for every external object using the checkExtern function
            and prints their symbol name and address according to the required format*/

    while(tableObjectTemp)
    {
        if (checkExtern(tableObjectTemp->name))
            fprintf(ext, "%s\t%.4d\n", tableObjectTemp->name, tableObjectTemp->address);
        
        tableObjectTemp = tableObjectTemp->next;
    }
}

/*------intoFiles------*/
/*Function creates the output files and calls the write functions accordingly (entry and extern files will be created only if needed)
        this functions is called only if the first and second pass were finished successfully*/

void intoFiles(char* fileName)
{
    char temp[MAX_LINE];
    int entryFlag = 0, externFlag = 0;
    symbolPtr symbolTemp = symbolHead;

    /*loops through 'symbol list' and searches for an entry symbol (isEntry)
            and an external symbol (isExtern) to check if creating an entry or extern file is needed*/

    while(symbolTemp)
    {
        if (symbolTemp->isEntry && !entryFlag)  /*checks if an entry symbol is found (only once)*/
        {
            strcpy(temp, fileName);  /*copies the name of the current input file into temp*/
            ent = fopen(strcat(temp, ".ent"), "w");  /*creates the entry file while adding the correct file extension ('.ent')*/
            entryFlag = 1;  /*turn on entryFlag so the file creation will only happen once*/

            if (!ent)
            {
                printf("FAILED TO CREATE FILE %s (PROGRAM WILL EXIT)\n", temp);
                exit(0);  /*if file creation failed the program will exit*/
            }
        }

        if (symbolTemp->isExtern && !externFlag)  /*checks if an entry symbol is found (only once)*/
        {
            strcpy(temp, fileName);  /*copies the name of the current input file into temp*/
            ext = fopen(strcat(temp, ".ext"), "w");  /*creates the entry file while adding the correct file extension ('.ent')*/
            externFlag = 1;  /*turn on entryFlag so the file creation will only happen once*/

            if (!ext)
            {
                printf("FAILED TO CREATE FILE %s (PROGRAM WILL EXIT)\n", temp);
                exit(0);  /*if file creation failed the program will exit*/
            }
        }

        symbolTemp = symbolTemp->next;
    }

    strcpy(temp, fileName);  /*copies the name of the current input file into temp*/
    ob = fopen(strcat(temp, ".ob"), "w");  /*creates the entry file while adding the correct file extension ('.ent')*/

    if (!ob)
    {
        printf("FAILED TO CREATE FILE %s (PROGRAM WILL EXIT)\n", temp);
        exit(0);  /*if file creation failed the program will exit*/
    }

    /*calls for all the write functions each file will have the output written (if needed)
            after each function call the file that was taken care of will be closed*/

    writeObject(&tableObjectHead);
    fclose(ob);

    if (ent)  /*write only if entry file was created*/
    {
        writeEntry(&symbolHead);
        fclose(ent);
    }

    if (ext)  /*write only if extern file was created*/
    {
        writeExtern(&tableObjectHead);
        fclose(ext);
    }
}
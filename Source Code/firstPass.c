/*------firstPass.c------*/
/*This file includes a variation of functions used in the first pass, if any of the
        functions in this file turns on the error flag the program will end after the first pass is finished*/

/*--------------------------------------------------*/
/*------HEADER AND EXTERN FOR GLOBAL VARIABLES------*/
/*--------------------------------------------------*/

#include "header.h"

extern FILE* fd;
extern tableObjectPtr tableObjectTail;
extern char matrix[MAX_LINE][MAX_LINE];
extern int ic, dc, matR, matC, error, row, firstPass;

/*---------------------*/
/*------FUNCTIONS------*/
/*---------------------*/

/*------openFiles------*/
/*Function opens the input file and initiates the first and second pass (if needed)
        respectively then calls the write into files function if needed*/

void openFiles(char* fileName)
{
    char temp[MAX_LINE];

    /*copies the name of the current input file into temp, adds the file extension
            for the input ('.as') then opens the file with the extension for read*/

    strcpy(temp, fileName);
    strcat(temp, ".as");
    fd = fopen(temp, "r");

    if (!fd)
    {
        printf("FAILED TO OPEN FILE %s (PROGRAM WILL EXIT)\n", temp);
        exit(0);  /*if file opening failed the program will exit*/
    }

    initiatePass(fd);  /*initiates the first pass with the current input file as an argument*/

    /*if first pass was finished successfully (error flag was not turned on)
            sets up and initiates the second pass*/

    if(!error)
    {
        firstPass = 0;  /*turns off first pass flag (used by the handle input functions)*/
        updateAddress();  /*calls the updateAddress function to update DC objects to their final address*/
        rewind(fd);  /*rewind the file pointer to the beginning of the file for the second pass*/
        initiatePass(fd);  /*initiates the second pass with the current input file as an argument*/
    }


    /*if first pass and / or second pass were not finished successfully (error flag was turned on)
            prints an output to the terminal indicating in which file the errors printed are located*/

    if (error)
    {
        printf("\nALL ERRORS AND WARNINGS ABOVE ARE WITHIN FILE: %s\n", temp);
        printf("\n-------------------------------------------------------------------------------------\n\n");
    }

    /*if first pass and second pass were finished successfully (error flag was not turned on)
            calls the intoFiles function to create and print the required files and information*/

    if (!error)
        intoFiles(fileName);

    fclose(fd);  /*closes the current input file after all is done*/
}

/*------initiatePass------*/
/*Function initiates the pass by scanning the given input file (by row) and sends each row scanned to the
        first of the handle input functions. function is called each time a pass needs to begin (first or second)*/

void initiatePass(FILE* fd)
{
    char str[1028];

    row = RESET_ZERO;  /*resets the global row number to '0' for (used for error printing)*/

    while (fgets(str, sizeof str, fd) != NULL)  /*loops through the current input file and scans each sentence to 'str'*/
    {
        row++;
        str[strcspn(str, "\n")] = END_OF_STRING;  /*places '\0' at the end of each sentence*/

        if(strlen(str) <= MAX_LINE)  /*if sentence has a legal length (80 chars)*/
            splitString(str);  /*calls the first function that handles input*/

        else  /*if sentence exceeds the legal length*/
        {
            printf("ERROR IN ROW: %d - SENTENCE EXCEEDS 80 CHARS\n", row);
            error = 1;  /*in this error there is no return so the function will continue to the next sentence*/
        }
    }
}

/*------executeOrder------*/
/*One of the two main functions within the first pass, function handles a given command and
        its operands as needed in the first pass and adds objects to the 'table object list' when needed*/

/*commands command: used to decide what command is being handled (enum)*/
/*int source: divides each command to a group of legal types of source operands needed to be checked (enum)*/
/*int destination: divides each command to a group of legal types of destination operands needed to be checked (enum)*/

void executeOrder(commands command, int source, int destination)
{
    int firstOperandReg = 0, firstOperand = 0, onlyDestination = 0;
    tableObjectPtr temp;

    addToObjectList(true);  /*adds a new object to the 'table object list' to store the information of the given command*/
    temp = tableObjectTail;

    /*initializes the information for the current
            object (the given command) accordingly*/

    temp->wordMemory.all = RESET_ZERO;
    temp->wordMemory.orderMemory.ARE = absolute;
    temp->wordMemory.orderMemory.opcode = command;

    ic++;  /*advances the global IC*/

    if(source > no_source_method)  /*if the given command is allowed to receive a source operand*/
    {
        matR++;  /*advances the global pointer on the matrix to point to the word following the command*/

        if (isVariable(matrix[matR]))  /*if source operand is a legal variable*/
        {
            temp->wordMemory.orderMemory.source = direct_addressing;  /*initializes the current object's source*/
            addToObjectList(true);  /*adds a new object to the 'table object list' to be initialized in the second pass*/
            ic++;  /*advances the global IC*/
            firstOperand = 1;  /*if source operand was handled successfully turn on the flag*/
        }

        else if(source > one_source_method)  /*if source operand is not a variable checks if there are more options allowed*/
        {
            if(matrix[matR][FIRST_CHARACTER] == '#')  /*if the source operand is an immediate addressing operand*/
            {
                matR++;  /*advances the global pointer on the matrix to the number following the '#'*/

                if (checkImmediateAddressing(matrix[matR]))  /*checks if the given number is legal*/
                {
                    /*same algorithm as the before but with a different source type*/

                    temp->wordMemory.orderMemory.source = immediate_addressing;
                    addToObjectList(true);
                    ic++;
                    firstOperand = 1;
                }
            }

            else if(matrix[matR][FIRST_CHARACTER] == '*')  /*if the source operand is an indirect register operand*/
            {
                matR++;  /*advances the global pointer on the matrix to the register following the '*'*/

                if (checkAsterisk(matrix[matR]))  /*checks if the given register is legal*/
                {
                    /*same algorithm as the before but with a different source type
                            also turns on the flag for first operand is a register*/

                    temp->wordMemory.orderMemory.source = register_indirect_addressing;
                    addToObjectList(true);
                    ic++;
                    firstOperand = 1;
                    firstOperandReg = 1;  /*source operand is a register, turns on the flag*/
                }
            }

            else if (isRegister(matrix[matR]))  /*if the source operand is a direct addressing operand and is a legal register*/
            {
                /*same algorithm as the before but with a different source type
                        also turns on the flag for first operand is a register*/

                temp->wordMemory.orderMemory.source = register_direct_addressing;
                addToObjectList(true);
                ic++;
                firstOperand = 1;
                firstOperandReg = 1;  /*source operand is a register, turns on the flag*/
            }

            else  /*if source operand is none of the above types prints an error*/
            {
                printf("ERROR IN ROW: %d - SOURCE OPERAND IS OF UNKNOWN TYPE FOR THIS COMMAND\n", row);
                error = 1;
                return;
            }
        }

        else  /*if source group allows only variable type but operand is not a legal variable prints an error*/
        {
            printf("ERROR IN ROW: %d - SOURCE OPERAND IS NOT A LEGAL VARIABLE\n", row);
            error = 1;
            return;
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

            if (isVariable(matrix[matR]))  /*if destination operand is a legal variable*/
            {
                temp->wordMemory.orderMemory.destination = direct_addressing;  /*initializes the current object's destination*/
                addToObjectList(true);  /*adds a new object to the 'table object list' to be initialized in the second pass*/
                ic++;  /*advances the global IC*/
            }

            else if (matrix[matR][FIRST_CHARACTER] == '*')  /*if the destination operand is an indirect register operand*/
            {
                matR++;  /*advances the global pointer on the matrix to the register following the '*'*/

                if (checkAsterisk(matrix[matR]))  /*if the given register is legal*/
                {
                    /*initializes the current object's destination*/

                    temp->wordMemory.orderMemory.destination = register_indirect_addressing;  

                    /*adds a new object to the 'table object list' and increases
                            the IC only if the source operand was not a register*/

                    if (!firstOperandReg)
                    {
                        addToObjectList(true);
                        ic++;
                    }
                }
            }

            /*if destination operand is not of the types above
                    checks if there are more options allowed*/

            else if(destination > two_destination_methods)
            {
                if(isRegister(matrix[matR]))  /*if the given register is legal*/
                {
                    /*initializes the current object's destination*/

                    temp->wordMemory.orderMemory.destination = register_direct_addressing;

                    /*adds a new object to the 'table object list' and increases
                            the IC only if the source operand was not a register*/

                    if (!firstOperandReg)
                    {
                        addToObjectList(true);
                        ic++;
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
                            temp->wordMemory.orderMemory.destination = immediate_addressing;
                            addToObjectList(true);
                            ic++;
                            firstOperand = 1;
                        }
                    }
                }

                else  /*if destination operand is none of the above types prints an error*/
                {
                    printf("ERROR IN ROW: %d - DESTINATION OPERAND IS OF UNKNOWN TYPE FOR THIS COMMAND\n", row);
                    error = 1;
                    return;
                }
            }

            else  /*if destination operand is none of the above types prints an error*/
            {
                printf("ERROR IN ROW: %d - DESTINATION OPERAND IS OF UNKNOWN TYPE FOR THIS COMMAND\n", row);
                error = 1;
                return;
            }
        }

        /*if there is no destination method checks if the
                character following the source operand is '\0'*/

        else if (matrix[matR][FIRST_CHARACTER] != END_OF_STRING)
        {
            printf("ERROR IN ROW: %d - TOO MANY ARGUMENTS\n", row);
            error = 1;
            return;
        }
    }

    else  /*if there was a source operand but no ',' was found between it and the destination operand*/
    {
        printf("ERROR IN ROW: %d - NO COMMA BETWEEN ARGUMENTS\n", row);
        error = 1;
        return;
    }

    if (matrix[matR+1][FIRST_CHARACTER] != END_OF_STRING)  /*checks if the character following the operands is '\0'*/
    {
        printf("ERROR IN ROW: %d - TOO MANY ARGUMENTS\n", row);
        error = 1;
        return;
    }
}

/*------executeInstruct------*/
/*The second main function within the first pass, function handles a given instruction and its operands
        as needed in the first pass and adds symbols to the 'symbol list' and object to the 'table object list' when needed*/

void executeInstruct(instructions instruct)
{
    matR++;  /*advances the global pointer on the matrix to point to the operand following the instruction*/

    switch(instruct)  /*used to decide which type of instruction is being handled (enum)*/
    {
        case myExtern:
        {
            if (matrix[matR+1][FIRST_CHARACTER] == END_OF_STRING)  /*checks that there is only one operand for extern*/
            {
                if (isVariable(matrix[matR]))  /*if operand is a legal variable*/
                {
                    /*if symbol already exists in symbol list and is external returns from function
                            else adds the symbol to the 'symbol list'*/
                    
                    if (checkList(matrix[matR]) && checkExtern(matrix[matR]))
                            break;
                
                    addToSymbolList(matrix[matR], false, true);
                }

                else  /*if source operand is not a legal variable*/
                    printf("ERROR IN ROW: %d - EXTERN OPERAND IS NOT A LEGAL VARIABLE\n", row);

                break;
            }

             else  /*if the character following the operand is not '\0' prints an error*/
            {
                printf("ERROR IN ROW: %d - TOO MANY ARGUMENTS FOR EXTERN\n", row);
                error = 1;
                return;
            }
        }

        case data:
        {
            int commaFlag = 0;

            if (matrix[matR][FIRST_CHARACTER] == ',')  /*if input for data begins with a comma*/
            {
                printf("ERROR IN ROW: %d - ARGUMENT FOR DATA BEGINS WITH A COMMA\n", row);
                error = 1;
                return;
            }

            /*loops through the matrix (by word), checks if all operands are legal numbers
                    and for each operand adds an object to the 'table object list' and increases the DC*/

            for(; matrix[matR][FIRST_CHARACTER] != END_OF_STRING; matR++)
            {
                if(matrix[matR][FIRST_CHARACTER] != ',')  /*skips commas between operands*/
                {
                    checkImmediateAddressing(matrix[matR]);
                    addToObjectList(false);
                    commaFlag = 0;  /*resets comma flag*/
                    dc++;
                }

                else  /*when encountering ','*/
                {
                    if (commaFlag)  /*if comma flag is turned on (second comma in a row)*/
                    {
                        printf("ERROR IN ROW: %d - ARGUMENT FOR DATA HAS NO OPERAND BETWEEN COMMAS\n", row);
                        error = 1;
                        return;
                    }

                    commaFlag = 1;  /*turns on comma flag*/
                }
            }

            if(matrix[matR-1][FIRST_CHARACTER] == ',')  /*if input for data ends with a comma*/
            {
                printf("ERROR IN ROW: %d - ARGUMENT FOR DATA ENDS WITH A COMMA\n", row);
                error = 1;
                return;
            }

            break;
        }

        case string:
        {
            if (matrix[matR][FIRST_CHARACTER] != '"')  /*if input for string does not begin with quotation*/
            {
                printf("ERROR IN ROW: %d - ARGUMENT FOR STRING STARTS WITH ILLEGAL CHAR\n", row);
                error = 1;
                return;
            }

            else  /*if input for string starts with quotation*/
            {
                /*loops through the word (by character), checks if all characters are legal (printable)
                    and for each character adds an object to the 'table object list' and increases the DC
                        plus one more time for the ending quotation (holder for '\0')*/

                for(matC = 1; matrix[matR][matC] != END_OF_STRING; matC++)
                {
                    if (matrix[matR][matC] < FIRST_PRINTABLE_CHAR || matrix[matR][matC] > LAST_PRINTABLE_CHAR)
                    {
                        printf("ERROR IN ROW: %d - ARGUMENT FOR STRING HAS AN UNPRINTABLE CHAR\n", row);
                        error = 1;
                        return;
                    }

                    addToObjectList(false);
                    dc++;
                }

                if (matrix[matR][matC-1] != '"')  /*if input for string does not end with quotation*/
                {
                    printf("ERROR IN ROW: %d - ARGUMENT FOR STRING ENDS WITH ILLEGAL CHAR\n", row);
                    error = 1;
                    return;
                }
            }

            break;
        }

        default:
            break;
    }
}

/*------reset------*/
/*Function all resets necessary to the global variables
        and lists before proceeding to the next input file*/

void reset()
{
    freeLists();
    ic = START_IC;
    dc = RESET_ZERO;
    error = RESET_ZERO;
    firstPass = 1;
}
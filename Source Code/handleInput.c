/*------handleInput.c------*/
/*This file includes a variation of functions used to handle a given string (sentence)
        from the input file each time it is sent to them by the initiatePass function*/

/*--------------------------------------------------*/
/*------HEADER AND EXTERN FOR GLOBAL VARIABLES------*/
/*--------------------------------------------------*/

#include "header.h"

extern int matR;
extern int matC;
extern int row;
extern int error;
extern int ctr;
extern int firstPass;
extern char matrix[MAX_LINE][MAX_LINE];

/*---------------------*/
/*------FUNCTIONS------*/
/*---------------------*/

/*------splitString------*/
/*Function receives a sentence from the initiatePass function and divides it into a matrix
        in which every row (matR) is a different word and every column (matC) is that word's characters*/

/*this function does numerous testings on the sentence and is the initial
        tester for the sentence's syntax (there are more legality tests along the way)*/

void splitString(char str[])
{
    int ctr;
    int quotationFlag = 0;

    /*resets the global index to '0' on each sentence this function is called with
    --matR: each time matR advances the matrix handles a new word
    --matC: each time matC advances the matrix handles a new character
    --ctr: character index through the original sentence (str)*/

    matR = RESET_ZERO;
    matC = RESET_ZERO;
    ctr = RESET_ZERO;

    /*if the first character is not a legal character to
            begin a sentence with, prints an error*/

    if ((!isalpha(str[ctr])) && str[ctr] != END_OF_STRING && str[ctr] != '\t' && str[ctr] != '.' && str[ctr] != ';' && str[ctr] != ' ')
    {
        printf("ERROR IN ROW: %d - SENTENCE STARTS WITH AN ILLEGAL CHAR\n", row);
        error = 1;
        return;
    }

    if (str[ctr] == ';')  /*if sentence starts ';' (comment) return from function*/
        return;
    

    /*loops through the str (original sentence) by character and does numerous
            testings on each character deciding where on the matrix it should be entered*/

    for(ctr = 0; ctr <= (strlen(str)); ctr++)
    {
        /*when encountering '"' (for .string) add to the matrix all that is within the first
                '"' and the second '"' as a whole word*/

        if (str[ctr] == '"')
        {
            if (quotationFlag)  /*second time encountering '"'*/
            {
                matrix[matR][matC] = str[ctr];  /*assigns '"' to current position on the matrix*/

                /*if the character following the second '"' is not '\0' prints an error*/

                if (str[ctr+1] != '\t' && str[ctr+1] != ' ' && str[ctr+1] != END_OF_STRING)
                {
                    printf("ERROR IN ROW: %d - ARGUMENT FOR STRING ENDS WITH ILLEGAL CHAR\n", row);
                    error = 1;
                    return;
                }

                matC++;  /*advances the character index on the matrix*/
                matrix[matR][matC] = END_OF_STRING;  /*assigns '\0' at the end of the current word*/
                matR++;  /*advances the word index on the matrix*/
            }

            else  /*first time encountering '"'*/
            {
                matrix[matR][matC] = str[ctr];  /*assigns '"' to current position on the matrix*/
                matC++;  /*advances the character index on the matrix*/
                quotationFlag = 1;  /*turns on quotation flag*/
            }
        }

        /*when encountering either a white character or '\0' or '\t' if character index is 0 (new word)
            continues to the next character, if character index is bigger than 0 (during a word)
                assigns '\0' at the end of the current word*/

        else if((str[ctr] == ' '|| str[ctr] == END_OF_STRING || str[ctr]== '\t') && !quotationFlag)
        {
            if(matC > 0)
            {
                matrix[matR][matC]=END_OF_STRING;  /*assigns '\0' at the end of the current word*/
                matR++;  /*advances the word index on the matrix*/
                matC = RESET_ZERO;  /*resets the character index on the matrix*/
            }
        }

        /*when encountering either '#' or '*' or ',' the function will divide it into
                a different word, this is used later for operand testings*/

        else if ((str[ctr]=='#' || str[ctr]=='*' || str[ctr]==',') && !quotationFlag)
        {
            /*this test is used to ensure there is no white character between the '#'
                    symbol and the number assigned to it, prints an error if there is*/

            if (str[ctr] == '#' && (str[ctr+1] == ' ' || str[ctr+1] == '\t'))
            {
                printf("ERROR IN ROW: %d - IMMEDIATE ADDRESSING ARGUMENT HAS WHITE CHAR BETWEEN HASHTAG AND NUMBER\n", row);
                error = 1;
                return;
            }

            /*if character index is bigger than 0 (during a word) assigns '\0' at the end of the current word
                    and advances the word index on the matrix so it will assign that character to a new word*/

            if (matC > 0)
            {
                matrix[matR][matC]=END_OF_STRING;  /*assigns '\0' at the end of the current word*/
                matR++;  /*advances the word index on the matrix*/
                matC = RESET_ZERO;  /*resets the character index on the matrix*/
            }

            matrix[matR][matC]=str[ctr];  /*assigns the character to the current position on the matrix*/
            matC++;  /*advances the character index on the matrix*/
            matrix[matR][matC]=END_OF_STRING;  /*assigns '\0' at the end of the current word*/
            matR++;  /*advances the word index on the matrix*/
            matC = RESET_ZERO;  /*resets the character index on the matrix*/
        }

        /*if the current character is none of the 'special' characters above
                assigns it to the current position on the matrix and advance the character index*/
        else
        {
            matrix[matR][matC]=str[ctr];  /*assigns the character to the current position on the matrix*/
            matC++;  /*advances the character index on the matrix*/
        }
    }

    matrix[matR][matC] = END_OF_STRING;  /*assigns '\0' at the end of the current word*/
    matrix[matR+1][FIRST_CHARACTER] = END_OF_STRING;  /*assigns '\0' at the end of the current matrix*/

    /*calls for checkSentence to run tests on the new global matrix
            and sends matR (number of words in the matrix) as a parameter*/
    
    checkSentence(matR);
}

/*------checkSentence------*/
/*Function receives a sentence divided into a matrix from the spliString function
        in which every row (matR) is a different word and every column (matC) is that word's characters*/

/*this function does numerous testings on the first and second word of the
        sentence eventually deciding which one should be sent to which function*/

void checkSentence(int numOfWords)
{
    int commaFlag = 0, labelFlag = 0;

    matR = RESET_ZERO;  /*resets word number to '0'*/
    matC = RESET_ZERO;  /*resets character number to '0'*/

    if (matrix[FIRST_WORD][FIRST_CHARACTER] == END_OF_STRING)  /*if sentence is blank returns from function*/
        return;


    /*checks if the sentence has more than one comma in it which is illegal
            test will not be made if the first or second word of that sentence is '.data'*/

    if (strcmp(matrix[FIRST_WORD], ".data"))  /*if first word is not '.data'*/
    {
        if (strcmp(matrix[SECOND_WORD], ".data"))  /*if second word is not '.data'*/
        {
            /*loops through the matrix (by word) and searches for ','
                    when encountering the first one turns on the flag, on the second one prints an error*/

            for(; matR < numOfWords; matR++)
            {
                if (matrix[matR][FIRST_CHARACTER] == ',')
                {
                    if (commaFlag == 1)  /*if encountered ',' for the second time*/
                    {
                        printf("ERROR IN ROW: %d - SENTENCE IS NOT .DATA AND HAS MORE THAN ONE COMMA\n", row);
                        error = 1;
                        return;
                    }

                    commaFlag = 1;  /*if encountered ',' for the first time*/
                }
            }   
        }
    }

    /*if the first word is a label, does numerous testings and eventually decides
            whether or not it should be added to the 'symbol list' and with what parameters*/

    if (isLabel(matrix[FIRST_WORD]))
    {
        if(isReserved(matrix[FIRST_WORD]))  /*if the first word is reserved prints an error*/
        {
            printf("ERROR IN ROW: %d - %s IS AN ASSEMBLY RESERVED WORDS\n", row, matrix[FIRST_WORD]);
            error = 1;
            return;
        }

        if (matrix[SECOND_WORD][FIRST_CHARACTER] == END_OF_STRING)  /*if the word following the label is '\0' prints an error*/
        {
            printf("WARNING IN ROW: %d - LABEL %s HAS NOTHING ATTACHED (WILL NOT BE ADDED TO SYMBOL LIST)\n", row, matrix[FIRST_WORD]);
            return;
        }

        if (firstPass)  /*if the first word is a legal label and we are during the first pass*/
        {
            /*if the word following the label is a known command
                adds it to the 'symbol list'*/

            if (isCommand(matrix[SECOND_WORD]) != FAIL)
                addToSymbolList(matrix[FIRST_WORD], false, false);

            /*if the word following the label is either '.data' or '.string'
                    adds it to the 'symbol list'*/

            else if(isInstruct(matrix[SECOND_WORD]) == data || isInstruct(matrix[SECOND_WORD]) == string)
                addToSymbolList(matrix[FIRST_WORD], true, false);

            /*if the word following the label is either '.entry' or '.extern'
                    prints a warning but and does not add it to the 'symbol list'*/

            else if(isInstruct(matrix[SECOND_WORD]) == entry || isInstruct(matrix[SECOND_WORD]) == myExtern)
                printf("WARNING IN ROW: %d - LABEL %s IS ATTACHED TO EITHER ENTRY OR EXTERN (WILL NOT BE ADDED TO SYMBOL LIST)\n", row, matrix[FIRST_WORD]);
        }

        labelFlag = 1;  /*if the first word was a label and was assigned successfully turns on the flag*/
    }

    /*if the first word was not a label
            sends the first word to the checkCommand function*/
    else
    {
        matR = FIRST_WORD;
        checkCommand(matrix[FIRST_WORD]);
    }

    /*if the first word was a label and was assigned successfully
            sends the second word to the checkCommand function*/

    if (labelFlag == 1)
    {
        matR = SECOND_WORD;
        checkCommand(matrix[SECOND_WORD]);
    }
}

/*------checkCommand------*/
/*Function receives either the first or second word of the current sentence from
        the checkSentence function and decide whether it is or not a known command or instruct*/

/*if the given word is a known command or instruct the function does numerous
        testings to the decide if it needs to executed and with what parameters*/

void checkCommand(char* word)
{
    int source, destination;
    commands command = isCommand(word);  /*if the given word is a known command then its number will be assigned*/
    instructions instruct = isInstruct(word);  /*if the given word is a known instruction then its number will be assigned*/

    if (command != FAIL)  /*if a command number was successfully assigned to 'command' variable*/
    {
        /*divides all the known commands into groups by the number of source operand methods available to them
                and the number of destination operand methods available to them*/

        /*source groups:
        --mov, cmp, add, sub - all four methods allowed
        --lea - one method allowed (immediate addressing)
        --clr, not, inc, dec, jmp, bne, red, prn, jsr, rts, stop - no methods allowed*/

        /*destination groups:
        --mov, add, sub, lea, clr, not, inc, dec, red - three methods allowed (immediate, direct, register indirect)
        --cmp, prn - all four methods allowed
        --rts, stop - no methods allowed
        --jmp, bne, jsr - two methods allowed (immediate, direct)*/

        if (command >= mov && command <= sub)
            source = four_source_methods;

        else if (command == lea)
            source = one_source_method;

        else
            source = no_source_method;

        if ((command >= mov && command <= dec && command != cmp) || command == red)
            destination = three_destination_methods;

        else if (command == cmp || command == prn)
            destination = four_destination_methods;

        else if (command >= rts)
            destination = no_destination_method;

        else
            destination = two_destination_methods;

        /*sends the given command and the defined source and destination group
                to either executeOrder or executeOrderSecond accordingly*/

        if(firstPass)
            executeOrder(command, source, destination);

        else
            executeOrderSecond(command, source, destination);
    }

    /*if command was not assigned (FAIL) checks if an instruction
            number was successfully assigned to 'instruct' variable*/

    else if (instruct == data || instruct == string)
    {
        if (firstPass)
            executeInstruct(instruct);

        else
            executeInstructSecond(instruct);
    }

    else if (instruct == entry)
    {
        if (!firstPass)  /*executes entry only during the second pass*/
                executeInstructSecond(instruct);
    }

    else if (instruct == myExtern)
    {
        if (firstPass)  /*executes extern only during the first pass*/
            executeInstruct(instruct);
    }

    else  /*if neither command nor instruct were successfully assigned, prints an error*/
    {
        printf("ERROR IN ROW: %d - %s IS NOT A LEGAL LABEL NOR A KNOWN INSTRUCT NOR COMMAND\n", row, word);
        error = 1;
        return;
    }
}

/*------isCommand------*/
/*Function checks if the given word is a known command
        returns either false or the number of that command */

int isCommand(char* word)
{
    int i;
    char* commandNames[] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};

    /*loops through the command names array while comparing it to the
            given word, if a match is found returns the index of the command*/

    for (i = 0; i < NUM_OF_COMMANDS; i++)
        if (!strcmp(commandNames[i], word))
            return i;

    return FAIL;  /*if failed returns -1 since command "mov" is 0*/
}

/*------isInstruct------*/
/*Function checks if the given word is a known instruction
        returns either false or the number of that instruction */

int isInstruct(char* word)
{
    int i;
    char* instructionNames[] = {".data", ".entry", ".extern", ".string"};

    /*loops through the instruction names array while comparing it to the
            given word, if a match is found returns the index (plus one) of the command*/

    for (i = 0; i < NUM_OF_INSTRUCTIONS; i++)
        if (!strcmp(instructionNames[i], word))
            return i+1;

    return 0;  /*if instruction was not found returns false*/
}

/*------isReserved------*/
/*Function checks if the given word is reserved
        by the language, returns true or false accordingly*/

int isReserved(char* word)
{
    int i;
    char temp[MAX_LINE];
    strcpy(temp, word);
    temp[strcspn(temp, ":")] = END_OF_STRING;  /*replaces the ':' at the end of the word to '\0' for comparing*/

    /*loops through the word (by character) and convert it to lower case
            so function will work even if the given word was written with alternating cases*/

    for(i = 0; temp[i] != END_OF_STRING; i++)
        temp[i] = tolower(temp[i]);

    if(isRegister(temp))  /*if the given word is a known register returns true (is reserved)*/
        return 1;

    if(isCommand(temp) != FAIL)  /*if the given word is a known command returns true (is reserved)*/
        return 1;

    /*if the given word is a known instruction returns true (is reserved)*/

    else if(!strcmp("string", temp) || !strcmp("entry", temp) || !strcmp("data", temp) || !strcmp("extern", temp))
        return 1;

    return 0;  /*if the given word is none of the above returns false (is not reserved)*/
}
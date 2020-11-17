/*------generalFunctions.c------*/
/*This file includes a variation of helpful functions
        used across the program, all receiving the same type of argument*/

/*--------------------------------------------------*/
/*------HEADER AND EXTERN FOR GLOBAL VARIABLES------*/
/*--------------------------------------------------*/

#include "header.h"
extern int error, row;

/*---------------------*/
/*------FUNCTIONS------*/
/*---------------------*/

/*------isRegister------*/
/*Function checks if the given word is a register
        returns true or false accordingly*/

int isRegister(char* word)
{
    /*checks if the first character is 'r'
            if true: checks if the second character is between 0-7 and if the third character is '\0'*/

    if (word[FIRST_CHARACTER] == 'r') 
    {
        if (word[SECOND_CHARACTER] >= '0' && word[SECOND_CHARACTER] <= '7' && word[THIRD_CHARACTER] == END_OF_STRING )
            return 1;  /*if both second and third characters are legal returns true*/
    }

    return 0;  /*if the given word is not a legal register returns false*/
}

/*------isVariable------*/
/*Function checks if the given word is a legal variable
        returns true or false accordingly*/

int isVariable(char* word)
{
    int i;

    if(isalpha(word[FIRST_CHARACTER]))  /*checks if the first character is an alphabet letter*/
    {
        if (isRegister(word))  /*returns false if the given variable is a register*/
            return 0;

        for (i = 0; word[i] != END_OF_STRING; i++)  /*loops through the word (by character) and checks if all characters are legal*/
        {
            if (!(isalpha(word[i]) || isdigit(word[i])))  /*only alphabet letters and digits are allowed*/
            {
                printf("ERROR IN ROW: %d - ILLEGAL VALUE FOR A VARIABLE\n", row);
                error = 1;
                return 0;  /*in case of an illegal character prints an error and returns false*/
            }
        }

        return 1;  /*in case of a legal variable returns true*/
    } 

    return 0;  /*if the first character is illegal returns false*/
}

/*------isLabel------*/
/*Function checks if the given word is a legal label
        returns true or false accordingly*/

int isLabel(char* word)
{
    if(strlen(word)-1 > MAX_LABEL)  /*checks if the given word (minus the ':') is over 31 characters*/
    {
        printf("ERROR IN ROW: %d - LABEL EXCEEDS 31 CHARS\n", row);
        error = 1;
        return 0;  /*if the label is too long returns false*/
    }

    if (!(strchr(word, ':')))   /*checks for the existence of ':' in the given word*/
        return 0;  /*if there none is found returns false*/

    if (!isalpha(*word))    /*checks if the first character is an alphabet letter*/
        return 0;  /*if the first character is illegal returns false*/

    /*loops through the word (by character) and checks 
            if all characters are legal (stops at the first ':')*/

    while(*word != ':')
    {
        if (!(isalpha(*word) || isdigit (*word)))  /*only alphabet letters and digits are allowed*/
            return 0;   /*in case of an illegal character prints an error and returns false*/

        word++;
    }

    if (!(*(word+1) == END_OF_STRING))  /*checks if the character after the ':' is '\0' and if not returns false*/
        return 0;

    return 1;  /*returns true if function made it all the way through without returning false*/
}

/*------checkAsterisk------*/
/*Function checks if the word following the '*' is a legal register
        returns true or false accordingly*/

int checkAsterisk(char* word)
{
    if (!isRegister(word))  /*use of function isRegister on the given word*/
    {
        printf("ERROR IN ROW: %d - OPERAND AFTER INDIRERCT REGISTER ADDRESSING IS NOT A REGISTER\n", row);
        error = 1;
        return 0;  /*if the given word isn't a legal register returns false*/
    }

    return 1;  /*if the given word is a legal register returns true*/
}

/*------checkImmediateAddressing------*/
/*Function checks if the number following the '#' is a legal number
        returns true or false accordingly*/

int checkImmediateAddressing(char* word)
{
    int i;

    /*checks if the first character is legal ('+' || '-' || digit)*/

    if(!(word[FIRST_CHARACTER] == '+' || word[FIRST_CHARACTER] == '-' || isdigit(word[FIRST_CHARACTER])))
    {
        printf("ERROR IN ROW: %d - IMMEDIATE ADDRESSING OPERAND STARTS WITH AN ILLEGAL CHAR\n", row);
        error = 1;
        return 0;  /*in case of an illegal character prints an error and returns false*/
    }

    /*in case of the first character being a sign
            checks if there are characters following it*/

    if (!isdigit(word[FIRST_CHARACTER]) && word[SECOND_CHARACTER] == END_OF_STRING)
    {
        printf("ERROR IN ROW: %d - IMMEDIATE ADDRESSING OPERAND HAS NO NUMBERS FOLLOWING THE SIGN\n", row);
        error = 1;
        return 0;  /*if there are no character following the sign returns false*/
    }

    /*loops through the word (by character) and
            checks if all characters are legal numbers*/

    for (i = 1; word[i] != END_OF_STRING; i++)
    {
        if(!isdigit(word[i]))
        {
            printf("ERROR IN ROW: %d - CHAR IN IMMEDIATE ADDRESSING OPERAND IS NOT A NUMBER\n", row);
            error = 1;
            return 0;  /*if a character is not a digit returns false*/
        }
    } 

    return 1;  /*returns true if function made it all the way through without returning false*/
}
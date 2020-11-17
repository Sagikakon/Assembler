assembler :  assembler.o handleInput.o handleLists.o firstPass.o secondPass.o generalFunctions.o 
	gcc -g -ansi -Wall -pedantic  assembler.o handleInput.o handleLists.o firstPass.o secondPass.o generalFunctions.o -o  assembler
assembler.o :  assembler.c header.h
	gcc -c -ansi -Wall -pedantic  assembler.c -o  assembler.o
handleLists.o : handleLists.c header.h
	gcc -c -ansi -Wall -pedantic handleLists.c -o handleLists.o
generalFunctions.o : generalFunctions.c header.h
	gcc -c -ansi -Wall -pedantic generalFunctions.c -o generalFunctions.o
handleInput.o : handleInput.c header.h
	gcc -c -ansi -Wall -pedantic handleInput.c -o handleInput.o
secondPass.o : secondPass.c header.h
	gcc -c -ansi -Wall -pedantic secondPass.c -o secondPass.o
firstPass.o : firstPass.c header.h
	gcc -c -ansi -Wall -pedantic firstPass.c -o firstPass.o
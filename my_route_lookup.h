
#include <stdlib.h>

#define LENGTH_TABLE_1 16777216
#define MIN_POINTER 32001
#define ARG_ERROR -1

void wrong_arguments();

/***************************************************
 * This method manages the input argument errors 
 * an user could make.
 **************************************************/
void wrong_arguments(){
    printf("Oops! Error while introducing the arguments. The correct order is:");
    printf("\t<string> FIB file.\n\t<string> Input file.");
    exit(ARG_ERROR);
}
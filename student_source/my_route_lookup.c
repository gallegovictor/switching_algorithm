#include "io.h"
#include "utils.h"
#include "my_route_lookup.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>
#include <sys/resource.h>




uint16_t static my_table[LENGTH_TABLE_1];

int main(int argc, char * argv[]){
    if(argc!=3){
        wrong_arguments();
    }

    uint16_t static my_table[LENGTH_TABLE_1];
    char *routing_table_file = argv[1];
	char *input_file= argv[2];
    int error, i;

    if((error = initializeIO(routing_table_file, input_file))!=OK) printIOExplanationError(error);
	//Filling the table
	for(i=0; i<LENGTH_TABLE_1; i++){
		my_table[i] = 0x0000;
	}
    //Initializing variables for the tables 
    uint32_t prefix = 0;
	int prefixLength = 0;
	int outInterface = 0;
	uint32_t ip_addr;

}



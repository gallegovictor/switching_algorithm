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
#define LENGTH_TABLE_1 16777216
#define MIN_POINTER 32001			//default value that will give us table1[X] as a pointer and not an output interface.

void wrong_arguments(){
		printf("Execute as:	\n");
		printf("        Executable file my_route_lookup\n");
		printf("        <string>: FIB file\n");
		printf("        <string>: Input file\n");
	exit(-1);
}

void fill_empty_table(uint16_t table[]){
		int i;
		for(i=0; i<LENGTH_TABLE_1; i++){
		table[i] = 0x0000;
	}
}

void insert_output_interface1(int n, uint32_t prefix, int prefixLength, uint16_t table[], int outputInterface){
	n = pow(2, 24 - prefixLength);
			int i;
			for(i = (prefix >> 8); i < (prefix >> 8) + n; i++){
				table[i] = outputInterface;
			}
}

void insert_output_interface2(int n, uint32_t prefix, int prefixLength, uint16_t table[], int outputInterface, int result){
	n = pow(2, 32 - prefixLength);
		int i;
		for(i = 0; i < n; i++){
			table[ ((result - MIN_POINTER)*256) + (prefix & 0x000000FF) + i ] = outputInterface;		//table2[ N(base value for subtable in table2) + position to write + i ]
		}
}


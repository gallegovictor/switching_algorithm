#include "io.h"
#include "utils.h"

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

uint16_t static table1[LENGTH_TABLE_1];
void wrong_arguments();
int main ( int argc, char * argv[] ){

	if ( argc != 3 ) {
		wrong_arguments();
    	}

	char *routing_table_file = argv[1];
	char *input_file= argv[2];

	int error, i;

	error = initializeIO(routing_table_file, input_file);
	if(error != OK)
		printIOExplanationError(error);

	for(i=0; i<LENGTH_TABLE_1; i++){
		table1[i] = 0x0000;
	}

	uint32_t prefix = 0;
	int prefixLength = 0;
	int outInterface = 0;
	uint32_t ip_addr;
	int processed_packets = 0;
	struct timespec initialTime;
	struct timespec finalTime;
	int numberOfAccesses;
	int totalNumberOfAccesses = 0;
	double averageNumberOfAccesses;
	double totalTime = 0;
	double average_searching_time;
	double searchingTime;
	int ntables = 0; 	//increments table 2 memory reservation
	int n; 	//number of entries we have to fill
	int result;	//stores the result of table1[x] when needed for table2
	
	uint16_t *table2 = malloc(256*sizeof(uint16_t));


	/*INSERTION SECTION*/
	while(readFIBLine(&prefix, &prefixLength, &outInterface)!= REACHED_EOF){

		if(prefixLength <= 24){		//OUTPUT INTERFACE IN TABLE 1

			n = pow(2, 24 - prefixLength);
			for(i = (prefix >> 8); i < (prefix >> 8) + n; i++){
				table1[i] = outInterface;
			}

		}else{					//OUTPUT INTERFACE IN TABLE 2

			result = table1[prefix >> 8] ;

			if(result >= MIN_POINTER){		//the table that corresponds to that address is already created

				n = pow(2, 32 - prefixLength);
				for(i = 0; i < n; i++){
					table2[ ((result - MIN_POINTER)*256) + (prefix & 0x000000FF) + i ] = outInterface;		//table2[ N(base value for subtable in table2) + position to write + i ]
				}

			}else{

				ntables ++;
				table2 = realloc(table2, ntables * 256 * sizeof(uint16_t) );

				for(i = 0; i < 256; i++){		//Fill with the bale1 corresponding values.
					table2[ ((ntables - 1) * 256) + 1 ] = result;
				}

				n = pow(2, 32 - prefixLength);

				for(i = 0; i < n; i++){		//Overwrite the positions corresponding to that prefix
					table2[ ((ntables - 1)*256) + (prefix & 0x000000FF) + i ] = outInterface;		//table2[ N(base value for subtable in table2) + position to write + i ]
				}

				table1[ prefix >> 8 ] = MIN_POINTER + ntables - 1;

			}

		}

	}


	/*	SEARCHING SECTION 	*/

	while(readInputPacketFileLine(&ip_addr) != REACHED_EOF){

		clock_gettime(CLOCK_REALTIME, &initialTime);

		result = table1[ip_addr >> 8];
		
		if(result < MIN_POINTER){
			outInterface = result;
			numberOfAccesses = 1;
			totalNumberOfAccesses++;
		}else{
			outInterface = table2[ ((result - MIN_POINTER) * 256) + ( ip_addr & 0x000000FF ) ];
			numberOfAccesses = 2;
			totalNumberOfAccesses += 2;			
		}

		clock_gettime(CLOCK_REALTIME, &finalTime);
		processed_packets++;
		
		printOutputLine(ip_addr, outInterface, &initialTime, &finalTime, &searchingTime, numberOfAccesses);

		totalTime += searchingTime;

	}

	average_searching_time = totalTime / processed_packets;
	averageNumberOfAccesses = (double) totalNumberOfAccesses / processed_packets;

	printSummary(processed_packets,  averageNumberOfAccesses, average_searching_time );

	free(table2);

	freeIO();

	return 0;

}

void wrong_arguments(){
		printf("Execute as:	\n");
		printf("        Executable file my_route_lookup\n");
		printf("        <string>: FIB file\n");
		printf("        <string>: Input file\n");
	exit(-1);
}
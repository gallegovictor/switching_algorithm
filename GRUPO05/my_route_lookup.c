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



uint16_t static table1[LENGTH_TABLE_1];

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

	fill_empty_table(table1);	

	uint32_t prefix = 0;
	int prefixLength = 0;
	int outputInterface = 0;
	uint32_t ip_addr;
	int processed_packets = 0;
	struct timespec initialTime;
	struct timespec finalTime;
	int n_accesses=0;
	int total_n_accesses = 0;
	double avg_Accesses;
	double totalTime = 0;
	double avg_searchingTime;
	double searchingTime;
	int ntables = 0; 	//increments table 2 memory reservation
	int n; 	//number of entries we have to fill
	int result;	//stores the result of table1[x] when needed for table2
	
	uint16_t *table2 = malloc(256*sizeof(uint16_t));


	/*INSERTION SECTION*/
	while(readFIBLine(&prefix, &prefixLength, &outputInterface)!= REACHED_EOF){

		if(prefixLength <= 24){		//OUTPUT INTERFACE IN TABLE 1

			insert_output_interface1( n,  prefix,  prefixLength, table1,  outputInterface);

		}else{					//OUTPUT INTERFACE IN TABLE 2

			result = table1[prefix >> 8] ;

			if(result >= MIN_POINTER){		//the table that corresponds to that address is already created
				insert_output_interface2(n, prefix, prefixLength, table2, outputInterface, result);
			}else{

				ntables ++;
				table2 = realloc(table2, ntables * 256 * sizeof(uint16_t) );

				for(i = 0; i < 256; i++){		//Fill with the table1 corresponding values.
					table2[ ((ntables - 1) * 256) + 1 ] = result;
				}

				n = pow(2, 32 - prefixLength);

				for(i = 0; i < n; i++){		//Overwrite the positions corresponding to that prefix
					table2[ ((ntables - 1)*256) + (prefix & 0x000000FF) + i ] = outputInterface;		//table2[ N(base value for subtable in table2) + position to write + i ]
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
			outputInterface = result;
			n_accesses = 1;
			total_n_accesses++;
		}else{
			outputInterface = table2[ ((result - MIN_POINTER) * 256) + ( ip_addr & 0x000000FF ) ];
			n_accesses = 2;
			total_n_accesses += 2;			
		}
		
		clock_gettime(CLOCK_REALTIME, &finalTime);
		processed_packets++;
		
		printOutputLine(ip_addr, outputInterface, &initialTime, &finalTime, &searchingTime, n_accesses);

		totalTime += searchingTime;

	}

	avg_searchingTime = totalTime / processed_packets;
	avg_Accesses = (double) total_n_accesses / processed_packets;

	printSummary(processed_packets,  avg_Accesses, avg_searchingTime );

	free(table2);

	freeIO();

	return 0;

}

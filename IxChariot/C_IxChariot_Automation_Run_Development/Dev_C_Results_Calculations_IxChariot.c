/***********************************************************************
*                                                                      *
* IxChariot Results and Data Calculations                              *
* Author: Nathan Hu                                                    *
* Reference: Ixia APIGuide                                             *
*                                                                      *
* Functions to take IxCHariot test result using the API funtions and   * 
* calculate based on testing needs (eg. averages, usable data)         *
*                                                                      *
***********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Include the Config file */
#include "Dev_C_Config_IxChariot_Single_Pair_Multiple_Streams.c"
#include "chrapi.h"


/* *********  TEST DATA STRUCT  *********
 *
 * Struct containing all the data points that we want to see from tests
 * Defined for ease of use
 */
typedef struct Datapoint{

	double throughput[test_iterations];
	int PHY_rate[test_iterations];
	int e1_RSSI[test_iterations];
	int e2_RSSI[test_iterations];
	double DF[test_iterations];
	// Add more later
}	


/* ********* (1) FORMAT THE TEST DATA FUNCTION  *********
 *
 * Get the numerical data of each test run using CHR_ API functions
 * Present all data in a format that is portable
 * Made as a function that can be implemented in other scripts
 */
char format_data ( int* data_size, double *data_in[], double *data_buffer[] ) {
	 
	char data_out[data_size];
	
	for( int n = 0; n < data_size; n++ ) {
		
		If( n == 0 ) {
			snprintf( data_out, data_size + 1, "%d", data_in[n] );
		}
		else {
			snprintf( data_out, data_size + 1, ", %d", data_in[n] );
		}	
	}
	
	return data_out;
	 
 }	/* ********* End of FORMAT DATA FUNCTION ********* */


/* ********* (2) CALCULATE THE AVERAGE FUNCTION *********
 *
 * Get the average of each test run using CHR_pair_results_get_average();
 * Calculate the average across all test runs

 * Made as a function that can be implemented in other scripts
 */
double average_data( double *avg_data_in[], double *avg_data_buffer[] ) {
	
	int avg_buffer = sizeof(avg_data_in);
	double sum = 0.0, avg = 0.0;
	
	for ( int n = 0; n < avg_buffer; n++ ) {
		sum += avg_data_in[n];
	}
	
	avg = sum/avg_buffer;	
	return avg;

}	/* ********* End of CALCULATE AVERAGE FUNCTION ********* */




/* ********* (3) RSSI DATA FUNCTION *********
 *
 * Retrieve the RSSI data of Endpoint 1 and Endpoint 2
 * CHR_
 * Made as a function that can be implemented in other scripts
 */
void get_RSSI() { 

}	/* ********* End of TEST INFORMATION FUNCTION ********* */



/* MAIN CODE TEMPLATE FOR MATH */
int main() {

    /* ------------------------ Step 0 ------------------------
     * TESTING SECTION
	 */
	double  tx1 = 102.345, tx2 = 225.432, tx3 = 187.320;		
	int r1 = 63, r2 = 78, r3 = 80;


	/* --- Initial Setup --- 
	 * For Code: CHR_STRING data_path, char wifi_channel[]
	 * For description: CHR_STRING ap_name, char location[], char mesh[], ENDPOINTS
	 */
	double test_d_avg[test_iterations];
	int test_i_avg[test_iterations];
	
	/* *** IMPORTANT: Declare one instance of struct Datapoint for each endpoint client *** */
	Datapoint client1, client2, client3, client4;
	/* ************************************************************************************ */
	
	char *test_name = malloc(200);
	char test_timestamp[30];


    /* ------------------------ Step 1 ------------------------
     * DETERMINE THE FILE NAME
     * [data_path]/[description]_[timestamp]_[location]_[wifi_channel].txt
	 * Made as a function that can be implemented in other scripts
     */
	

	/* ------------------------ Step 2 ------------------------
	 * Create the .txt file for the results
	 */
	printf("Creating Test Results File...\n");						// TEST TO MAKE SURE THE FILE NAME IS CORRECT
	printf("\n");
	printf("The file name for this test is: %s\n", test_name);		// TEST TO MAKE SURE THE FILE NAME IS CORRECT
	printf("\n");


	/* ------------------------ Step 3 ------------------------
	 * Initialize the .txt with introductory information
	 */


	/* ------------------------ Step 4 ------------------------
	 * Write the results to the .txt file
	 */
   
	return 0;
}
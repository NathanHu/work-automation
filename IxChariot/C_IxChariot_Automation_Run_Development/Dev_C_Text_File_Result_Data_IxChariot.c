/***********************************************************************
*                                                                      *
* Chariot Test Result Reporting - .txt                                 *
* Author: Nathan Hu                                                    *
* Reference: Ixia APIGuide                                             *
*                                                                      *
* Take IxCHariot test result to a text file                            *
*                                                                      *
***********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

/* Include the Config file */
#include "Dev_C_Config_IxChariot_Single_Pair_Multiple_Streams.c"
#include "chrapi.h"

/* ********* (1) GET CURRENT TIME FUNCTION *********
 *
 * Get the current time in format Date-[Month]-[Day]-[Year]_Time-[Hour]-[Minute][AM/PM]
 * Made as a function that can be implemented in other scripts
 */
void get_current_time( char *time_var ) {

	/* Determine system time using the time.h library */
	time_t raw_time;
	struct tm *time_data;
	time( &raw_time );
	time_data = localtime( &raw_time );

	/* Convert the raw time into the specified format */
	char timestamp[30];
	strftime( timestamp, sizeof(timestamp), "Date-%m-%d-%Y_Time-%H-%m%p", time_data );
	//printf("timestamp variable is %s", timestamp);	// TEST 
	
	/* Copy full file name to the name variable */
	strcpy(time_var, timestamp);

}	/* ********* End of TEST INFORMATION FUNCTION ********* */




/* ********* (2) RETRIEVE TEST FILE NAME INFORMATION FUNCTION *********
 *
 * Find [data_path][description][timestamp][location][wifi_channel]
 * Made as a function that can be implemented in other scripts
 */
void get_test_information() {

}	/* ********* End of TEST INFORMATION FUNCTION ********* */




/* ********* (3) DETERMINE THE FILE NAME FUNCTION *********
 * 
 * [data_path]/[description]_[timestamp]_[location]_[wifi_channel].txt
 * Made as a function that can be implemented in other scripts
 */
void get_file_name( char *name_var, CHR_STRING *data_path, char const *description, char const *timestamp, char const *location, char const *wifi_channel ) {

	/* Determine buffer size (characters) of the final string */
	size_t name_size = snprintf(NULL, 0, "%s/%s_%s_%s_%s.txt", data_path, description, timestamp, location, wifi_channel);
	int size_buffer = name_size + 1;
	char *file_name = malloc(size_buffer);						// Allocate memory for the file name
	
	/* Reallocate the file name array in the main code */
	realloc(name_var, size_buffer);

	/* Combine information strings with snprint() */
	snprintf(file_name, size_buffer, "%s/%s_%s_%s_%s.txt", data_path, description, timestamp, location, wifi_channel);
	memcpy(name_var, file_name, size_buffer);					// Copy full file name to the name variable
	free(file_name);											// Clear the local file_name buffer

}	/* ********* End of FILE NAME FUNCTION ********* */




/* ********* (4) WRITE RESULTS TO TEXT FILE FUNCTION *********
*
* Each test run result written to created text file
* Made as a function that can be implemented in other scripts
*/
void write_to_file( FILE *file_pointer, char *to_file_name, char *data_input ) {
    
	//file_pointer = fopen_s(&file_pointer, to_file_name, "a+");	// Visual Studio suggests fopen_s. FIX LATER
	file_pointer = fopen(to_file_name, "a");						// Create .txt file and set to append mode if it does not exist
	printf("Writing to results...\n");								// TEST TO MAKE SURE THE FILE NAME IS CORRECT
	printf("\n");

	fprintf(file_pointer, "%s\n", data_input);						// Write data input to results file
	//printf("Writing to the results file was successful!\n");		// Test that writing to the file step was completed
	fclose(file_pointer);

}	/* ********* End of FILE WRITE FUNCTION ********* */


/* MAIN CODE TEMPLATE FOR CREATING .txt FILE */
char main() {

    /* ------------------------ Step 0 ------------------------
     * TESTING SECTION
	 */
	//char test_data_path[]		= "C:/Users/Nathan/Desktop/Automation/Testing";
	////char test_timestamp[]	= "date-07-26-1990-time-0900";
	//char test_timestamp[30];
	//char test_wifi_channel[]	= "149";
	//char test_description[]	= "IxChariot_SingleMultiStrmPairTest";
	//char test_location[]		= "In Room";
	//char *test_name = malloc(200);
	//char test_input[] = "Hello World!";


	/* --- Initial Setup --- 
	 * For Code: CHR_STRING data_path, char wifi_channel[]
	 * For description: CHR_STRING ap_name, char location[], char mesh[], ENDPOINTS
	 */
	char *test_name = malloc(200);
	char test_timestamp[30];


    /* ------------------------ Step 1 ------------------------
     * DETERMINE THE FILE NAME
     * [data_path]/[description]_[timestamp]_[location]_[wifi_channel].txt
	 * Made as a function that can be implemented in other scripts
     */
	get_current_time(test_timestamp);
	printf("The current timestamp is: %s\n", test_timestamp);
	printf("\n");

	get_file_name( test_name, data_path, test_description, test_timestamp, location, wifi_channel );
	if (test_name == NULL) {

		perror("Unable to create filename. Check test variables");
		exit(EXIT_FAILURE);

	};
	

	/* ------------------------ Step 2 ------------------------
	 * Create the .txt file for the results
	 */
	printf("Creating Test Results File...\n");						// TEST TO MAKE SURE THE FILE NAME IS CORRECT
	printf("\n");
	printf("The file name for this test is: %s\n", test_name);		// TEST TO MAKE SURE THE FILE NAME IS CORRECT
	printf("\n");

	/* Create the file */
	FILE *test_pointer;
	test_pointer = fopen(test_name, "a");


	/* ------------------------ Step 3 ------------------------
	 * Initialize the .txt with introductory information
	 */
	fprintf(test_pointer, "TEST TYPE:            %s\n", test_type);
	fprintf(test_pointer, "TEST EQUIPMENT:       %s\n", test_equipment);
	fprintf(test_pointer, "DEVICE UNDER TEST:    %s\n", ap_name);
	fprintf(test_pointer, "SCRIPT START TIME:    %s\n", test_timestamp);
	fprintf(test_pointer, "WI-FI CHANNEL :       %s\n", wifi_channel);
	fprintf(test_pointer, "\n");	// Space

	fprintf(test_pointer, "PAIR TEST DURATION:   %d\n", test_run_duration);
	fprintf(test_pointer, "PAIR DELAY:           %d\n", between_pair_delay);
	fprintf(test_pointer, "ITERATION DELAY:      %d\n", iteration_delay);
	fprintf(test_pointer, "NUMBER OF ITERATIONS: %d\n", test_iterations);
	fprintf(test_pointer, "\n");	// Space

	fprintf(test_pointer, "COMMENT: %s\n", text);
	fprintf(test_pointer, "%s\n", divider);
	
	/* Close file until next write */
	fclose(test_pointer);
	//write_to_file(test_pointer, test_name, test_input); // TEST
	printf("Result file created!\n");
	printf("\n");


	/* ------------------------ Step 4 ------------------------
	 * Write the results to the .txt file
	 */
   

}
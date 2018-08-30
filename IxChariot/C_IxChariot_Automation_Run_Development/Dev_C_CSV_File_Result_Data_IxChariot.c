/***********************************************************************
*                                                                      *
* Chariot Test Result Reporting - .csv                                 *
* Author: Nathan Hu                                                    *
* Reference: Ixia APIGuide                                             *
*                                                                      *
* Take IxCHariot test result and format to a .csv file
* Commas for colums, '\n' for rows                            *
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




/* ********* (2) DETERMINE THE FILE NAME FUNCTION *********
 * 
 * [data_path]/[description]_[timestamp]_[location]_[wifi_channel].txt
 * Made as a function that can be implemented in other scripts
 */
void get_csv_name( char *name_var, char *data_path, char const *description, char const *timestamp, char const *location, char const *wifi_channel ) {

	/* Determine buffer size (characters) of the final string */
	size_t name_size = snprintf(NULL, 0, "%s/%s_%s_%s_%s.csv", data_path, description, timestamp, location, wifi_channel);
	int size_buffer = name_size + 1;
	char *file_name = malloc(size_buffer);						// Allocate memory for the file name
	
	/* Reallocate the file name array in the main code */
	realloc(name_var, size_buffer);

	/* Combine information strings with snprint() */
	snprintf(file_name, size_buffer, "%s/%s_%s_%s_%s.csv", data_path, description, timestamp, location, wifi_channel);
	memcpy(name_var, file_name, size_buffer);					// Copy full file name to the name variable
	free(file_name);											// Clear the local file_name buffer

}	/* ********* End of FILE NAME FUNCTION ********* */

	
	
	 
/* ********* (5) PRINT ROW HEADER FUNCTION *********
*
* Writes a formatted client identifier to the first colum of a row
* Useful for setting row identifier
* Made as a function that can be implemented in other scripts
*/
void set_row_header( char *header, int *streams, char *client, char *location, char *channel ) {

	memset(header, 0, 30);
	snprintf( header, 30, "%d-%s-%s-%s", streams, client, location, channel);
	printf("Row identifier is %s-%s-%s\n", client, location, channel);


}	/* ********* End of ROW HEADER FUNCTION ********* */




/* ********* (5) WRITE RESULTS TO .csv FILE FUNCTION *********
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




/* MAIN CODE TEMPLATE FOR CREATING .csv FILE */
char main() {

    /* ------------------------ Step 0 ------------------------
     * TESTING SECTION
	 */
	double test_tx[5] = { 123.45, 135.43, 154.63, 121.11, 126.51};
	//char test_head[] = "MBP-L19-149";
	int test_RSSI[] = { 64, 72, 80 };


	/* --- Initial Setup --- 
	 * For Code: CHR_STRING data_path, char wifi_channel[]
	 * For description: CHR_STRING ap_name, char location[], char mesh[], ENDPOINTS
	 */
	double average_data(double avg_data_in[5]);
	char *test_name		= malloc(200);
	char *row_buffer	= malloc(255);
	char test_timestamp[30];
	char row_header[30];
	int num_streams = 7;


    /* ------------------------ Step 1 ------------------------
     * DETERMINE THE FILE NAME
     * [data_path]/[description]_[timestamp]_[location]_[wifi_channel].txt
	 * Made as a function that can be implemented in other scripts
     */
	get_current_time(test_timestamp);
	printf("The current timestamp is: %s\n", test_timestamp);
	printf("\n");

	get_csv_name( test_name, data_path, test_description, test_timestamp, location, wifi_channel );
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
	test_pointer = fopen(test_name, "a+");
	if (test_name == NULL) {

		perror("Unable to create filename. Check test variables");
		exit(EXIT_FAILURE);

	};


	/* ------------------------ Step 3 ------------------------
	 * Initialize the .txt with introductory information
	 */
	 /* Test information at the top of file */
	fprintf(test_pointer, "TEST TYPE, %s\n", test_type);
	fprintf(test_pointer, "TEST EQUIPMENT, DUT, CHANNEL\n");
	fprintf(test_pointer, "%s, %s, %s\n", test_equipment, ap_name, wifi_channel);
	fprintf(test_pointer, "RUN DURATION (sec), # ITERATIONS\n");
	fprintf(test_pointer, "%d, %d\n", test_run_duration, test_iterations);
	fprintf(test_pointer, "\n"); 	// Blank row as divider
	
	/* Close file until next write */
	//fclose(test_pointer);
	//write_to_file(test_pointer, test_name, test_input); // TEST
	printf("Result .csv file created!\n");
	printf("\n");


	/* ------------------------ Step 4 ------------------------
	 * Write the results to the .csv file
	 * Unfortunately for now manually format the data layout
	 */
	
	 /* Get the row header */
	//set_row_header(row_header, client_id[0], location, wifi_channel);
	//printf("The received header is: %s\n", row_header);	// TEST
	 
	 /* *** DATASET 1 *** */
	printf("********* DATASET 1 ********\n");
	set_row_header(row_header, num_streams, client_id[0], location, wifi_channel);		// FOR TEST. ADD TO LOOP IN REAL CODE. 
	double avg1 = average_data(test_tx);

	fprintf(test_pointer, "Client Endpoint, Run 1, Run 2, Run 3, Run 4, Run 5, Avg Throughput\n");
	fprintf(test_pointer, "%s, %s, %s, %s, %s, %s, %s\n", row_id, units[1], units[1], units[1], units[1], units[1], units[1]);
	fprintf(test_pointer, "%s, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f\n", 
						row_header, test_tx[0], test_tx[1], test_tx[2], test_tx[3], test_tx[4], avg1);
	printf("The average throughput is %.3f %s\n", avg1, units[1]);
	 /* Close file until next write */
	fclose(test_pointer);

	printf("******* END DATASET 1 ******\n");
	 /* *** END OF DATASET 1 *** */

	
} /* END OF MAIN */

/* ********* CALCULATE THE AVERAGE FUNCTION *********
 *
 * Get the average of each test run using CHR_pair_results_get_average();
 * Calculate the average across all test runs

 * Made as a function that can be implemented in other scripts
 */
double average_data(double avg_data_in[5]) {

	int elements = 1;	// At least 1 element
	double sum = 0, avg = 0;
	
	for (int n = 0; n < sizeof(avg_data_in); n++) {
		sum += avg_data_in[n];
		elements++;
	}
	
	avg = sum / elements;
	printf("The sum of the %d datapoints is %.3f and the avg is %.3f\n", elements, sum, avg);	// TEST
	printf("\n");

	return avg;

}	/* ********* End of CALCULATE AVERAGE FUNCTION ********* */
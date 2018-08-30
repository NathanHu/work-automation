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
/* For directories */
#include <sys/stat.h>
#include <sys/types.h>
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




/* ********* (2.1) DETERMINE THE TXT NAME FUNCTION *********
 *
 * [data_path]/[description]_[timestamp]_[location]_[wifi_channel].txt
 * Made as a function that can be implemented in other scripts
 */
void get_txt_name( char *name_var, CHR_STRING *data_path, char *folder, char const *description, char const *timestamp, char const *location, char const *wifi_channel ) {

	/* Determine buffer size (characters) of the final string */
	size_t name_size = snprintf(NULL, 0, "%s/%s/%s_%s_%s_%s.txt", data_path, folder, description, timestamp, location, wifi_channel);
	int size_buffer = name_size + 1;
	char *file_name = malloc(size_buffer);						// Allocate memory for the file name

	/* Reallocate the file name array in the main code */
	realloc(name_var, size_buffer);

	/* Combine information strings with snprint() */
	//snprintf(file_name, size_buffer, "%s/%s/%s_%s_%s_%s.txt", data_path, folder, description, timestamp, location, wifi_channel);
	snprintf(file_name, size_buffer, "%s/%s/", data_path, folder);
	/* Create directory if it does not exist */
	int check_directoy = mkdir(file_name, 0777);
	snprintf(file_name, size_buffer, "%s/%s/%s_%s_%s_%s.txt", data_path, folder, description, timestamp, location, wifi_channel);
	
	memcpy(name_var, file_name, size_buffer);					// Copy full file name to the name variable
	free(file_name);											// Clear the local file_name buffer

}	/* ********* End of FILE NAME FUNCTION ********* */




/* ********* (2.2) DETERMINE THE CSV NAME FUNCTION *********
 *
 * [data_path]/[description]_[timestamp]_[location]_[wifi_channel].txt
 * Made as a function that can be implemented in other scripts
 */
void get_csv_name( char *name_var, CHR_STRING *data_path, char *folder, char const *description, char const *timestamp, char const *location, char const *wifi_channel ) {

	/* Determine buffer size (characters) of the final string */
	size_t name_size = snprintf(NULL, 0, "%s/%s/%s_%s_%s_%s.csv", data_path, folder, description, timestamp, location, wifi_channel);
	int size_buffer = name_size + 1;
	char *file_name = malloc(size_buffer);						// Allocate memory for the file name

	/* Reallocate the file name array in the main code */
	realloc(name_var, size_buffer);

	/* Combine information strings with snprint() */
	snprintf(file_name, size_buffer, "%s/%s/", data_path, folder);
	/* Create directory if it does not exist */
	int check_directory = mkdir(file_name, 0777);
	snprintf(file_name, size_buffer, "%s/%s/%s_%s_%s_%s.csv", data_path, folder, description, timestamp, location, wifi_channel);

	memcpy(name_var, file_name, size_buffer);					// Copy full file name to the name variable
	free(file_name);											// Clear the local file_name buffer

}	/* ********* End of FILE NAME FUNCTION ********* */




/* ********* (3) WRITE RESULTS TO .csv FILE FUNCTION *********
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




/* ********* (4) PRINT ROW HEADER .csv FUNCTION *********
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



/***************************************************************************
 ******************************** MAIN CODE ********************************
 **************************************************************************/
int main() {

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
	//char test_head[] = "MBP-L19-149";
	double test_tx[5] = { 123.45, 135.43, 154.63, 121.11, 126.51};

	int test_RSSI[] = { 64, 72, 80 };
	int num_streams = 7;

	/* --- Initial Setup ---
	 * For Code: CHR_STRING data_path, char wifi_channel[]
	 * For description: CHR_STRING ap_name, char location[], char mesh[], ENDPOINTS
	 */
	  /* Additional function initialization */
	double average_data(double avg_data_in[5]);
	 /* For file creation and access */
	char *txt_name = malloc(200);
	char *csv_name = malloc(200);
	char test_timestamp[30];
     /* For .csv use */
	char row_header[30];
	char *row_buffer = malloc(255);
	
	 /* IMPORTANT FILE POINTERS */
	FILE *txt_pointer, *csv_pointer;


 /***************************************************************************
  ****************************** FILE CREATION ******************************
  **************************************************************************/

	  /* ------------------------ Step 1 ------------------------
     * DETERMINE THE FILE NAME
     * [data_path]/[description]_[timestamp]_[location]_[wifi_channel].txt
	 * Made as a function that can be implemented in other scripts
     */
	get_current_time(test_timestamp);
	printf("The current timestamp is: %s\n", test_timestamp);
	printf("\n");

	get_txt_name( txt_name, data_path, ap_name, test_description, test_timestamp, location, wifi_channel );
	get_csv_name( csv_name, data_path, ap_name, test_description, test_timestamp, location, wifi_channel );
	if ( txt_name == NULL || csv_name == NULL ) {
		perror("Unable to create filename. Check test variables");
		exit(EXIT_FAILURE);
	};

	/* ------------------------ Step 2 ------------------------
	 * Create the files for the results
	 */
	printf("Creating Test Results Files...\n");						// TEST TO MAKE SURE THE FILE NAME IS CORRECT
	printf("\n");
	printf("The .txt for this test is: %s\n", txt_name);		// TEST TO MAKE SURE THE FILE NAME IS CORRECT
	printf("The .csv for this test is: %s\n", csv_name);		// TEST TO MAKE SURE THE FILE NAME IS CORRECT
	printf("\n");

	 /* *** 2.1 initialize the .txt file *** */
	txt_pointer = fopen(txt_name, "a+");
	if (txt_name == NULL) {
		perror("Unable to create .txt. Check test variables");
		exit(EXIT_FAILURE);
	};

	fprintf(txt_pointer, "TEST TYPE:            %s\n", test_type);
	fprintf(txt_pointer, "TEST EQUIPMENT:       %s\n", test_equipment);
	fprintf(txt_pointer, "DEVICE UNDER TEST:    %s\n", ap_name);
	fprintf(txt_pointer, "SCRIPT START TIME:    %s\n", test_timestamp);
	fprintf(txt_pointer, "WI-FI CHANNEL :       %s\n", wifi_channel);
	fprintf(txt_pointer, "\n");	// Space

	fprintf(txt_pointer, "PAIR TEST DURATION:   %d\n", test_run_duration);
	fprintf(txt_pointer, "PAIR DELAY:           %d\n", between_pair_delay);
	fprintf(txt_pointer, "ITERATION DELAY:      %d\n", iteration_delay);
	fprintf(txt_pointer, "NUMBER OF ITERATIONS: %d\n", test_iterations);
	fprintf(txt_pointer, "\n");	// Space

	fprintf(txt_pointer, "COMMENT: %s\n", text);
	fprintf(txt_pointer, "%s\n", divider);

	/* Close file until next write */
	fclose(txt_pointer);
	printf("Result .txt file created!\n");
	//printf("\n");

	/* *** 2.2 initialize the .csv file *** */
	csv_pointer = fopen(csv_name, "a+");
	if (csv_name == NULL) {
	 perror("Unable to create .csv. Check test variables");
	 exit(EXIT_FAILURE);
	};
	/* Test information at the top of file */
	fprintf(csv_pointer, "TEST TYPE, %s\n", test_type);
	fprintf(csv_pointer, "TEST EQUIPMENT, DUT, CHANNEL\n");
	fprintf(csv_pointer, "%s, %s, %s\n", test_equipment, ap_name, wifi_channel);
	fprintf(csv_pointer, "RUN DURATION (sec), # ITERATIONS\n");
	fprintf(csv_pointer, "%d, %d\n", test_run_duration, test_iterations);
	fprintf(csv_pointer, "\n"); 	// Blank row as divider

	/* Close file until next write */
	fclose(csv_pointer);
	//write_to_file(test_pointer, test_name, test_input); // TEST
	printf("Result .csv file created!\n");
	printf("\n");


	/* ------------------------ Step 3 ------------------------
	 * Writing the results to the .txt and .csv file
	 */
	 /* *** 3.1 Write to the .txt file *** */

	 /* *** 3.2 Write to the .txt file (currently manual) *** */
	  /* *** DATASET 1 *** */
	printf("********* DATASET 1 ********\n");
	set_row_header(row_header, num_streams, client_id[0], location, wifi_channel);		// FOR TEST. ADD TO LOOP IN REAL CODE.
	double avg1 = average_data(test_tx);	// MOVE LATER

	csv_pointer = fopen(csv_name, "a+");
	fprintf(csv_pointer, "Client Endpoint, Run 1, Run 2, Run 3, Run 4, Run 5, Avg Throughput\n");
	fprintf(csv_pointer, "%s, %s, %s, %s, %s, %s, %s\n", row_id, units[1], units[1], units[1], units[1], units[1], units[1]);
	fprintf(csv_pointer, "%s, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f\n",
						row_header, test_tx[0], test_tx[1], test_tx[2], test_tx[3], test_tx[4], avg1);
	printf("The average throughput is %.3f %s\n", avg1, units[1]);
	 /* Close file until next write */
	fclose(csv_pointer);
	printf("******* END DATASET 1 ******\n");
	 /* *** END OF DATASET 1 *** */

	 
	/* ------------------------ Step 4 ------------------------
	 * Writing the results to the .txt and .csv file
	 */




	return 0;
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

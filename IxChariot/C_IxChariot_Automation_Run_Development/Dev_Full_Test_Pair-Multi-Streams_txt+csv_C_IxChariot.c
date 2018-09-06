/***********************************************************************
*                                                                      *
*######################################################################*
*#      IxChariot Single Endpoint Pair - Multiple Streams Test        #*
*######################################################################*
* IxChariot Config File Location:                                      *
* Test Result Path:                                                *
* Test Config Location:                                           *
* Author: Nathan Hu                                                    *
* Reference: Ixia APIGuide                                             *
*                                                                      *
*  This program runs endpoint to endpoint TCP performance throughput   *
*  tests across multiple clients. The user can specify in the config   *
*  file the number of streams that are run per endpoint pair and the   *
*  number of iterations the test will run. The results will be output  *
*  to the specified folder as a .txt file.                             *
*                                                                      *
***********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <windows.h>
/* For directories */
#include <sys/stat.h>
#include <sys/types.h>
/* Macro for variable arguments */
#include <stdarg.h>
/* Automation headers and includes */
#include "chrapi.h"													// IxChariot Header/API
#include "Dev_C_Config_IxChariot_Single_Pair_Multiple_Streams.c"	// Test Config file


/* File Name Struct */
struct Filename {

	CHR_STRING data_path;
	char *folder;
	char *description;
	char *timestamp;
	char *location;
	char *wifi_channel;
};
typedef struct Filename Filename;


/* Row Header Struct */
struct Header {

	char *client;
	char *location;
	char *wifi_channel;
	int *streams;
};
typedef struct Header Header;


/* *********  TEST DATA STRUCT  *********
 *
 * Struct containing all the data points that we want to see from tests
 * Change DATA_STORAGE if needed
 * Initialized as 'report'
 * Defined for ease of use
 */
typedef struct Datapoint Datapoint;
struct Datapoint{

	int ep_id;
	int stream_id;

	double throughput;
    double throughput_min;
    double throughput_max;
	double e1_RSSI;
    double e1_RSSI_min;
    double e1_RSSI_max;
	double e2_RSSI;
    double e2_RSSI_min;
    double e2_RSSI_max;
    double PHY_rate;
	double latency;
	// Add more later
};


/* Success counter */
typedef struct {

	int t_success;
	int e1_RSSI_success;
	int e2_RSSI_success;
	int PHY_success;
	int lat_success;
	// Add more later
} Success;

typedef struct {

	Success *buffer;

} SuccessArr;

/* Linked list node used for test run results */
struct Node {
	// Any data type can be stored in this node
	Datapoint test_data;

	struct Node *next;
};

void push_data( struct Node **head, Datapoint data )
{
	// Allocate memory for node
	struct Node* new_node = malloc(sizeof(struct Node));

	new_node->test_data = data;
	new_node->next = (*head);

	// Copy contents of new_data to newly allocated memory.
	new_node->test_data = data;

	// Change head pointer as new node is added at the beginning
	(*head) = new_node;
}


/* FOR TESTING OF NODES IN LINKEDLIST */
void printList( struct Node *node )
{
	while ( node != NULL )
	{

		printf("%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n",
			node->test_data.throughput,
			node->test_data.throughput_min,
			node->test_data.throughput_max,
			node->test_data.e1_RSSI,
			node->test_data.e1_RSSI_min,
			node->test_data.e1_RSSI_max,
			node->test_data.e2_RSSI,
			node->test_data.e2_RSSI_min,
			node->test_data.e2_RSSI_max,
			node->test_data.PHY_rate,
			node->test_data.latency);

		node = node->next;
	}
}


/* Initialization of local functions used in this automation code */
/* ********* (1) GET CURRENT TIME FUNCTION ********* */
static void get_current_time( char *time_var );
/* ********* (2) DETERMINE THE FILE NAME FUNCTION ********* */
static void get_file_name(char *name_var, char *csv, char *txt, Filename info);
/* ********* (3) PRINT ROW HEADER .csv FUNCTION ********* */
static void set_row_header(char *header, Header info);
//static void write_to_csv( FILE *file_pointer, int num_col, ... );
//void allocate_success(Success ** stream_pointer, int streamLength);
//void allocate_successArr(SuccessArr ** client_pointer, int clientLength);
Datapoint *scan_data( struct Node **head, int client, int stream );
void output_final( FILE *txt_pointer, FILE *csv_pointer, Datapoint total ,
				   Success *counter, Header header, int *iterations );

/* Initialization of Ixia API functions used in this automation code */
/* Local function to print results common to pairs, mpairs and timing records */
static void show_common( CHR_HANDLE handle );
static void show_results(FILE *TXT_pointer, FILE *CSV_pointer,
						 Datapoint *report, Success *counter, CHR_HANDLE test);
/* Local function to print results for all timing records. */
static void show_timing_rec( CHR_HANDLE timingRec );
/* Local function to log & print information about errors. */
static void show_error( CHR_HANDLE handle, CHR_API_RC code, CHR_STRING where );
/* Local function to print information about results get errors. */
static void show_results_error( CHR_API_RC rc, CHR_STRING where );

/* Local function to print common protocol string. */
static char *show_protocol_string( CHR_PROTOCOL protocol );
/* Local function to print information about how a test ended. */
static char *show_how_ended_string( CHR_TEST_HOW_ENDED howEnded );
/* Local function to print units used to express throughput. */
//static char *show_throughput_units_string( CHR_THROUGHPUT_UNITS throughputUnits );



/***************************************************************************
 ******************************** MAIN CODE ********************************
 **************************************************************************/
void main(int argc, char* argv[]) {

    /* --- Initial Setup ---
     * CHR_ IxChariot
	 */
     /* Ixia API Variables */
    CHR_TEST_HANDLE         test;
    CHR_PAIR_HANDLE         pair;
	CHR_RUNOPTS_HANDLE		runopts;
    /* For error reporting and handling */
    char		            errorInfo[CHR_MAX_ERROR_INFO];
    CHR_LENGTH	            errorLen;
    CHR_BOOLEAN             isStopped;
    //CHR_TEST_HOW_ENDED      endCode;
    /* Indexes used by Test. Will change based on test configuration */
    int						iteration_index, client_index, stream_index;
    //CHR_COUNT               prIndex;
    //CHR_COUNT               lastPrinted, lastIndex;
    /* timing */
    CHR_COUNT               timer = 0;
    //CHR_TIMINGREC_HANDLE    timingRec;
    /* reporting */
    //CHR_PROTOCOL            protocol;
    //CHR_THROUGHPUT_UNITS    throughput;
    //CHR_MESSAGE_NUMBER      msgNumber;
    //CHR_LENGTH				len;

	//CHR_STRING testFile;
    CHR_API_RC	rc;

     /* Local Test Variables */
	int streamTests = sizeof(streamNum)/sizeof(int *);

     /* For file creation and access */
    char file_name[CHR_MAX_FILE_PATH];
  	char txt_name[CHR_MAX_FILE_PATH];
  	char csv_name[CHR_MAX_FILE_PATH];
  	char test_timestamp[30];
     /* For .csv use */
  	//char *row_buffer = malloc(255);
  	 /* IMPORTANT FILE POINTERS */
  	FILE *txt_pointer, *csv_pointer;
     /* For test tracking */
    int total_runs		= test_iterations * client_count * streamTests;
    int client_runs		= client_count * streamTests;
	int stream_count	= 0;
    int run_counter		= 0;		// Total runs counter

     /* Initialize structs that will hold data for each iteration */
	//Header header = { header.location = &location, header.wifi_channel = &wifi_channel };
	Header header = { NULL, &location, &wifi_channel, 0 };
	Datapoint report, total;
	Success check = { 0, 0, 0, 0, 0 };
	SuccessArr *checkArr;

	/* Allocate memory and initialize the the run success counter(s) */
	checkArr = malloc( client_count * sizeof( checkArr ) );
	for (int s_c = 0; s_c < client_count; s_c++) {

		checkArr[s_c].buffer = malloc( streamTests * sizeof( Success *) );
		
		/*for (int s_s = 0; s_s < streamTests; s_s++) {

			checkArr[s_c].buffer[s_s].t_success = 0;
			checkArr[s_c].buffer[s_s].e1_RSSI_success = 0;
			checkArr[s_c].buffer[s_s].e2_RSSI_success = 0;
			checkArr[s_c].buffer[s_s].PHY_success = 0;
			checkArr[s_c].buffer[s_s].lat_success = 0;

		}*/
	}

	 /* Initialize linked list head reference 'start' for data */
	struct Node *start = NULL;

    /************************************************************************
     ************************ Step 1.0 FILE CREATION ************************
     ************************************************************************/
	 /* ------------------------ 1.1 ------------------------
      * DETERMINE THE FILE NAME
      * [data_path]/[description]_[timestamp]_[location]_[wifi_channel].txt
 	  * Made as a function that can be implemented in other scripts
      */
   	get_current_time(test_timestamp);
   	printf("The current timestamp is: %s\n", test_timestamp);
   	printf("\n");

	/* Initialize new Filename struct */
	Filename info = { data_path, &ap_name, test_description, &test_timestamp, &location, &wifi_channel };
	get_file_name(file_name, csv_name, txt_name, info);
    if ( txt_name == NULL || csv_name == NULL ) {
   		perror("Unable to create filename. Check test variables");
   		exit(EXIT_FAILURE);
   	};

   	 /* ------------------------ 1.2 ------------------------
   	  * Create the files for the results
   	  */
   	printf("Creating Test Results Files...\n");					// TEST TO MAKE SURE THE FILE NAME IS CORRECT
   	printf("\n");
   	printf("The .txt for this test is: %s\n", txt_name);		// TEST TO MAKE SURE THE FILE NAME IS CORRECT
   	printf("The .csv for this test is: %s\n", csv_name);		// TEST TO MAKE SURE THE FILE NAME IS CORRECT
   	printf("\n");

   	 /* *** 1.2.1 initialize the .txt file *** */
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
	fprintf(txt_pointer, "\n");

   	  /* Close file until next write */
   	fclose(txt_pointer);
   	printf("Result .txt file created!\n");
   	//printf("\n");

   	 /* *** 1.2.2 initialize the .csv file *** */
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

    /************************************************************************
    ******************* Step 2.0 IxCHARIOT INITIALIZATION *******************
    *************************************************************************/
    /* ------------------------ 2.1 ------------------------
     * Initialize the Chariot API
     */
    rc = CHR_api_initialize( CHR_DETAIL_LEVEL_ALL, errorInfo,
                             CHR_MAX_ERROR_INFO, &errorLen );

	/* *** Error handling variable CHR_OK ***
	 * Checked frequently at almost every step
	 * Exit upon failure
	 */
	 if (rc != CHR_OK) {
        printf("Initialization failed: rc = %d\n", rc);
        printf("Extended error info:\n%s\n", errorInfo);
        exit(EXIT_FAILURE);
	 }

    /* ------------------------ 2.2 ------------------------
     * TEST LOOP
     *
     * Test permutations are  [# of iterations] x [# of clients] x [# of stream pair]
     * Modified to run each pair in sequence. A single iteration is completed
     * after all pairs have been run for the specified number of times
     */
     /* ***** TEST ITERATION LOOP ***** */
    printf("Initializing test run. There will be %d tests each set for a duration of %d seconds...\n",
            total_runs, test_run_duration);
    printf("\n");

    for (iteration_index = 1; iteration_index <= test_iterations; iteration_index++) {

		/* Initialize the client instance struct */
		//init_client(client_s, stream_s);

		printf("%s ITERATION  %d  %s\n", special, iteration_index, special);	// Indicate new iteration
        printf("\n");

        /* Write iteration # to results */
        txt_pointer = fopen(txt_name, "a+");
        fprintf(txt_pointer, "%s TEST ITERATION  %d  %s\n", special, iteration_index, special);
        fprintf(txt_pointer, "\n");
        fclose(txt_pointer);

        csv_pointer = fopen(csv_name, "a+");
        fprintf(csv_pointer, "TEST ITERATION  %d\n", iteration_index);
        fclose(csv_pointer);
        /* Client * # of stream tests counter */
        int test_counter = 0;

         /* ***** CLIENT PAIRS/ENDPOINTS ITERATION LOOP ***** */
        for (client_index = 0; client_index < client_count; client_index++) {

			/* Initialize the pair instance struct */
			//init_pair(stream_s, test_iterations);

			/* Initialize the stream test instance struct */
			//init_client(stream_s, pairCount);

			/* Indicate on .txt and .csv the client being tested in this iteration */
            txt_pointer = fopen(txt_name, "a+");
            fprintf(txt_pointer, "%s-%s test - Location: %s - Channel: %s\n",
				endpoint1, client_id[client_index], location, wifi_channel);
            fprintf(txt_pointer, "\n");
            fclose(txt_pointer);

            csv_pointer = fopen(csv_name, "a+");
            fprintf(csv_pointer, "%s-%s test - Location: %s - Channel: %s\n",
                    endpoint1, client_id[client_index], location, wifi_channel);
            fclose(csv_pointer);

            printf("Starting %s test(s) with %s - Location: %s - Channel: %s\n",
                    endpoint1, client_id[client_index], location, wifi_channel);
            printf("\n");

            /* ***** NUMBER OF STREAMS PER CLIENT ITERATION LOOP ***** */
            for (stream_index = 0; stream_index < streamTests; stream_index++) {

                /* local variables for test instance */
				char row_header[50];
				run_counter++;

                /* Determine number of streams */
                stream_count = streamNum[stream_index];
				/* Determine row header for test instance */
				header.client = client_id[client_index];
				header.streams = stream_count;

				set_row_header(row_header, header);

				/* Section off runs for visibility */
				printf("%s\n", divider);
				printf("Run %d - %s Stream Pairs\n", run_counter, row_header);
                printf("\n");

                /* Label this test to add to final struct */
                report.stream_id = stream_index;
                report.ep_id = client_index;

                /* Name test file that will be deleted after run */
                char test_name[CHR_MAX_FILE_PATH];
                sprintf(test_name, "%s_TEMP.tst", file_name);
                //printf("The test file is: %s\n", test_name);
				//printf("\n");

                /* ------------------------ 2.2 ------------------------
                 * Create/open new test for every test permutation
                 */
                printf("Initializing the test instance...\n");
                printf("\n");
                rc = CHR_test_new(&test);
                if (rc != CHR_OK) {
                   show_error((CHR_HANDLE)NULL, rc, "test_new");
                }

				/* Initialize the runopts handle for this test instance and set the run time */
				rc = CHR_test_get_runopts(test, &runopts);
				rc = CHR_runopts_set_test_duration(runopts, test_run_duration);
				/* Set test to end after a fixed duration */
				rc = CHR_runopts_set_test_end(runopts, CHR_TEST_END_AFTER_FIXED_DURATION);
				if (rc != CHR_OK) {
					show_error(runopts, test, rc, "set_test_duration");
				}

                /* Set the test filename test */
                rc = CHR_test_set_filename(test, test_name, strlen(test_name));
                if (rc != CHR_OK) {
                    show_error(test, rc, "test_set_filename");
                }

                /* ------------------------ 2.3 ------------------------
                 * Set up results file run information
                 */
                txt_pointer = fopen(txt_name, "a+");
				//fprintf(txt_pointer, "%s\n", divider);
				fprintf(txt_pointer, "Run %d - %s Stream Pairs\n", run_counter, row_header);
                fprintf(txt_pointer, "\n");

                /* PAIR(s) CREATION */
                printf("Pairing %d stream pair(s)\n", stream_count);
                printf("Setting pair(s) attributes...\n");

                /* ------------------------ 2.4 ------------------------
                 * Set the pair attributes from config file
                 */
                for ( int each_pair = 0; each_pair < stream_count; each_pair++ ) {

                    char comment[CHR_MAX_PAIR_COMMENT];

                    /* --- Create a pair per stream --- */
                    rc = CHR_pair_new(&pair);
                    if (rc != CHR_OK) {
                        show_error((CHR_HANDLE)NULL, rc, "pair_new");
                    }

                    sprintf(comment, "%s to %s", e1Addrs[client_index], e2Addrs[client_index]);
                    rc = CHR_pair_set_comment(pair,
                        comment,
                        strlen(comment));
                    if (rc != CHR_OK) {
                        show_error(pair, rc, "pair_set_comment");
                    }

                    /* --- Set Endpoint 1 --- */
                    rc = CHR_pair_set_e1_addr(pair,
        				e1Addrs[client_index],
        				strlen(e1Addrs[client_index]));
        			if (rc != CHR_OK) {
        				show_error(pair, rc, "pair_set_e1_addr");
        			}

                    /* --- Set Endpoint 2 --- */
        			rc = CHR_pair_set_e2_addr(pair,
        				e2Addrs[client_index],
        				strlen(e2Addrs[client_index]));
        			if (rc != CHR_OK) {
        				show_error(pair, rc, "pair_set_e2_addr");
        			}
                    /* --- Set pair protocol --- */
        			rc = CHR_pair_set_protocol(pair, protocols[0]);     // Indicate pair protocol from config
        			if (rc != CHR_OK) {
        				show_error(pair, rc, "pair_set_protocol");
        			}

        			rc = CHR_pair_use_script_filename(pair,             // Script from config
        				script,
        				strlen(script));
        			if (rc != CHR_OK) {
        				show_error(pair, rc, "pair_use_script_filename");
        			}

        			/* --- Add the pair protocol --- */
        			rc = CHR_test_add_pair(test, pair);
        			if (rc != CHR_OK) {
        				show_error(test, rc, "test_add_pair");
        			}

                    fprintf(txt_pointer, "Stream Pair %d - %s\n",
                        each_pair + 1, comment);

					printf("Stream Pair %d - %s\n",
						each_pair + 1, comment);

                } /* End of stream pairing */
                fprintf(txt_pointer, "\n");

				printf("Pairs added to test!\n");
				printf("\n");

    			/* ------------------------ 2.5 ------------------------
    			 * RUN THE TEST!
    			 */
    			printf("Running the test...\n");
    			rc = CHR_test_start(test);
    			if (rc != CHR_OK) {
    				show_error(test, rc, "start_test");
    			}

    			/*
				 * Wait for the test to stop.
				 * Duration is  test_run_duration + between_pair_delay
				 *
				 * We'll check in a loop here every 5 seconds
				 * then call it an error after two minutes if
				 * the test is still not stopped.
				 */
    			printf("Wait for the test to stop...\n");
				printf("\n");
    			isStopped = CHR_FALSE;
    			timer = 0;
    			while (!isStopped && timer < maxWait) {
    				rc = CHR_test_query_stop(test, timeout);
    				if (rc == CHR_OK) {
    					isStopped = CHR_TRUE;
    				}
    				else if (rc == CHR_TIMED_OUT) {
    					timer += timeout;
    					//printf("Waiting for test to stop... (%d)\n", timeout);
    				}
    				else {
    					show_error(test, rc, "test_query_stop");
    				}
    			}
    			if (!isStopped) {
    				show_error(test, CHR_TIMED_OUT, "test_query_stop");
    			}
                /* End of a single test permutation */

    /************************************************************************
    ************************ Step 3.0 DATA REPORTING ************************
    *************************************************************************/
                /* ------------------------ 3.1 ------------------------
                 * Print results to console and results files
                 */
                fprintf(txt_pointer, "Run %d - %d Stream Pair(s) Results:\n",
                    run_counter, stream_count);
                //fprintf(txt_pointer, "\n");

                /* *** CSV writing implementation *** */
                csv_pointer = fopen(csv_name, "a+");
                fprintf(csv_pointer, "Run %d - %d Stream Pair(s) Results:\n",
                        run_counter, stream_count);
                fprintf(csv_pointer, "%s, ", row_header);
				
				/* Reset check counter to 0 */
				memset(&check, 0, sizeof(Success));

                /* Collect results from test API */
                show_results( txt_pointer, csv_pointer, &report, &check, test );

				/* Set the current run success counter in the success array */
				//checkArr[client_index].buffer[stream_index] = check;

				checkArr[client_index].buffer[stream_index].t_success = check.t_success;
				checkArr[client_index].buffer[stream_index].e1_RSSI_success = check.e1_RSSI_success;
				checkArr[client_index].buffer[stream_index].e2_RSSI_success = check.e2_RSSI_success;
				checkArr[client_index].buffer[stream_index].PHY_success = check.PHY_success;
				checkArr[client_index].buffer[stream_index].lat_success = check.lat_success;


                /* Close file pointers for this loop */
				fprintf(txt_pointer, "%s\n", divider);
				fprintf(txt_pointer, "\n");

				fclose(txt_pointer);
                fclose(csv_pointer);

				/* Push results datapoint struct to linked list */
				push_data( &start, report );


				// FOR CODE TESTING
				/*if ( &start != NULL ) {
					printf("Node contains the following: ");
					printList( start );
				}*/
				//printf("\nChecks are: %d %d %d %d %d\n",
				//	checkArr[client_index].buffer[stream_index].t_success,
				//	checkArr[client_index].buffer[stream_index].e1_RSSI_success,
				//	checkArr[client_index].buffer[stream_index].e2_RSSI_success,
				//	checkArr[client_index].buffer[stream_index].PHY_success,
				//	checkArr[client_index].buffer[stream_index].lat_success
				//);
				// FOR CODE TESTING


				/* Section off runs for visibility */
				printf("%s\n", divider);
				printf("\n");

                /* ------------------------ 3.1 ------------------------
                 * ****** CLOSE THE TESTINSTANCE! ******
                 */
                //client_counter++;
                printf("Completed test %d/%d!\n", run_counter, total_runs);
                printf("Closing test instance...\n");
                printf("\n");
                //run_counter++;

                rc = CHR_test_force_delete(test);
				Sleep(5000);

            } /* End of a single test permutation */

        }

        test_counter++;
		Sleep(5000);
	}	/* End of one full test iteration */

    /* ------------------------ Step 3.2 ------------------------
     * Calculate the final results
		Checks:
		t_success
		e1_RSSI_success
		e2_RSSI_success
		PHY_success
		lat_success
     */

	printf("FULL TEST RUN COMPLETED!\n");
	printf("\nFinal Results for %d test instances across %d client(s):\n",
			run_counter, client_count);
	printf("%s\n", divider);
	printf("\n");

	/* TXT final result setup */
	txt_pointer = fopen(txt_name, "a+");
	fprintf(txt_pointer, "Final Results for %d test instances across %d client(s):\n",
			run_counter, client_count);
	fprintf(txt_pointer, "%s\n", divider);
	fprintf(txt_pointer, "\n");

	/* CSV final result setup */
	csv_pointer = fopen(csv_name, "a+");
	fprintf(csv_pointer, "\nFINAL RESULTS:\n");
	fprintf(csv_pointer, "Total Average-%d Runs, Throughput, Min, Max, Run, E1 RSSI, Min, Max, Run, E2 RSSI, Min, Max, Run, PHY Rate, Run, Latency, Run\n", test_iterations);
	fprintf(csv_pointer, "DUT-LOC-CH-#Streams, %s, %s, %s, Rate, %s, %s, %s, Rate, %s, %s, %s, Rate, %s, Rate, %s, Rate\n",
			units[1], units[1], units[1], units[1], units[1], units[1], units[1], units[1], units[1], units[1], units[1]);

	Datapoint *calculation;
	for( int c = 0; c < client_count; c++ ) {
		for ( int s = 0; s < streamTests; s++ ) {

			/* Pull the success counter of the current test permutation */
			check.t_success			= checkArr[c].buffer[s].t_success;
			check.e1_RSSI_success	= checkArr[c].buffer[s].e1_RSSI_success;
			check.e2_RSSI_success	= checkArr[c].buffer[s].e2_RSSI_success;
			check.PHY_success		= checkArr[c].buffer[s].PHY_success;
			check.lat_success		= checkArr[c].buffer[s].lat_success;
			
			/* Pull the report node data of the current test permutation
			 * Adds the permutation to total buffer and divide by success count for the total average
			 */
			header.client = client_id[c];
			header.streams = streamNum[s];
			calculation = scan_data( &start, c, s );

			// FOR CODE TESTING
			//printf("TESTING CODE:\n");
			//printf("Calculation data is: %d %d %f %f %f %f %f %f %f %f %f %f %f\n",
			//	calculation->ep_id,
			//	calculation->stream_id,
			//	calculation->throughput,
			//	calculation->throughput_min,
			//	calculation->throughput_max,
			//	calculation->e1_RSSI,
			//	calculation->e1_RSSI_min,
			//	calculation->e1_RSSI_max,
			//	calculation->e2_RSSI,
			//	calculation->e2_RSSI_min,
			//	calculation->e2_RSSI_max,
			//	calculation->PHY_rate,
			//	calculation->latency
			//);

			//printf("\nChecks are: %d %d %d %d %d\n",
			//	checkArr[client_index].buffer[stream_index].t_success,
			//	checkArr[client_index].buffer[stream_index].e1_RSSI_success,
			//	checkArr[client_index].buffer[stream_index].e2_RSSI_success,
			//	checkArr[client_index].buffer[stream_index].PHY_success,
			//	checkArr[client_index].buffer[stream_index].lat_success
			//);
			//printf("\n");
			// FOR CODE TESTING

			total.ep_id				= calculation->ep_id;
			total.stream_id			= calculation->stream_id;

			total.throughput		= calculation->throughput/check.t_success;
			total.throughput_min	= calculation->throughput_min/check.t_success;
			total.throughput_max	= calculation->throughput_max/check.t_success;
			total.e1_RSSI			= calculation->e1_RSSI/check.e1_RSSI_success;
			total.e1_RSSI_min		= calculation->e1_RSSI_min/check.e1_RSSI_success;
			total.e1_RSSI_max		= calculation->e1_RSSI_max/check.e1_RSSI_success;
			total.e2_RSSI			= calculation->e2_RSSI/check.e2_RSSI_success;
			total.e2_RSSI_min		= calculation->e2_RSSI_min/check.e2_RSSI_success;
			total.e2_RSSI_max		= calculation->e2_RSSI_max/check.e2_RSSI_success;
			total.PHY_rate			= calculation->PHY_rate/check.PHY_success;
			total.latency			= calculation->latency/check.lat_success;

			output_final( txt_pointer, csv_pointer, total, &check, header, test_iterations, s );
		}
	}
    /* ------------------------ END ------------------------*/
	fclose(txt_pointer);
	fclose(csv_pointer);

	printf("Ending full test run...\n");

	Sleep(60000);

	exit(EXIT_SUCCESS);
}


/***************************************************************************
 ******************************** FUNCTIONS ********************************
 **************************************************************************/

/* ********* (1) GET CURRENT TIME FUNCTION *********
 *
 * Get the current time in format Date-[Month]-[Day]-[Year]_Time-[Hour]-[Minute][AM/PM]
 * Made as a function that can be implemented in other scripts
 */
static void get_current_time( char *time_var ) {

 	/* Determine system time using the time.h library */
 	time_t raw_time;
 	struct tm *time_data;
	time_data = malloc(sizeof(time_data));
 	time( &raw_time );
 	time_data = localtime( &raw_time );

 	/* Convert the raw time into the specified format */
 	char timestamp[30];
 	strftime( timestamp, sizeof(timestamp), "Date-%m-%d-%Y_Time-%H-%m%p", time_data );
 	//printf("timestamp variable is %s", timestamp);	// TEST

 	/* Copy full file name to the name variable */
 	strcpy(time_var, timestamp);
	free(time_data);

 }	/* ********* End of TEST INFORMATION FUNCTION ********* */


/* ********* (2) DETERMINE THE FILE NAME FUNCTION *********
*
* [data_path]/[description]_[timestamp]_[location]_[wifi_channel].txt
* Made as a function that can be implemented in other scripts
*/
static void get_file_name(char *name_var, char *csv, char *txt, Filename info)
{

	/* Determine buffer size (characters) of the final string */
	size_t name_size = snprintf(NULL, 0, "%s/%s/%s_%s_%s_%s", info.data_path, info.folder, info.description, info.timestamp, info.location, info.wifi_channel);
	int size_buffer = name_size + 1;
	char *file_buffer = malloc(size_buffer);					// Allocate memory for the file name

	/* Combine information strings with snprint() */
	//snprintf(file_name, size_buffer, "%s/%s/%s_%s_%s_%s.txt", data_path, folder, description, timestamp, location, wifi_channel);
	snprintf(file_buffer, size_buffer, "%s/%s/", info.data_path, info.folder);
	/* Create directory if it does not exist */
	int check_directoy = mkdir(file_buffer, 0777);
	snprintf(file_buffer, size_buffer, "%s/%s/%s_%s_%s_%s", info.data_path, info.folder, info.description, info.timestamp, info.location, info.wifi_channel);

	memcpy(name_var, file_buffer, size_buffer);					// Copy full file name to the name variable
	sprintf(csv, "%s.csv", file_buffer);
	sprintf(txt, "%s.txt", file_buffer);

	free(file_buffer);											// Clear the local file_name buffer

}	/* ********* End of FILE NAME FUNCTION ********* */


/* ********* (3) SET ROW HEADER .csv FUNCTION *********
 *
 * Writes a formatted client identifier to the first colum of a row
 * Useful for setting row identifier
 * Made as a function that can be implemented in other scripts
 */
static void set_row_header(char *curr_header, Header info) {

	memset(curr_header, 0, 50);
	snprintf(curr_header, 51, "%s-%s-%s-%d", info.client, info.location, info.wifi_channel, info.streams);
	printf("Identifier is %s\n", curr_header);

}	/* ********* End of ROW HEADER FUNCTION ********* */

 /* ********* (3) WRITE TO CSV ROW RESULTS FUNCTION *********
  *
  * Writes a variable number of elements to a row in .csv_name
  * Adds in commas and a '\n' at the end of the row
  * Made as a function that can be implemented in other scripts
  */
//static void write_to_csv( FILE *file_pointer, int num_col, ... ) {
//
//  	double current;
//  	va_list valist;
//  	va_start(valist, num_col);
//
//  	printf("Writing data to CSV file: ");   // TEST
//  	current = va_arg(valist, double);
//
//     /* First element of row */
//  	fprintf(file_pointer, "%f", current);
//  	printf("%f", current);
//
//  	for (int n = 1; n < num_col; n++) {
//  		current = va_arg(valist, double);
//  		printf(", %f", current);			// TEST
//  		fprintf(file_pointer, ", %f", current);
//  	}
//  	va_end(valist);
//  	fprintf(file_pointer, "\n");
//    printf("\n");
//
// }   /* ********* End of WRITE TO CSV FUNCTION ********* */


///* ********* Successful Run Counter Allocation Function(s) **********/
//void allocate_success(Success ** stream_pointer, int streamLength) {
//
//	Success *success;
//	success = malloc(streamLength * sizeof(Success *));
//
//	for (int s_s = 0; s_s < streamLength; s_s++) {
//
//		success[s_s].t_success = 0;
//		success[s_s].e1_RSSI_success = 0;
//		success[s_s].e2_RSSI_success = 0;
//		success[s_s].PHY_success = 0;
//		success[s_s].lat_success = 0;
//
//	}
//	*stream_pointer = success;
//	//free(success);
//}
//void allocate_successArr(SuccessArr ** client_pointer, int clientLength) {
//
//	SuccessArr *successArr;
//	successArr = malloc(clientLength * sizeof(SuccessArr *));
//
//	for (int s_c = 0; s_c < clientLength; s_c++) {
//
//		successArr[s_c].buffer = malloc(sizeof(Success*));
//
//	}
//
//	*client_pointer = successArr;
//	//free(successArr);
//}
///* ********* END Run Counter Allocation Function(s) **********/


/* ********* Scan Data Nodes Function **********/
Datapoint *scan_data( struct Node **head, int client, int stream ) {

	struct Node *current = *head;
	Datapoint *sum;

	//buffer = malloc(sizeof(Datapoint));
	sum = malloc(sizeof(Datapoint));
	memset(sum, 0, sizeof(Datapoint));

	while (current != NULL) {

		if ((current->test_data.ep_id == client) && (current->test_data.stream_id == stream)) {

			sum->ep_id		= current->test_data.ep_id;
			sum->stream_id	= current->test_data.stream_id;

			sum->throughput			+= current->test_data.throughput;
			sum->throughput_min		+= current->test_data.throughput_min;
			sum->throughput_max		+= current->test_data.throughput_max;
			sum->e1_RSSI			+= current->test_data.e1_RSSI;
			sum->e1_RSSI_min		+= current->test_data.e1_RSSI_min;
			sum->e1_RSSI_max		+= current->test_data.e1_RSSI_max;
			sum->e2_RSSI			+= current->test_data.e2_RSSI;
			sum->e2_RSSI_min		+= current->test_data.e2_RSSI_min;
			sum->e2_RSSI_max		+= current->test_data.e2_RSSI_max;
			sum->PHY_rate			+= current->test_data.PHY_rate;
			sum->latency			+= current->test_data.latency;

			/*struct Node *temp = node->next;
			node->test_data = node->next->test_data;
			node->next = temp->next;

			free(temp);*/
			//break;
		}

		current = current->next;
	}

	return sum;
}
/* ********* END Scan Data Nodes Function **********/


/* ********* Output Final Results Function **********/
void output_final( FILE *txt_pointer, FILE *csv_pointer,
	Datapoint total , Success *counter, Header header, int *iterations ) {

	/* CONSOLE OUTPUT */
	printf("%s-%s-%s-%d Streams\n", header.client, header.location, header.wifi_channel, header.streams);
	printf("Total Average Throughput:	%.3f  Min: %.3f  Max: %.3f	of %d/%d runs\n", total.throughput, total.throughput_min, total.throughput_max, counter->t_success, iterations);
	printf("Average Endpoint 1 RSSI:	%.3f  Min: %.3f  Max: %.3f  of %d/%d runs\n", total.e1_RSSI, total.e1_RSSI_min, total.e1_RSSI_max, counter->e1_RSSI_success, iterations);
	printf("Average Endpoint 2 RSSI:	%.3f  Min: %.3f  Max: %.3f  of %d/%d runs\n", total.e2_RSSI, total.e2_RSSI_min, total.e2_RSSI_max, counter->e2_RSSI_success, iterations);
	printf("Average PHY Rate:			%.3f						of %d/%d runs\n", total.PHY_rate, counter->PHY_success, iterations);
	printf("Response Time:				%.3f						of %d/%d runs\n", total.latency, counter->lat_success, iterations);
	printf("\n");

	/* TXT FORMATTING */
	fprintf(txt_pointer, "%s-%s-%s-%d Streams\n", header.client, header.location, header.wifi_channel, header.streams);
	fprintf(txt_pointer, "Total Average Throughput:	%.3f  Min: %.3f  Max: %.3f	of %d/%d runs\n", total.throughput, total.throughput_min, total.throughput_max, counter->t_success, iterations);
	fprintf(txt_pointer, "Average Endpoint 1 RSSI:  %.3f  Min: %.3f  Max: %.3f	of %d/%d runs\n", total.e1_RSSI, total.e1_RSSI_min, total.e1_RSSI_max, counter->e1_RSSI_success, iterations);
	fprintf(txt_pointer, "Average Endpoint 2 RSSI:  %.3f  Min: %.3f  Max: %.3f	of %d/%d runs\n", total.e2_RSSI, total.e2_RSSI_min, total.e2_RSSI_max, counter->e2_RSSI_success, iterations);
	fprintf(txt_pointer, "Average PHY Rate:	        %.3f						of %d/%d runs\n", total.PHY_rate, counter->PHY_success, iterations);
	fprintf(txt_pointer, "Response Time:		    %.3f						of %d/%d runs\n", total.latency, counter->lat_success, iterations);
	fprintf(txt_pointer, "\n");

	/* CSV FORMATTING */
	//fprintf("Total Average - %d Runs, Throughput, Min, Max, Run, E1 RSSI, Min, Max, Run, E2 RSSI, Min, Max, Run, PHY Rate, Run, Latency, Run\n", test_iterations );
	//fprintf("DUT-Loc-# Streams, %s, %s, %s, Rate, %s, %s, %s, Rate, %s, %s, %s, Rate, %s, Rate, %s, Rate\n", units[1]);

	fprintf(csv_pointer, "%s-%s-%s-%d, %.3f, %.3f, %.3f, %d/%d, %.3f, %.3f, %.3f, %d/%d, %.3f, %.3f, %.3f, %d/%d, %.3f, %d/%d, %.3f, %d/%d\n",
			header.client, header.location, header.wifi_channel, header.streams,
			total.throughput, total.throughput_min, total.throughput_max, counter->t_success, iterations,
			total.e1_RSSI, total.e1_RSSI_min, total.e1_RSSI_max, counter->e1_RSSI_success, iterations,
			total.e2_RSSI, total.e2_RSSI_min, total.e2_RSSI_max, counter->e2_RSSI_success, iterations,
			total.PHY_rate, counter->PHY_success, iterations, total.latency, counter->lat_success, iterations);

}
/* ********* END Final Results Function **********/


/**************************************************************
 *
 * Print results common to pairs mpairs, and timing records.
 * Show "n/a" is NO_SUCH_VALUE is returned.
 *
 * Parameters: handle to object for which to get results
 *
 * Note: If the attempt to get any results fails, a message
 *       is printed showing the reason and we return rather
 *       than force exit of the program, since an error when
 *       getting results is not necessarily a fatal condition.
 **************************************************************/
static void show_common(CHR_HANDLE handle) {

	 CHR_API_RC rc;
	 CHR_FLOAT  float_result;
	 CHR_COUNT  count_result;

	 /*
	  * These do not have NO_SUCH_VALUE as a possible return,
	  * so anything other than success is an error
	  */
	 rc = CHR_common_results_get_meas_time(handle, &float_result);
	 if (rc != CHR_OK) {
		 show_results_error(rc, "meas_time");
		 return;
	 }
	 printf("  Measured time           : %.3f\n", float_result);

	 rc = CHR_common_results_get_trans_count(handle, &float_result);
	 if (rc != CHR_OK) {
		 show_results_error(rc, "trans_count");
		 return;
	 }
	 printf("  Transaction count       : %.0f\n", float_result);

	 /*
	  * These could have NO_SUCH_VALUE as a valid return,
	  * depending on the specifics of the test.
	  */
	 rc = CHR_common_results_get_bytes_sent_e1(handle, &float_result);
	 if (rc != CHR_OK) {

		 if (rc == CHR_NO_SUCH_VALUE) {
			 printf("  Bytes sent E1            : n/a\n");
		 }
		 else {
			 show_results_error(rc, "bytes_sent_e1");
			 return;
		 }
	 }
	 else {
		 printf("  Bytes sent E1           : %.0f\n", float_result);
	 }

	 rc = CHR_common_results_get_bytes_recv_e1(handle, &float_result);
	 if (rc != CHR_OK) {

		 if (rc == CHR_NO_SUCH_VALUE) {
			 printf("  Bytes recv E1           : n/a\n");
		 }
		 else {
			 show_results_error(rc, "bytes_recv_e1");
			 return;
		 }
	 }
	 else {
		 printf("  Bytes recv E1           : %.0f\n", float_result);
	 }

	 rc = CHR_common_results_get_bytes_recv_e2(handle, &float_result);
	 if (rc != CHR_OK) {

		 if (rc == CHR_NO_SUCH_VALUE) {
			 printf("  Bytes recv E2           : n/a\n");
		 }
		 else {
			 show_results_error(rc, "bytes_recv_e2");
			 return;
		 }
	 }
	 else {
		 printf("  Bytes recv E2           : %.0f\n", float_result);
	 }

	 rc = CHR_common_results_get_dg_sent_e1(handle, &float_result);
	 if (rc != CHR_OK) {

		 if (rc == CHR_NO_SUCH_VALUE) {
			 printf("  DGs sent E1             : n/a\n");
		 }
		 else {
			 show_results_error(rc, "dg_sent_e1");
			 return;
		 }
	 }
	 else {
		 printf("  DGs sent E1             : %.0f\n", float_result);
	 }

	 rc = CHR_common_results_get_dg_recv_e2(handle, &float_result);
	 if (rc != CHR_OK) {

		 if (rc == CHR_NO_SUCH_VALUE) {
			 printf("  DGs recv E2             : n/a\n");
		 }
		 else {
			 show_results_error(rc, "dg_recv_e2");
			 return;
		 }
	 }
	 else {
		 printf("  DGs recv E2             : %.0f\n", float_result);
	 }

	 rc = CHR_common_results_get_dg_dup_recv_e2(handle, &float_result);
	 if (rc != CHR_OK) {

		 if (rc == CHR_NO_SUCH_VALUE) {
			 printf("  DGs dup recv E2         : n/a\n");
		 }
		 else {
			 show_results_error(rc, "dg_dup_recv_e2");
			 return;
		 }
	 }
	 else {
		 printf("  DGs dup recv E2         : %.0f\n", float_result);
	 }

	 rc = CHR_common_results_get_dg_lost_e1_to_e2(handle, &float_result);
	 if (rc != CHR_OK) {

		 if (rc == CHR_NO_SUCH_VALUE) {
			 printf("  DGs lost E1 to E2       : n/a\n");
		 }
		 else {
			 show_results_error(rc, "dg_lost_e1_to_e2");
			 return;
		 }
	 }
	 else {
		 printf("  DGs lost E1 to E2       : %.0f\n", float_result);
	 }

	 rc = CHR_common_results_get_dg_out_of_order(handle, &float_result);
	 if (rc != CHR_OK) {

		 if (rc == CHR_NO_SUCH_VALUE) {
			 printf("  DGs out of order        : n/a\n");
		 }
		 else {
			 show_results_error(rc, "dg_out_of_order");
			 return;
		 }
	 }
	 else {
		 printf("  DGs out of order        : %.0f\n", float_result);
	 }

	 /* TCP Statistics */
	 rc = CHR_common_results_get_e1_syn_tx(handle, &count_result);
	 if (rc != CHR_OK) {

		 if (rc == CHR_NO_SUCH_VALUE) {
			 printf("  Syn sent                : n/a\n");
		 }
		 else {
			 show_results_error(rc, "Syn sent");
			 return;
		 }
	 }
	 else {
		 printf("  Syn sent                : %d\n", count_result);
	 }

	 rc = CHR_common_results_get_e1_syn_rx(handle, &count_result);
	 if (rc != CHR_OK) {

		 if (rc == CHR_NO_SUCH_VALUE) {
			 printf("  Syn received            : n/a\n");
		 }
		 else {
			 show_results_error(rc, "Syn received");
			 return;
		 }
	 }
	 else {
		 printf("  Syn received            : %d\n", count_result);
	 }

	 rc = CHR_common_results_get_e1_syn_failed(handle, &count_result);
	 if (rc != CHR_OK) {

		 if (rc == CHR_NO_SUCH_VALUE) {
			 printf("  Syn failed              : n/a\n");
		 }
		 else {
			 show_results_error(rc, "Syn failed");
			 return;
		 }
	 }
	 else {
		 printf("  Syn failed              : %d\n", count_result);
	 }

	 rc = CHR_common_results_get_e1_conn_established(handle, &count_result);
	 if (rc != CHR_OK) {

		 if (rc == CHR_NO_SUCH_VALUE) {
			 printf("  Connections established : n/a\n");
		 }
		 else {
			 show_results_error(rc, "Connections established");
			 return;
		 }
	 }
	 else {
		 printf("  Connections established : %d\n", count_result);
	 }

	 rc = CHR_common_results_get_e1_fin_tx(handle, &count_result);
	 if (rc != CHR_OK) {

		 if (rc == CHR_NO_SUCH_VALUE) {
			 printf("  Fin sent                : n/a\n");
		 }
		 else {
			 show_results_error(rc, "Fin sent");
			 return;
		 }
	 }
	 else {
		 printf("  Fin sent                : %d\n", count_result);
	 }

	 rc = CHR_common_results_get_e1_fin_rx(handle, &count_result);
	 if (rc != CHR_OK) {

		 if (rc == CHR_NO_SUCH_VALUE) {
			 printf("  Fin received            : n/a\n");
		 }
		 else {
			 show_results_error(rc, "Fin received");
			 return;
		 }
	 }
	 else {
		 printf("  Fin received            : %d\n", count_result);
	 }

	 rc = CHR_common_results_get_e1_ack_to_fin_tx(handle, &count_result);
	 if (rc != CHR_OK) {

		 if (rc == CHR_NO_SUCH_VALUE) {
			 printf("  Ack to Fin sent         : n/a\n");
		 }
		 else {
			 show_results_error(rc, "Ack to Fin sent");
			 return;
		 }
	 }
	 else {
		 printf("  Ack to Fin sent         : %d\n", count_result);
	 }

	 rc = CHR_common_results_get_e1_ack_to_fin_rx(handle, &count_result);
	 if (rc != CHR_OK) {

		 if (rc == CHR_NO_SUCH_VALUE) {
			 printf("  Ack to Fin received     : n/a\n");
		 }
		 else {
			 show_results_error(rc, "Ack to Fin received");
			 return;
		 }
	 }
	 else {
		 printf("  Ack to Fin received     : %d\n", count_result);
	 }

	 rc = CHR_common_results_get_e1_rst_tx(handle, &count_result);
	 if (rc != CHR_OK) {

		 if (rc == CHR_NO_SUCH_VALUE) {
			 printf("  Reset sent              : n/a\n");
		 }
		 else {
			 show_results_error(rc, "Reset sent");
			 return;
		 }
	 }
	 else {
		 printf("  Reset sent              : %d\n", count_result);
	 }

	 rc = CHR_common_results_get_e1_rst_rx(handle, &count_result);
	 if (rc != CHR_OK) {

		 if (rc == CHR_NO_SUCH_VALUE) {
			 printf("  Reset received          : n/a\n");
		 }
		 else {
			 show_results_error(rc, "Reset received");
			 return;
		 }
	 }
	 else {
		 printf("  Reset received          : %d\n", count_result);
	 }

	 rc = CHR_common_results_get_e1_tcp_retransmissions(handle, &count_result);
	 if (rc != CHR_OK) {

		 if (rc == CHR_NO_SUCH_VALUE) {
			 printf("  TCP retransmissions     : n/a\n");
		 }
		 else {
			 show_results_error(rc, "TCP retransmissions");
			 return;
		 }
	 }
	 else {
		 printf("  TCP retransmissions     : %d\n", count_result);
	 }

	 rc = CHR_common_results_get_e1_tcp_timeouts(handle, &count_result);
	 if (rc != CHR_OK) {

		 if (rc == CHR_NO_SUCH_VALUE) {
			 printf("  TCP timeouts            : n/a\n");
		 }
		 else {
			 show_results_error(rc, "TCP timeouts");
			 return;
		 }
	 }
	 else {
		 printf("  TCP timeouts            : %d\n", count_result);
	 }

 }


/* ********* GET TEST RUN RESULTS FUNCTION *********
 *
 * Get the current test results and print to console and .txt
 * Pulls the average, min, and max values of the specified datapoints
 * Made as a function that can be implemented in other scripts
 */
static void show_results( FILE *TXT_pointer, FILE *CSV_pointer,
	 Datapoint *report, Success *counter, CHR_HANDLE test )
 {

	 CHR_PAIR_HANDLE     pair; // Local handle for current pair
	 CHR_COUNT			pr_index, pairCount;

	 int t_count = 0, r1_count = 0, r2_count = 0,
		 PHY_count = 0, lat_count = 0;

	 double t_total = 0, r1_total = 0, r2_total = 0,
		 PHY_total = 0, lat_total = 0;

	 CHR_FLOAT t_min, r1_min, r2_min, PHY_min, lat_min;
	 CHR_FLOAT t_max, r1_max, r2_max, PHY_max, lat_max;

	 double total_t_min = 0, total_t_max = 0,
		 total_r1_min = 0, total_r1_max = 0, total_r2_min = 0, total_r2_max = 0;
	 //CHR_FLOAT temp1, temp2;

	 CHR_FLOAT t_avg, r1_avg, r2_avg, PHY_avg, lat_avg;

	 CHR_API_RC rc;
	 rc = CHR_test_get_pair_count(test, &pairCount);
	 if (rc != CHR_OK) {
		 show_error(test, rc, "test_get_pair_count");
	 }

	 /* .csv data formatting. Header has been written already on new line */
	 fprintf(CSV_pointer,
		 "Avg Throughput, Avg Min, Avg Max,Avg Ep1 RSSI, Ep1 RSSI Min, Ep1 RSSI Max, Avg Ep2 RSSI, Ep2 RSSI Min, Ep2 RSSI Max, Avg PHY Rate, Avg Latency\n");


	 for (pr_index = 0; pr_index < pairCount; pr_index++) {

		 //char buffer[CHR_MAX_FILE_PATH];

		 /* Print pair definition */
		 printf("Pair %d:", pr_index + 1);
		 fprintf(TXT_pointer, "Pair %d:", pr_index + 1);
		 fprintf(CSV_pointer, "Pair %d, ", pr_index + 1);

		 rc = CHR_test_get_pair(test, pr_index, &pair);
		 if (rc != CHR_OK) {
			 show_error(test, rc, "test_get_pair");
		 }
		 // rc = CHR_pair_get_comment(pair, buffer,
		 //                           CHR_MAX_PAIR_COMMENT, &len);
		 // if (rc != CHR_OK ) {
		 //     show_error(pair, rc, "pair_get_comment");
		 // }
		 // printf("  Comment            : %s\n", buffer);
		 // rc = CHR_pair_get_e1_addr(pair, buffer,
		 //                           CHR_MAX_ADDR, &len);
		 // if (rc != CHR_OK ) {
		 //     show_error(pair, rc, "pair_get_e1_addr");
		 // }
		 // printf("  E1 address         : %s\n", buffer);
		 //rc = CHR_pair_get_e2_addr(pair, buffer,
		 //                          CHR_MAX_ADDR, &len);
		 //if (rc != CHR_OK ) {
		 //    show_error(pair, rc, "pair_get_e2_addr");
		 //}
		 //printf("%s", buffer);

		 /* ************************************************ */
		 /* ****************** THROUGHPUT ****************** */
		 /* ************************************************ */
		 rc = CHR_pair_results_get_average(pair,
			 CHR_RESULTS_THROUGHPUT, &t_avg);
		 if (rc != CHR_OK) {

			 if (rc == CHR_NO_SUCH_VALUE) {
				 printf("  Throughput: n/a ");
				 fprintf(TXT_pointer, "  Throughput: n/a ");
				 fprintf(CSV_pointer, "n/a, ");
			 }
			 else {
				 show_results_error(rc, "throughput avg");
				 return;
			 }
		 }
		 else {

			 t_count++;
			 t_total += t_avg;

			 rc = CHR_pair_results_get_minimum(pair,
				 CHR_RESULTS_THROUGHPUT, &t_min);
			 if (rc != CHR_OK) {
				 show_results_error(rc, "throughput min");
				 return;
			 }
			 rc = CHR_pair_results_get_maximum(pair,
				 CHR_RESULTS_THROUGHPUT, &t_max);
			 if (rc != CHR_OK) {
				 show_results_error(rc, "throughput max");
				 return;
			 }

			 /* Determine the final max and min */
			 if ((total_t_min == 0) || (t_min < total_t_min)) {
				 total_t_min = t_min;
			 }

			 if (total_t_max < t_max) {
				 total_t_max = t_max;
			 }

			 printf("  Throughput: avg %.3f   min %.3f   max %.3f\n",
				 t_avg, t_min, t_max);
			 fprintf(TXT_pointer, "  Throughput: avg %.3f   min %.3f   max %.3f\n",
				 t_avg, t_min, t_max);
			 fprintf(CSV_pointer, "%.3f, %.3f, %.3f, ",
				 t_avg, t_min, t_max);
		 }

		 /* ***************************************************** */
		 /* ****************** RSSI Endpoint 1 ****************** */
		 /* ***************************************************** */
		 rc = CHR_pair_results_get_average(pair,
			 CHR_RESULTS_RSSI_E1, &r1_avg);
		 if (rc != CHR_OK) {

			 if (rc == CHR_NO_SUCH_VALUE) {
				 printf("  Ep1 RSSI: n/a ");
				 fprintf(TXT_pointer, "  Ep 1 RSSI: n/a ");
				 fprintf(CSV_pointer, "n/a, n/a, n/a, ");
			 }
			 else {
				 show_results_error(rc, "Endpoint 1 RSSI avg");
				 return;
			 }
		 }
		 else {

			 r1_count++;
			 r1_total += r1_avg;

			 rc = CHR_pair_results_get_minimum(pair,
				 CHR_RESULTS_RSSI_E1, &r1_min);
			 if (rc != CHR_OK) {
				 show_results_error(rc, "Endpoint 1 RSSI min");
				 return;
			 }
			 rc = CHR_pair_results_get_maximum(pair,
				 CHR_RESULTS_RSSI_E1, &r1_max);
			 if (rc != CHR_OK) {
				 show_results_error(rc, "Endpoint 1 RSSI max");
				 return;
			 }

			 /* Determine the final max and min */
			 if ((total_r1_min == 0) || (r1_min < total_r1_min)) {
				 total_r1_min = r1_min;
			 }

			 if (total_r1_max < r1_max) {
				 total_r1_max = r1_max;
			 }

			 printf("  Ep1 RSSI: avg %.3f",
				 r1_avg);
			 fprintf(TXT_pointer, "  Ep1 RSSI: avg %.3f", r1_avg);
			 fprintf(CSV_pointer, "%.3f, n/a, n/a, ", r1_avg);
		 }

		 /* ***************************************************** */
		 /* ****************** RSSI Endpoint 2 ****************** */
		 /* ***************************************************** */
		 rc = CHR_pair_results_get_average(pair,
			 CHR_RESULTS_RSSI_E1, &r2_avg);
		 if (rc != CHR_OK) {

			 if (rc == CHR_NO_SUCH_VALUE) {
				 printf("  Ep 2 RSSI: n/a ");
				 fprintf(TXT_pointer, "  Ep 2 RSSI: n/a ");
				 fprintf(CSV_pointer, "n/a, n/a, n/a, ");
			 }
			 else {
				 show_results_error(rc, "Endpoint 2 RSSI avg");
				 return;
			 }
		 }
		 else {

			 r2_count++;
			 r2_total += r2_avg;

			 rc = CHR_pair_results_get_minimum(pair,
				 CHR_RESULTS_RSSI_E2, &r2_min);
			 if (rc != CHR_OK) {
				 show_results_error(rc, "Endpoint 2 RSSI min");
				 return;
			 }
			 rc = CHR_pair_results_get_maximum(pair,
				 CHR_RESULTS_RSSI_E2, &r2_max);
			 if (rc != CHR_OK) {
				 show_results_error(rc, "Endpoint 2 RSSI max");
				 return;
			 }

			 /* Determine the final max and min */
			 if ((total_r2_min == 0) || (r2_min < total_r2_min)) {
				 total_r2_min = r2_min;
			 }

			 if (total_r2_max < r2_max) {
				 total_r2_max = r2_max;
			 }

			 printf("  Ep2 RSSI: avg %.3f", r2_avg);
			 fprintf(TXT_pointer, "  Ep2 RSSI: avg %.3f", r2_avg);
			 fprintf(CSV_pointer, "%.3f, n/a, n/a, ", r2_avg);
		 }

		 /* ********************************************** */
		 /* ****************** PHY RATE ****************** */
		 /* ********************************************** */
		 rc = CHR_pair_results_get_average(pair,
			 CHR_RESULTS_TRANSACTION_RATE, &PHY_avg);
		 if (rc != CHR_OK) {

			 if (rc == CHR_NO_SUCH_VALUE) {
				 printf("  PHY: n/a ");
				 fprintf(TXT_pointer, "  PHY: n/a ");
				 fprintf(CSV_pointer, "n/a, ");
			 }
			 else {
				 show_results_error(rc, "transaction rate avg");
				 return;
			 }
		 }
		 else {

			 PHY_count++;
			 PHY_total += PHY_avg;

			 /*
			  * If we get the average, we should be able
			  * to get the minimum and maximum too
			  */
			 rc = CHR_pair_results_get_minimum(pair,
				 CHR_RESULTS_TRANSACTION_RATE, &PHY_min);
			 if (rc != CHR_OK) {
				 show_results_error(rc, "transaction rate min");
				 return;
			 }
			 rc = CHR_pair_results_get_maximum(pair,
				 CHR_RESULTS_TRANSACTION_RATE, &PHY_max);
			 if (rc != CHR_OK) {
				 show_results_error(rc, "transaction rate max");
				 return;
			 }
			 printf("  PHY: avg %.3f", PHY_avg);
			 fprintf(TXT_pointer, "  PHY: avg %.3f", PHY_avg);
			 fprintf(CSV_pointer, "%.3f, ", PHY_avg);
		 }

		 /* ********************************************* */
		 /* ****************** LATENCY ****************** */
		 /* ********************************************* */
		 rc = CHR_pair_results_get_average(pair,
			 CHR_RESULTS_RESPONSE_TIME, &lat_avg);
		 if (rc != CHR_OK) {

			 if (rc == CHR_NO_SUCH_VALUE) {
				 printf("  Response time: n/a ");
				 fprintf(TXT_pointer, "  Response time: n/a ");
				 fprintf(CSV_pointer, "n/a, ");
			 }
			 else {
				 show_results_error(rc, "response time avg");
				 return;
			 }
		 }
		 else {

			 lat_count++;
			 lat_total += lat_avg;

			 rc = CHR_pair_results_get_minimum(pair,
				 CHR_RESULTS_RESPONSE_TIME, &lat_min);
			 if (rc != CHR_OK) {
				 show_results_error(rc, "response time min");
				 return;
			 }
			 rc = CHR_pair_results_get_maximum(pair,
				 CHR_RESULTS_RESPONSE_TIME, &lat_max);
			 if (rc != CHR_OK) {
				 show_results_error(rc, "response time max");
				 return;
			 }
			 printf("  Response time: avg %.3f\n", lat_avg);
			 fprintf(TXT_pointer, "  Response time: avg %.3f\n", lat_avg);
			 fprintf(CSV_pointer, "%.3f\n", lat_avg);
		 }
		 /* IMPORTANT: add \n to the prints of the last datapoint if desired*/
	 }

	 /* ************************************************** */
	 /* ****************** CALCULATIONS ****************** */
	 /* ************************************************** */
	 /* Check: t_success, e1_RSSI_success, e2_RSSI_success,
		PHY_success, lat_success */
		//show_common(timingRec);
	 printf("\nTotal average of %d pairs:\n", pairCount);
	 fprintf(TXT_pointer, "\nTotal of %d pairs:\n", pairCount);
	 /* Total throughput */
	 t_avg = t_total / t_count;
	 if (t_avg > 0) { counter->t_success += 1; }
	 report->throughput = t_total;
	 report->throughput_min = total_t_min;
	 report->throughput_max = total_t_max;
	 printf("Throughput: total %.3f  avg %.3f  min %.3f  max %.3f\n",
		 t_total, t_avg, total_t_min, total_t_max);
	 fprintf(TXT_pointer, "Throughput: total %.3f  avg %.3f  min %.3f  max %.3f\n",
		 t_total, t_avg, total_t_min, total_t_max);
	 fprintf(CSV_pointer, "Total, %.3f, %.3f, %.3f, ", t_total, total_t_min, total_t_max);
	 /* Total RSSI */
	 r1_avg = r1_total / r1_count;
	 if (r1_avg > 0) { counter->e1_RSSI_success += 1; }
	 report->e1_RSSI = r1_avg;
	 report->e1_RSSI_min = total_r1_min;
	 report->e1_RSSI_max = total_r1_max;
	 printf("Ep1 RSSI: avg %.3f  min %.3f  max %.3f\n",
		 r1_avg, total_r1_min, total_r1_max);
	 fprintf(TXT_pointer, "Ep1 RSSI: avg %.3f  min %.3f  max %.3f\n",
		 r1_avg, total_r1_min, total_r1_max);
	 fprintf(CSV_pointer, "%.3f, %.3f, %.3f, ", r1_avg, total_r1_min, total_r1_max);

	 r2_avg = r2_total / r2_count;
	 if (r2_avg > 0) { counter->e2_RSSI_success += 1; }
	 report->e2_RSSI = r2_avg;
	 report->e2_RSSI_min = total_r2_min;
	 report->e2_RSSI_max = total_r2_max;
	 printf("Ep2 RSSI: avg %.3f  min %.3f  max %.3f\n",
		 r2_avg, total_r2_min, total_r2_max);
	 fprintf(TXT_pointer, "Ep2 RSSI: avg %.3f  min %.3f  max %.3f\n",
		 r2_avg, total_r2_min, total_r2_max);
	 fprintf(CSV_pointer, "%.3f, %.3f, %.3f, ", r2_avg, total_r2_min, total_r2_max);
	 /* Total PHY */
	 PHY_avg = PHY_total / PHY_count;
	 if (PHY_avg > 0) { counter->PHY_success += 1; }
	 report->PHY_rate = PHY_avg;
	 printf("PHY: avg %.3f\n", PHY_avg);
	 fprintf(TXT_pointer, "PHY: avg %.3f\n", PHY_avg);
	 fprintf(CSV_pointer, "%.3f, ", PHY_avg);
	 /* Total response time */
	 lat_avg = lat_total / lat_count;
	 if (lat_avg > 0) { counter->lat_success += 1; }
	 report->latency = lat_avg;
	 printf("Response time: avg %.3f\n", lat_avg);
	 fprintf(TXT_pointer, "Response time: avg %.3f\n", lat_avg);
	 fprintf(CSV_pointer, "%.3f\n", lat_avg);
	 /* Send totals to the final data struct */

	 printf("\n");
	 fprintf(TXT_pointer, "\n");
	 fprintf(CSV_pointer, "\n");

 }   /* ********* End of TEST RESULTS FUNCTION ********* */


/**************************************************************
 *
 * Local function to print results common to timing records.
 * Show "n/a" if NO_SUCH_VALUE is returned.
 *
 * Parameters: handle to object for which to get results
 *
 * Note: If the attempt to get any results fails, a message
 *       is printed showing the reason and we return to rather
 *       than force exit of the program, since an error when
 *       getting results is not necessarily a fatal condition.
 **************************************************************/
static void show_timing_rec(CHR_HANDLE timingRec) {

	 CHR_FLOAT  result;
	 CHR_API_RC rc;


	 /* These should be available in all timing records */
	 rc = CHR_timingrec_get_elapsed(timingRec, &result);
	 if (rc != CHR_OK) {
		 show_results_error(rc, "elapsed");
		 return;
	 }
	 printf("  Elapsed time      : %.3f\n", result);

	 rc = CHR_timingrec_get_inactive(timingRec, &result);
	 if (rc != CHR_OK) {
		 show_results_error(rc, "inactive");
		 return;
	 }
	 printf("  Inactive time     : %.3f\n", result);

	 /* Jitter is only available sometimes */
	 rc = CHR_timingrec_get_jitter(timingRec, &result);
	 if (rc != CHR_OK) {

		 if (rc == CHR_NO_SUCH_VALUE) {
			 printf("  Jitter time       : n/a\n");
		 }
		 else {
			 show_results_error(rc, "jitter");
			 return;
		 }
	 }
	 else {
		 printf("  Jitter time       : %.3f\n", result);
	 }

	 /* Show common results */
	 show_common(timingRec);
 }


/***************************************************************
 *
 * Print information about an error and exit. If there is
 * extended error information, print that too.
 *
 * Parameters: handle - what object had the error
 *             code   - the Chariot API return code
 *             where  - what function call failed
 ***************************************************************/
static void show_error(
    CHR_HANDLE handle,
    CHR_API_RC code,
    CHR_STRING where)
{
    char       msg[CHR_MAX_RETURN_MSG];
    CHR_LENGTH msgLen;

    char       errorInfo[CHR_MAX_ERROR_INFO];
    CHR_LENGTH errorLen;

    CHR_API_RC rc;


    /*
     * Get the API message for this return code.
     */
    rc = CHR_api_get_return_msg(code, msg,
                                CHR_MAX_RETURN_MSG, &msgLen);
    if (rc != CHR_OK) {

        /* Could not get the message: show why */
        printf("%s failed\n", where);
        printf(
          "Unable to get message for return code %d, rc = %d\n",
           code, rc);
    }
    else {

        /* Tell the user about the error */
        printf("%s failed: rc = %d (%s)\n", where, code, msg);
    }

    /*
     * See if there is extended error information available.
     * It's meaningful only after some error returns. After
     * failed "new" function calls, we don't have a handle so
     * we cannot check for extended error information.
     */
    if ((code == CHR_OPERATION_FAILED ||
         code == CHR_OBJECT_INVALID ||
         code == CHR_APP_GROUP_INVALID) &&
         handle != (CHR_HANDLE)NULL) {

        /* Get the extended error info */
        rc = CHR_common_error_get_info(handle,
                                       CHR_DETAIL_LEVEL_ALL,
                                       errorInfo,
                                       CHR_MAX_ERROR_INFO,
                                      &errorLen);
        if (rc == CHR_OK) {

            /*
             * We can ignore all non-success return codes here
             * because most should not occur (the api's been
             * initialized, the handle is good, the buffer
             * pointer is valid, and the detail level is okay),
             * and the NO_SUCH_VALUE return code here means
             * there is no info available.
             */
            printf("%s\n", errorInfo);
        }
    }

    /*
     * We've told everything we know, so now just exit.
     */
    exit(EXIT_FAILURE);
}


/***************************************************************
 *
 * Print information about an error that occurred while
 * getting results information. There is no extended error
 * information in these cases and we do not want to exit
 * this program as these are not fatal errors.
 *
 * Parameters: code   - the Chariot API return code
 *             what  - what function call failed
 ***************************************************************/
static void show_results_error(
    CHR_API_RC rc,
    CHR_STRING what)
{
    char msg[CHR_MAX_RETURN_MSG];

    CHR_LENGTH len;

    CHR_API_RC msgRc;


    printf("Get %s failed: ", what);
    msgRc = CHR_api_get_return_msg(rc, msg,
                                   CHR_MAX_RETURN_MSG,
                                  &len);
    if (msgRc == CHR_OK) {
        printf("%s\n", msg);
    }
    else {
        printf("rc = %d\n", rc );
    }
}


/***************************************************************
 *
 * Print the protocol string that corresponds to the macro
 * passed to the function (macros defined in chrapi.h)
 *
 * Parameters: protocol - macro defined in chrapi.h
 *
 ***************************************************************/
static char*
show_protocol_string(
	CHR_PROTOCOL protocol)
{
	switch (protocol) {
	case CHR_PROTOCOL_IPX:
		return "IPX";
	case CHR_PROTOCOL_SPX:
		return "SPX";
	case CHR_PROTOCOL_TCP:
		return "TCP";
	case CHR_PROTOCOL_TCP6:
		return "TCP-IPv6";
	case CHR_PROTOCOL_UDP:
		return "UDP";
	case CHR_PROTOCOL_UDP6:
		return "UDP-IPv6";
	case CHR_PROTOCOL_RTP:
		return "RTP";
	case CHR_PROTOCOL_RTP6:
		return "RTP-IPv6";
	default:
		return "Unknown protocol or protocol not set";
	};
}


/***************************************************************
 *
 * Print the method by which the test was stopped, denoted by
 * macro set and passed as a parameter into this function.
 *
 * Parameters: howEnded - macro defined in chrapi.h
 *
 ***************************************************************/
static char*
show_how_ended_string(
	CHR_TEST_HOW_ENDED howEnded)
{
	switch (howEnded) {
	case CHR_TEST_HOW_ENDED_USER_STOPPED:
		return "USER STOPPED";
	case CHR_TEST_HOW_ENDED_ERROR:
		return "ERROR";
	case CHR_TEST_HOW_ENDED_NORMAL:
		return "NORMAL";
	default:
		return "Undefined";
	};
}


/***************************************************************
 *
 * Print the units in which throughput is expressed in the
 * function.
 *
 * Parameters: throughputUnits - one of MB,Mb,KB,Kb,Gb,kb as
 *             defined in chrapi.h
 *
 ***************************************************************/
//static char*
//show_throughput_units_string(
//	CHR_THROUGHPUT_UNITS throughputUnits)
//{

/***********************************************************************
*                                                                      *
*######################################################################*
*#      IxChariot Single Endpoint Pair - Multiple Streams Test        #*
*######################################################################*
* IxChariot Config File Location:                                      *
* Test Result Location:                                                *
* Test Config .txt Location:                                           *
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

#include "chrapi.h" // IxChariot Header/API
#include "Dev_C_Config_IxChariot_Single_Pair_Multiple_Streams.c" // Test Config file

/* Local function to print information about errors */
static void show_error(
    CHR_HANDLE handle,
    CHR_API_RC code,
    CHR_STRING where);


/* ************************ MAIN PROGRAM ************************ */
void main(){

    CHR_TEST_HANDLE test;
    CHR_PAIR_HANDLE pair;

    char      errorInfo[CHR_MAX_ERROR_INFO];
    CHR_LENGTH errorLen;

        CHR_COUNT index;

    CHR_BOOLEAN isStopped;
    CHR_COUNT   timer = 0;

    CHR_API_RC rc;


    /* ------------------------ Step 1 ------------------------
     * Initialize the Chariot API
     */
    rc = CHR_api_initialize(CHR_DETAIL_LEVEL_ALL, errorInfo,
                            CHR_MAX_ERROR_INFO, &errorLen);
    if (rc != CHR_OK) {

        /*
         * Because initialization failed, we can't
         * ask the API for the message for this
         * return code so we can't call our
         * show_error() function. Instead, we'll
         * print what we do know before exiting.
         */
        printf("Initialization failed: rc = %d\n", rc);
        printf("Extended error info:\n%s\n", errorInfo);
        exit(255);
    }


    /* ------------------------ Step 2 ------------------------
     * Create new test
     */
    printf("Create the test...\n");
    rc = CHR_test_new(&test);
    if (rc != CHR_OK) {
       show_error((CHR_HANDLE)NULL, rc, "test_new");
    }

    /* Set the test filename for saving later */
    rc = CHR_test_set_filename(test, testFile, strlen(testFile));
    if (rc != CHR_OK) {
        show_error(test, rc, "test_set_filename");
    }

    // ***** CODE RESULTS .txt GOING TO &data_path *****


    /* ------------------------ Step 3 ------------------------
     * Define and create the endpoint pairs
     */
    for (index = 0; index < pairCount; index++) {

        char comment[CHR_MAX_PAIR_COMMENT];

        /* Create a pair */
        printf("Creating the endpoint pair...\n");
        rc = CHR_pair_new(&pair);
        if (rc != CHR_OK) {
            show_error((CHR_HANDLE)NULL, rc, "pair_new");
        }

        /* Set the pair attributes from our lists */
        printf("Set pair attributes...\n");
        sprintf(comment, "Pair %d", index+1);
        rc = CHR_pair_set_comment(pair,
                                  comment,
                           strlen(comment));
        if (rc != CHR_OK) {
            show_error(pair, rc, "pair_set_comment");
        }
        rc = CHR_pair_set_e1_addr(pair,
                                  e1Addrs[index],
                           strlen(e1Addrs[index]));
        if (rc != CHR_OK) {
            show_error(pair, rc, "pair_set_e1_addr");
        }
        rc = CHR_pair_set_e2_addr(pair,
                                  e2Addrs[index],
                           strlen(e2Addrs[index]));
        if (rc != CHR_OK) {
            show_error(pair, rc, "pair_set_e2_addr");
        }
        rc = CHR_pair_set_protocol(pair, protocols[0]);     // Indicate pair protocol from config
        if (rc != CHR_OK) {
            show_error(pair, rc, "pair_set_protocol");
        }
        rc = CHR_pair_use_script_filename(pair,             // Script from config
                                          script,
                                   strlen(script);
        if (rc != CHR_OK) {
            show_error(pair, rc, "pair_use_script_filename");
        }

        /* Add the pair to the test */
        rc = CHR_test_add_pair(test, pair);
        if (rc != CHR_OK) {
            show_error(test, rc, "test_add_pair");
        }
    }


    /* ------------------------ Step 4 ------------------------
     * RUN THE TEST!
     */

    /* Step 4.1 ITERATION IMPLEMENTATION */
    for (int iteration = 1; iteration <= test_iterations; iteration++) {

        printf("Beginning iteration (%d)...\n", iteration);
        printf("\n");

        /* Step 4.2 Run */
        printf("Running the test...\n");
        rc = CHR_test_start(test);
        if (rc != CHR_OK) {
            show_error(test, rc, "start_ test");
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
        isStopped = CHR_FALSE;
        timer = 0;
        while (!isStopped && timer < maxWait) {
            rc = CHR_test_query_stop(test, timeout);
            if (rc == CHR_OK) {
                isStopped = CHR_TRUE;
            }
            else if (rc == CHR_TIMED_OUT) {
                timer += timeout;
                printf("Waiting for test to stop... (%d)\n", timer);
            }
            else {
                show_error(test, rc, "test_query_stop");
            }
        }
        if (!isStopped) {
            show_error(test, CHR_TIMED_OUT, "test_query_stop");
        }
    }


    /* ------------------------ Step 5 ------------------------
     * Save the test
     */
    printf("Saving the test..\n");
    rc = CHR_test_save(test);
    if (rc != CHR_OK) {
        show_error(test, rc, "test_save");
    }


    /* ------------------------ END ------------------------*/
    exit(EXIT_SUCCESS);

}


/***************************************************************
 *
 * Print information about an error and exit. If there is
 * extended error information, log that before exiting.
 *
 * Parameters: handle - what object had the error
 *             code   - the Chariot API return code
 *             where  - what function call failed
 ***************************************************************/

static void
show_error(
    CHR_HANDLE handle,
    CHR_API_RC code,
    CHR_STRING where)
{
    char       msg[CHR_MAX_RETURN_MSG];
    CHR_LENGTH msgLen;

    char       errorInfo[CHR_MAX_ERROR_INFO];
    CHR_LENGTH errorLen;

    FILE  *fd;
    time_t currentTime;
    char  *timestamp;

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

        /* Open the log file */
        fd = fopen(logFile, "a+");

        /* Get the timestamp */
        currentTime = time(NULL);
        timestamp = ctime(&currentTime);

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
            if (fd != NULL) {

                /* Log it if the file was opened successfully */
                fprintf(fd, "%s %s failed\n",
                        timestamp, where );
                fprintf(fd, "%s %s\n",
                        timestamp, errorInfo);
                fclose(fd);
            }
        }
    }

    /*
     * We've told everything we know, so now just exit.
     */
    exit(EXIT_FAILURE);
}

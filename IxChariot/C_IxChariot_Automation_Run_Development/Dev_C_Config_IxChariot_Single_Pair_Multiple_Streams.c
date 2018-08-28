/************************************************************************
 *                                                                      *
 * C Config file for Chariot Single Endpoint Pair Multiple Streams Test *
 * Author: Nathan Hu                                                    *
 *                                                                      *
 ***********************************************************************/


#include "chrapi.h"


/* ****** AP NAME ****** */
static CHR_STRING ap_name		= "Device Under Test Name";


/* ****** Test and Results Reporting ****** */
//static CHR_STRING data_path = "C:/Users/attlabs/Desktop/Data_Log"; // Path to store test results in ".txt" format
static CHR_STRING data_path		= "C:/Users/Nathan/Desktop/Automation/Testing";

static char test_description[]	= "IxChariot_SingleMultiStrmPairTest";

/* ****** TEST SETUP ****** */
static CHR_STRING script	=
"C:\\Program Files (x86)\\Ixia\\IxChariot\\Scripts\\High_Performance_Throughput.scr";
static CHR_STRING testfile	=
"c:/Program Files/Ixia/IxChariot/Test_Configs/[subst SinglePairTest_[subst $timestamp]_[subst $x]_[subst $iteration]].tst";

static int test_iterations = 5;					// Specify number of iterations the test will run
												// The variables below are for reporting
static char wifi_channel[]		= "149";		// Wi-Fi channel of DUT
static int int_channel			= 157;

static char location[]			= "In Room";	// Location of client endpoints
//char location					= "L19";
//char location					= "L10";
//char location					= "L24";

static char mesh[]				= "No Mesh";
//char mesh						= "Yes";


/* ****** Test Timing ****** */
static CHR_COUNT max_wait			= 120;			// 2 minutes in seconds
static CHR_COUNT timeout			= 5;			// Periodic 5 second check
static CHR_COUNT fix_run_time		= 1;			// 1 - Run test at fixed duration, 0 - full run

static CHR_COUNT test_run_duration	= 75;
static CHR_COUNT between_pair_delay = 30;

static CHR_COUNT iteration_delay	= 15; 			// Delay between iterations (sec)


/* ****** Clients/Endpoints ****** */
// Insert device IP address in ""
static char IxChariot_Console[] = "192.168.1.60";	// Endpoint 1

static char QTN_RDK[]			= "192.168.1.60";
static char MacBook_Pro[]		= "192.168.1.60";
static char Galaxy_S5[]			= "192.168.1.60";
static char iPad_Air[]			= "192.168.1.60";


/* ****** Endpoint Pair(s) ****** */
static CHR_PROTOCOL protocols[] = {					// Protocol API
    CHR_PROTOCOL_TCP,
    CHR_PROTOCOL_RTP,
    CHR_PROTOCOL_UDP
};

static CHR_COUNT pairCount		= 4;
static CHR_COUNT streamNum[]	= {3, 5, 7};		// Desired number of stream pairs for each test

static CHR_STRING e1Addrs[] = {						// Need an equal amount of endpoints in each array
    IxChariot_Console,
    IxChariot_Console,
    IxChariot_Console,
    IxChariot_Console
};

static CHR_STRING e2Addrs[] = {
    QTN_RDK,
    MacBook_Pro,
    Galaxy_S5,
    iPad_Air
};

//static CHR_STRING e1 = "192.168.1.x";
//static CHR_STRING e2 = "192.168.1.x";

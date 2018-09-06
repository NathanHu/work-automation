/************************************************************************
 *                                                                      *
 * C Config file for Chariot Single Endpoint Pair Multiple Streams Test *
 * Author: Nathan Hu                                                    *
 *                                                                      *
 ***********************************************************************/

#include "chrapi.h"

/* ****** Previously Tested Builds ******
 * Use to log old tests if needed

 */

/* ****** Test Information ****** */
static char test_type[]			= "Single Endpoint Pair Multiple Streams Test";
static char test_equipment[]	= "Ixia Chariot";
static char ap_name[]			= "Device Under Test Name";
static char text[]				= "TCP P2P Pair Multiple Stream";


/* ****** Test and Results Reporting ****** */
//static CHR_STRING data_path = "C:/Users/attlabs/Desktop/Data_Log"; // Path to store test results in ".txt" format
static CHR_STRING data_path		= "C:/Users/attlabs/Desktop/Automation/Testing/Test Runs";

static char test_description[]	= "IxChariot_SingleMultiStrmPairTest";


/* ****** TEST SETUP ****** */
static CHR_STRING script	=
"C:/Program Files (x86)/Ixia/IxChariot/Scripts/High_Performance_Throughput.scr";

static int test_iterations		= 1;			// Specify number of iterations the test will run
static char wifi_channel[]		= "149";		// Wi-Fi channel of DUT
static int int_channel			= 157;

static char location[]			= "In Room";	// Location of client endpoints
//char location					= "L19";
//char location					= "L10";
//char location					= "L24";

static char mesh[]				= "No Mesh";
//char mesh						= "Yes";


/* ****** Test Timing ****** */
static CHR_COUNT maxWait			= 120;			// 2 minutes in seconds
static CHR_COUNT timeout			= 5;			// Periodic 5 second check
static CHR_COUNT fix_run_time		= 1;			// 1 - Run test at fixed duration, 0 - full run

static CHR_COUNT test_run_duration	= 20;
static CHR_COUNT between_pair_delay = 30;
static CHR_COUNT iteration_delay	= 15; 			// Delay between iterations (sec)


/* ****** Clients/Endpoints ******
 *Insert device IP address in ""
 */
/* Console endpoint 1 */
static CHR_STRING endpoint1			= "IxChariot Console";
static char IxChariot_Console[]		= "192.168.1.60";

/* Client endpoint(s) 2 */
static char QTN_RDK[]				= "192.168.1.60";
static char MacBook_Pro[]			= "192.168.1.64";
static char Galaxy_S5[]				= "192.168.1.60";
static char iPad_Air[]				= "192.168.1.60";

/* ****** Endpoint Pair(s) ****** */
static CHR_PROTOCOL protocols[] = {					// Protocol API
	CHR_PROTOCOL_TCP,
	CHR_PROTOCOL_RTP,
	CHR_PROTOCOL_UDP
};

/* ------------------------------ TEST ------------------------------ */
static CHR_COUNT pairCount	= 1;
static int client_count		= 1;

static int streamNum[] = { 3, 7 };		// Desired number of stream pairs for each test

/* Client abreviation for reporting
 * Keep in same order as e2Addrs
 */
static CHR_STRING client_id[] = {

	"MBP"

};

static CHR_STRING e1Addrs[] = {						// Need an equal amount of endpoints in each array

	IxChariot_Console

};

static CHR_STRING e2Addrs[] = {

	MacBook_Pro

};
/* ------------------------------ TEST ------------------------------ */

//static CHR_COUNT pairCount = 4;
//static CHR_COUNT streamNum[] = { 3, 5, 7 };		// Desired number of stream pairs for each test
//
///* Client abreviation for reporting
// * Keep in same order as e2Addrs
// */
//static CHR_STRING client_id[] = {
//
//	"RDK",
//	"MBP",
//	"GS5",
//	"iPad"
//
//};
//
//static CHR_STRING e1Addrs[] = {						// Need an equal amount of endpoints in each array
//
//	IxChariot_Console,
//    IxChariot_Console,
//    IxChariot_Console,
//    IxChariot_Console
//
//};
//
//static CHR_STRING e2Addrs[] = {
//
//	QTN_RDK,
//    MacBook_Pro,
//    Galaxy_S5,
//    iPad_Air
//
//};

//static CHR_STRING e1 = "192.168.1.x";
//static CHR_STRING e2 = "192.168.1.x";


/* ****** Utiliy/Formatting ****** */
static char divider[]	= "-----------------------------------------------------------";
static char special[]	= "************************";
static char space[]		= "";
static char *units[3]	= { "kbps", "Mbps", "Gbps" };
static char row_id[]	= "#Streams-ID-Loc-Ch";

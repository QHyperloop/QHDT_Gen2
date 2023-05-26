#include <stdbool.h>
#include <stdint.h>

//countdown (RTL to launch)
unsigned long LAUNCH_TIME_THRESH = 20000;//(from launch to brake)
unsigned long COAST_TIME_THRESH = 5000; // (from coast to brake/fault)
//CRAWL_TIME_THRESH (from crawl to brake)


// preset calculated values for when the pod should engage certain aspects
// must be flashed each time to change
// distances are in in meters and speeds are in m/s
const int BRAKE_DIST_THRESH = 800;
const int END_DIST_THRESH = 1000;
// speed that the pod coasts at (~10km/h)
const float COAST_SPEED_THRESH = 2.8;
// Variable value to set the maximum speed of the pod
// max speed that the pod attempts to achieve (~200km/h)
const float LAUNCH_SPEED_THRESH = 200;

// Packet data
const uint8_t TEAM_ID = 1;
const int PACKET_SIZE = 34;

// adjust these values for pressure calibration
const float PRESSURE_SLOPE = 0.126;
const float BIAS = -0.5 * 1024 / 5;
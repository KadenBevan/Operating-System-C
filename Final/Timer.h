// Program Information ////////////////////////////////////////////////////////
/**
* @file Timer.h
*
* @brief Header file for micro-second precision timer
*
* @author Michael Leverington
*
* @details Specifies all member methods of the Timer
*
* @version 2.00 (13 January 2017)
*          1.00 (11 September 2015)
*
* @Note None
*/

// Precompiler directives /////////////////////////////////////////////////////

#ifndef TIMER_H
#define TIMER_H

// Header files ///////////////////////////////////////////////////////////////

#include <sys/time.h>
#include <string.h>
#include <math.h>

// Global Constants  //////////////////////////////////////////////////////////

enum TIMER_CTRL_CODES { ZERO_TIMER, LAP_TIMER, STOP_TIMER };


// Function Prototyp  /////////////////////////////////////////////////////////

void runTimer(int milliSeconds);
double accessTimer(int controlCode, char *timeStr);
double processTime(double startSec, double endSec,
	double startUSec, double endUSec, char *timeStr);
void timeToString(int secTime, int uSecTime, char *timeStr);

#endif // ifndef TIMER_H


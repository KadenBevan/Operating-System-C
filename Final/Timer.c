// Program Information ////////////////////////////////////////////////////////
/**
* @file Timer.c
*
* @brief Implementation file for using a timer with micro-second precision
*
* @author Michael Leverington
*
* @details Implements member methods for timing
*
* @version 3.00 (02 February 2017) Update to simulator timer
2.00 (13 January 2017) Update to C language
*          1.00 (11 September 2015)
*
* @Note Requires Timer.h.
*
*/

// Precompiler directives /////////////////////////////////////////////////////

#ifndef SIMTIMER_C
#define SIMTIMER_C

// Header files ///////////////////////////////////////////////////////////////

#include "Timer.h"

// Constants  /////////////////////////////////////////////////////////////////

const char RADIX_POINT = '.';


void runTimer(int milliSeconds)
{
	struct timeval startTime, endTime;
	int startSec, startUSec, endSec, endUSec;
	int uSecDiff, mSecDiff, secDiff, timeDiff;

	gettimeofday(&startTime, NULL);

	startSec = startTime.tv_sec;
	startUSec = startTime.tv_usec;

	timeDiff = 0;

	while (timeDiff < milliSeconds)
	{
		gettimeofday(&endTime, NULL);

		endSec = endTime.tv_sec;
		endUSec = endTime.tv_usec;
		uSecDiff = endUSec - startUSec;

		if (uSecDiff < 0)
		{
			uSecDiff = uSecDiff + 1000000;

			endSec = endSec - 1;
		}

		mSecDiff = uSecDiff / 1000;
		secDiff = (endSec - startSec) * 1000;
		timeDiff = secDiff + mSecDiff;
	}
}

double accessTimer(int controlCode, char *timeStr)
{
	static int running = 0;
	static int startSec = 0, endSec = 0, startUSec = 0, endUSec = 0;
	static int lapSec = 0, lapUSec = 0;
	struct timeval startData, lapData, endData;
	double fpTime = 0.0;
	
	switch (controlCode)
	{
	case ZERO_TIMER:
		gettimeofday(&startData, NULL);
		running = 1;

		startSec = startData.tv_sec;
		startUSec = startData.tv_usec;

		fpTime = 0.000000000;
		lapSec = 0.000000000;
		lapUSec = 0.000000000;

		timeToString(lapSec, lapUSec, timeStr);
		break;

	case LAP_TIMER:
		if (running == 1)
		{
			gettimeofday(&lapData, NULL);

			lapSec = lapData.tv_sec;
			lapUSec = lapData.tv_usec;

			fpTime = processTime(startSec, lapSec,
				startUSec, lapUSec, timeStr);
		}

		else
		{
			fpTime = 0.000000000;
		}
		break;

	case STOP_TIMER:
		if (running == 1)
		{
			gettimeofday(&endData, NULL);
			running = 0;

			endSec = endData.tv_sec;
			endUSec = endData.tv_usec;

			fpTime = processTime(startSec, endSec,
				startUSec, endUSec, timeStr);
		}

		// assume timer not running
		else
		{
			fpTime = 0.000000000;
		}
		break;
	}

	return fpTime;
}

double processTime(double startSec, double endSec,
	double startUSec, double endUSec, char *timeStr)
{
	double secDiff = endSec - startSec;
	double uSecDiff = endUSec - startUSec;
	double fpTime;

	fpTime = (double)secDiff + (double)uSecDiff / 1000000;

	if (uSecDiff < 0)
	{
		uSecDiff = uSecDiff + 1000000;
		secDiff = secDiff - 1;
	}

	timeToString(secDiff, uSecDiff, timeStr);

	return fpTime;
}


/* This is a bit of a drawn-out function, but it is written
to force the time result to always be in the form x.xxxxxxx
when printed as a string; this will not always be the case
if the time is presented as a floating point number
*/
void timeToString(int secTime, int uSecTime, char *timeStr)
{
	int low, high, index = 0;
	char temp;

	while (uSecTime > 0)
	{
		timeStr[index] = (char)(uSecTime % 10 + '0');
		uSecTime /= 10;

		index++;

	}

	while (index < 6)
	{
		timeStr[index] = '0';

		index++;
	}

	timeStr[index] = RADIX_POINT;

	index++;

	if (secTime < 10) // single digit in display
	{
		timeStr[index] = (char)(secTime % 10 + '0');

		index++;

		timeStr[index] = ' ';

		index++;

		secTime = 0;
	}
	/*
	if( secTime == 0 )
	{
	timeStr[ index ] = '0';

	index++;
	}
	*/
	while (secTime > 0)
	{
		timeStr[index] = (char)(secTime % 10 + '0');

		secTime /= 10;

		index++;
	}

	timeStr[index] = '\0';

	low = 0; high = index - 1;

	while (low < high)
	{
		temp = timeStr[low];
		timeStr[low] = timeStr[high];
		timeStr[high] = temp;

		low++; high--;
	}
}

#endif // ifndef TIMER_C






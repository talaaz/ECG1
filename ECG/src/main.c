/*
 ============================================================================
 Name        : ECG.c
 Author      : Tala
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#include "sensor.h"
#include "filters.h"
#include "filterTester.h"
#include "qrs.h"
#include "qrsTester.h"
#include "output.h"
#include "outputTester.h"
#include "performanceTest.h"

#define ARRAYSIZE 33


int main(void) {
	//testFilters();
	//WarningTest();

	//PulseTest();

	//storeArrayTest();
	//searchPeakTest();
	//findAvgTest();
	//peakDetectionTest();
	//calculateRRTest();

	//PerformancePeak();
	//PerformanceFilters();
	//PerformanceMain();

	clock_t start;
	double time=0.00000;

	FILE * ecgFile = openfile("ECG.txt");
	FILE * ecgFileNext = openfile("ECG.txt");
	getNextData(ecgFileNext);

	//x is the input array, y is the output array
	int xInput[LOW_PASS_INPUT_SIZE] = { 0 };
	int xNext[LOW_PASS_INPUT_SIZE] = { 0 };
	int y[ARRAYSIZE] = { 0 };

	int yLowPass[LOW_PASS_OUTPUT_SIZE] = { 0 };
	int yHighPass[HIGH_PASS_OUTPUT_SIZE] = { 0 };
	int yDeriv[DERIVATIVE_OUTPUT_SIZE] = { 0 };
	int ySquare[SQUARING_OUTPUT_SIZE] = { 0 };

	int yLowPassNext[LOW_PASS_OUTPUT_SIZE] = { 0 };
	int yHighPassNext[HIGH_PASS_OUTPUT_SIZE] = { 0 };
	int yDerivNext[DERIVATIVE_OUTPUT_SIZE] = { 0 };
	int ySquareNext[SQUARING_OUTPUT_SIZE] = { 0 };
	//Counters
	int inputCounter=0;
	int next=0;
	int new=0;

	extern int Rposition,peak;

	//Peakdetection algoritmen kører så længe der er input
			while (!feof(ecgFile)) {
					start = clock();

					xInput[inputCounter% LOW_PASS_INPUT_SIZE]= getNextData(ecgFile);
					yLowPass[inputCounter % LOW_PASS_OUTPUT_SIZE] = lowPassFilter(yLowPass, xInput, inputCounter);
					yHighPass[inputCounter % HIGH_PASS_OUTPUT_SIZE] = highPassFilter(yHighPass, yLowPass, inputCounter);
					yDeriv[inputCounter % DERIVATIVE_OUTPUT_SIZE] = derivativeFilter(yHighPass, inputCounter);
					ySquare[inputCounter % SQUARING_OUTPUT_SIZE] = squaringFilter(yDeriv, inputCounter);
					y[inputCounter%32] = mwiFilter(ySquare, inputCounter);


					xNext[inputCounter% LOW_PASS_INPUT_SIZE]= getNextData(ecgFileNext);
					yLowPassNext[inputCounter % LOW_PASS_OUTPUT_SIZE] = lowPassFilter(yLowPassNext, xNext, inputCounter);
					yHighPassNext[inputCounter % HIGH_PASS_OUTPUT_SIZE] = highPassFilter(yHighPassNext, yLowPassNext, inputCounter);
					yDerivNext[inputCounter % DERIVATIVE_OUTPUT_SIZE] = derivativeFilter(yHighPassNext, inputCounter);
					ySquareNext[inputCounter % SQUARING_OUTPUT_SIZE] = squaringFilter(yDerivNext, inputCounter);
					next = mwiFilter(ySquareNext, inputCounter);

					new=Rposition;
					peakDetection(y, inputCounter, next);


					inputCounter++;

					time=(((double) (start)));
					if (new!=Rposition){
					outputResults(inputCounter,time/CLOCKS_PER_SEC,peak);
				}
			}
			fclose(ecgFile);
			fclose(ecgFileNext);

	return EXIT_SUCCESS;
}



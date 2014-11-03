/* Timing code inspired by http://www.guyrutenberg.com/2007/09/22/profiling-code-using-clock_gettime/
 * ECDF formula taken from Statistics Textbook
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decide.h"

#define NUM_TESTS 100000
#define PLOT_POINTS 1000

void allocateGlobals(void);
void populateGlobals(void);
struct timespec diff(struct timespec start, struct timespec end);
void generate_ECDF(struct timespec* dataSet, unsigned int dataSet_size, unsigned int numPoints);

int main()
{
    allocateGlobals();
    populateGlobals();

    // Array of timespec structs to hold output data
    struct timespec* timingData = malloc(NUM_TESTS * sizeof(struct timespec));
    
	struct timespec start, stop;
    int i;

    for(i=0; i < NUM_TESTS; i++) {
*       clock_gettime(CLOCK_MONOTONIC, &start);
        DECIDE();
        clock_gettime(CLOCK_MONOTONIC, &stop);
        timingData[i] = diff(start,stop);

//        timingData[i].tv_nsec = (timingData[i-1].tv_nsec*59+439)%7919;  // Random data for testing
    }    

    generate_ECDF(timingData, NUM_TESTS, PLOT_POINTS);   
    free(timingData);
    
	return 0;
}


struct timespec diff(struct timespec start, struct timespec end)
{
	struct timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}


void generate_ECDF(struct timespec* dataSet, unsigned int dataSet_size, unsigned int numPoints) {
    
    unsigned int i, j, sum;
    long x_coord;
    float y_coord;
    
    // Find maximum value in dataSet (only dealing with nanoseconds)
    long max = dataSet[0].tv_nsec; 
    for(i=1; i < dataSet_size; i++) {
       if(dataSet[i].tv_nsec > max) 
            max = dataSet[i].tv_nsec;
    }
    
    FILE *plotData = fopen("plotData", "w+");
    
    // generate ECDF
    for(i=0; i < numPoints; i++) {
        // Calculate x_coord
        x_coord = (max/numPoints)*i;
        
        // Calculate y_coord
        for(j=0, sum=0; j < dataSet_size; j++) {
            if(dataSet[j].tv_nsec <= x_coord)
                sum++;
        }
        y_coord = ((float)sum/numPoints);
        
        fprintf(plotData,"%ld %f\n",x_coord,y_coord);
    }
    fclose(plotData);
    /*
    FILE *pipe = popen("gnuplot -persist", "w");
    fprintf(pipe, "set title 'Empirical Cumulative Distribution Function Plot' font 'sans,18';");
    fprintf(pipe, "set xlabel 'Time (nS)' font 'sans,14';");
    fprintf(pipe, "set ylabel 'ECFD \%%' font 'sans,14';");
    fprintf(pipe, "set grid; unset key;");
    fprintf(pipe, "plot 'plotData' with lines;");
    pclose(pipe);
    */
    
}

void populateGlobals() {

    // Data to initialize global variables with
    double x_init[100] =  {0};
    
    double y_init[100] =  {10};
    
    memcpy(X, x_init, 100);
    memcpy(Y, y_init, 100);
    

}

void fail(char* msg) {
    printf("FAIL: %s",msg);
    exit(1);
}

void allocateGlobals() {
    int i;

    // Allocate X coordinates of data points
    X = (double*) malloc (100 * sizeof (double));
    if (!X) fail("X Alloc");
    
    // Allocate Y coordinates of data points
    Y = (double*) malloc (100 * sizeof (double));
    if (!Y) fail("Y Alloc");

    // Allocate Conditions Met Vector
    CMV = (boolean*) malloc (15 * sizeof (boolean));
    if (!CMV) fail("CMV Alloc");

    // Allocate Final Unlocking Vector
    FUV = (boolean*) malloc (15 * sizeof (boolean));
    if (!FUV) fail("FUV Alloc");

    // Allocate Logical Connector Matrix    
    LCM = (CONNECTORS **) malloc (15 * sizeof (CONNECTORS*));
    if (!LCM) fail("LCM Outer Alloc");
    
    for (i=0; i<15; i++) {
        LCM[i] = (CONNECTORS *) malloc (15 * sizeof (CONNECTORS));
        if (!LCM[i]) fail("LCM Inner Alloc");
    }
    
    // Allocate Preliminary Unlocking Matrix
    PUM = (boolean **) malloc (15 * sizeof (boolean*));
    if (!PUM) fail("PUM Outer Alloc");
    
    for (i=0; i<15; i++) {
        PUM[i] = (boolean *) malloc (15 * sizeof (boolean));
        if (!PUM[i]) fail("PUM Inner Alloc");
    }
}


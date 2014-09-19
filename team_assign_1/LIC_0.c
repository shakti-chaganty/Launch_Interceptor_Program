#include"stdio.h"
#include"math.h"

//function to find the distance between two consecutive data points.

const double LENGTH1 = 14;//  These will be provided before the function is called.
int NUMPOINTS = 6;//
int X[100] = { 1, 2, 3, 5, 11, 21, 16, 15 };
int Y[100] = { 4, 5, 7, 9, 1, 17, 29, 19 };

void LIC_0()
{
	int CMV[1] = { 0 };
	int i = 0;
	int flag = 0;
	double distance;

	//loop through i data points.

	while (i<NUMPOINTS && flag ==0)
	{
		// Get the coordinates for the two consecutive points

		double x1 = X[i];
		double x2 = X[i + 1];
		double y1 = Y[i];
		double y2 = Y[i + 1];

		distance = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
		// Calculate the distance between each set of consecutive data points.
		//if distance exceeds LENGTH1 CMV[0] is set to true or 1 and looping is terminated.

		if (distance >  LENGTH1)
		{
			flag = 1;
			CMV[0] = 1;
			break;
		}
		else
		{
			CMV[0] = 1;
		}
		i++;
	}
	
}// end of LOC_0


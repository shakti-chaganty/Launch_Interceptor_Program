/* CS 5785/6785
 * Team Assignment #1
 *
 * Brent Mellor
 * Shakti Chaganty 
 * Eric Hair
 *
 * decide.c
 *    Part of a hypothetical anti-ballistic missile system.
 *    Determines whether an interceptor should be launched 
 *    based upon input radar tracking information.
 */

//#include "test.h"
#include "decide.h"
//#include <stdio.h>
#define TRUE 1
#define FALSE 0
const int really_big = 2e9;

// Takes x and y coordinates for two Cartesian points and returns the distance
// between the two points
double get_distance(double x1, double x2, double y1, double y2)
{
    return sqrt(pow(x1-x2,2)+pow(y1-y2,2));
}

// Takes the distance between three points and returns the angle <213
double get_angle(double l12, double l13, double l23)
{
    double result = (pow(l12,2)+pow(l13,2)-pow(l23,2))/(2*l12*l13);
    if (DOUBLECOMPARE(result,1)==EQ||DOUBLECOMPARE(result,1)==GT)
        result = 1;
    else if (DOUBLECOMPARE(result,-1)==EQ||DOUBLECOMPARE(result,-1)==LT)
        result = -1;
    return acos(result);
}

// Takes the x and y coordinates of two Cartesian points and returns the slope
double get_slope(double x1, double x2, double y1, double y2)
{
    if (DOUBLECOMPARE(x1,x2)==EQ) {
        if (DOUBLECOMPARE(y2-y1,0)==GT)
            return really_big;
        else
            return -really_big;
    }
    return (y2-y1)/(x2-x1);
}

// Takes slope and y intercept of the line (y = mx + c) and returns distance of point from the line.
double pt_line_distance(double m,double c,double x1, double y1)
{
    double z;
    z = (m*x1 - y1 + c)/sqrt((pow(m,2)+1));
    if (DOUBLECOMPARE(z,0)==GT||DOUBLECOMPARE(z,0)==EQ) 
        return z;
    else
        return -1*z;
}

double get_quadrant(double x1, double y1)//Returns quadrant based on priority
    //ordering when a point is in conflict
{
    //if (x1>=0&&y1>=0)
    if(((DOUBLECOMPARE(x1,0)==GT)||(DOUBLECOMPARE(x1,0)==EQ))&&((DOUBLECOMPARE(y1,0)==GT)||(DOUBLECOMPARE(y1,0)==EQ)))
        return 1;
    //if (x1<0&&y1>=0)
    if((DOUBLECOMPARE(x1,0)==LT)&&((DOUBLECOMPARE(y1,0)==GT)||(DOUBLECOMPARE(y1,0)==EQ)))
        return 2;
    //if(x1<=0&&y1<0)
    if(((DOUBLECOMPARE(x1,0)==LT)||(DOUBLECOMPARE(x1,0)==EQ))&&(DOUBLECOMPARE(y1,0)==LT))
        return 3;
    else
        return 4;
} 

// Given three points, decides if they can be contained in a circle of radius.
// Returns true if they cannot be, false if they can.
boolean cannot_be_contained_in_circle(double x1, double y1, double x2,
        double y2, double x3, double y3, double radius)
{
    double l12 = get_distance(x1,x2,y1,y2);
    double l13 = get_distance(x1,x3,y1,y3);
    double l23 = get_distance(x2,x3,y2,y3);

    //To include a case where all the three points are the same.
    if((DOUBLECOMPARE(x1,x2)==EQ)&&(DOUBLECOMPARE(x2,x3)==EQ)&&
            (DOUBLECOMPARE(y1,y2)==EQ)&&(DOUBLECOMPARE(y2,y3)==EQ))
    {
        return FALSE; 
    }

    // Find the angle where these two lines intersect, if greater than 90
    // then the line made by p1 and p3 is the diameter of the circle
    double theta1 = get_angle(l12,l13,l23);
    if (DOUBLECOMPARE(theta1,PI/2)==GT||DOUBLECOMPARE(theta1,PI/2)==EQ) {
        // If this is greater than RADIUS1 then set the CMV and return
        if (DOUBLECOMPARE(l23/2,radius)==GT) {
            return TRUE;
        }
        else
            return FALSE;
    }
    // If the angle is <= 90, then check one more angle
    double theta2 = get_angle(l12,l23,l13);

    // If theta1 and theta2 are zero, then we just have a line, not a triangle
    if (DOUBLECOMPARE(theta1,0)==EQ&&DOUBLECOMPARE(theta2,0)==EQ) {
        // Find a side that is non-zero
        if (DOUBLECOMPARE(l12,0)!=EQ) {
            if (DOUBLECOMPARE(l12/2,radius)==GT)
                return TRUE;
            else
                return FALSE;
        }
        // We should only have to test one more side
        else {
            if (DOUBLECOMPARE(l13/2,radius)==GT)
                return TRUE;
            else
                return FALSE;
        }
    }

    if (DOUBLECOMPARE(theta2,PI/2)==GT||DOUBLECOMPARE(theta2,PI/2)==EQ) {
        // If this is greater than RADIUS1 then set the CMV and return
        if (DOUBLECOMPARE(l13/2,radius)==GT) {
            return TRUE;
        }
        else
            return FALSE;
    }
    // Check the last angle
    if (DOUBLECOMPARE(theta1+theta2,PI/2)==LT||
            DOUBLECOMPARE(theta1+theta2,PI/2)==EQ) {
        if (DOUBLECOMPARE(l12/2,radius)==GT) {
            return TRUE;
        }
        else
            return FALSE;
    }
    // Now calculate the center of the circle
    double m_a = get_slope(x1,x2,y1,y2);
    double m_b = get_slope(x2,x3,y2,y3);
    while (DOUBLECOMPARE(m_a,0)==EQ||isnan(m_a)||isnan(m_b)) {
        double temp_x = x1;
        double temp_y = y1;
        x1 = x2;
        y1 = y2;
        x2 = x3;
        y2 = y3;
        x3 = temp_x;
        y3 = temp_y;
        m_a = get_slope(x1,x2,y1,y2);
        m_b = get_slope(x2,x3,y2,y3);
    }
    double center_x = (m_a*m_b*(y1-y3)+m_b*(x1+x2)-m_a*(x2+x3))
        /(2*(m_b-m_a));
    double center_y = -1/m_a*(center_x-(x1+x2)/2)+(y1+y2)/2;
    // All three points lie on the circle, so calculate the distance from
    // here out
    double radius1 = get_distance(center_x,x1,center_y,y1);
    if (DOUBLECOMPARE(radius1,radius)==GT) {
        return TRUE;
    }
    else
        return FALSE;
}

// An experimental area calculation that doesn't use Heron's formula. It might
// solve the LIC_3,10, and 14 discrepancies
double get_area(double x1, double y1, double x2, double y2, double x3,
        double y3)
{
    return fabs((x1*(y2-y3)+x2*(y3-y1)+x3*(y1-y2))/2);
}

//******************************LIC_0()***********************************
void LIC_0() 
{
CMV[0]=FALSE;
    int i = 0;
    double distance;
    for (i=0;i<NUMPOINTS-1;i++)
    {
        // Get the coordinates for the two consecutive points
        double x1 = X[i];
        double x2 = X[i + 1];
        double y1 = Y[i];
        double y2 = Y[i + 1];
	distance = get_distance(x1,x2,y1,y2);
        if (DOUBLECOMPARE(distance, PARAMETERS.LENGTH1) == GT)
        {
            CMV[0] = TRUE;
            return;
        }       
    }//closes for loop
} // End function LIC_0()
/*
 * The LIC_1 function checks three consecutive data points (p1, p2, p3) to see
 * if all three can be contained in a circle of radius RADIUS1
 */
void LIC_1() {

    CMV[1] = FALSE;

    // Loop through X and Y
    for (int i=0;i<NUMPOINTS-2;++i) {

        // Get the coordinates for the three points
        double x1 = X[i];
        double x2 = X[i+1];
        double x3 = X[i+2];
        double y1 = Y[i];
        double y2 = Y[i+1];
        double y3 = Y[i+2];

        CMV[1] = cannot_be_contained_in_circle(x1,y1,x2,y2,x3,y3,
                PARAMETERS.RADIUS1);
        if (CMV[1] == TRUE)
            return;
    }
    
} // End function LIC_1()

//**************************LIC_2()********************************************

void LIC_2() 
{
    CMV[2] = FALSE;
    // Loop through X and Y
    int i;
    for (i=0;i<NUMPOINTS-2;++i) 
    {
        // Get the coordinates for the three points
        double x1 = X[i];
        double x2 = X[i+1];
        double x3 = X[i+2];
        double y1 = Y[i];
        double y2 = Y[i+1];
        double y3 = Y[i+2];
        double l12 = get_distance(x1,x2,y1,y2);
        double l13 = get_distance(x1,x3,y1,y3);
        double l23 = get_distance(x2,x3,y2,y3);
        double angle = get_angle(l12,l23,l13); // vertex of angle is Pt. 2 (angle opp to line 13)
//To include a case where any two points are the same as vertex. Check if x1==x2 && y1==y2 || x2==x3 && y2==y3
        if(((DOUBLECOMPARE(x1,x2)==EQ)&&(DOUBLECOMPARE(y1,y2)==EQ))||
                ((DOUBLECOMPARE(x2,x3)==EQ)&&(DOUBLECOMPARE(y2,y3)==EQ)))
        continue;// eliminates setting CMV true even whenever any two points are coincident and angle = nan

	// checks for angle < (PI − EPSILON)or angle > (PI + EPSILON)
        if((DOUBLECOMPARE(angle,(PI+PARAMETERS.EPSILON))==GT)||
                (DOUBLECOMPARE(angle,(PI-PARAMETERS.EPSILON))==LT))
        {     
	CMV[2]=TRUE; 
        }
    }//closes for
}//end of LIC_2()

//Funtion that checks if area formed by three consecutive data points is greater
//than AREA1 

void LIC_3()
{
    CMV[3] = FALSE;
    int i;
    // Loop through X and Y
    for (i=0;i<NUMPOINTS-2;++i) 
    {
        // Get the coordinates for the three points
        double x1 = X[i];
        double x2 = X[i+1];
        double x3 = X[i+2];
        double y1 = Y[i];
        double y2 = Y[i+1];
        double y3 = Y[i+2];
        double area = get_area(x1,y1,x2,y2,x3,y3);
        //double l12 = get_distance(x1,x2,y1,y2);
        //double l13 = get_distance(x1,x3,y1,y3);
        //double l23 = get_distance(x2,x3,y2,y3);
        //double hp = (l12+l13+l23)/2; // calculating half-perimeter
        //double result = hp*(hp-l12)*(hp-l13)*(hp-l23);
        //if (DOUBLECOMPARE(result,0)==LT)
        //    result = 0;
        //double area = sqrt(result);
        if (DOUBLECOMPARE(area,PARAMETERS.AREA1)==GT)
        {
            CMV[3] = TRUE;
            return;
        }
    }//closes for loop
}// end of LIC3 functions LIC3 function


//Finds set of Q_PTS consecutive data points that lie in more than QUADS
//quadrants.
void LIC_4()
{
    CMV[4] = FALSE;
    int i,j,q1,q2,q3,q4;

    for(i=0;i<NUMPOINTS;i++)//NUMPOINTS
    {
        q1 = 0;
        q2 = 0;
        q3 = 0;
        q4 = 0;
        // iterate over Q_PTS consecutive points and stop if you reach the last
        // NUMPOINT
        for(j=i;(j<i+PARAMETERS.Q_PTS)&&(i+PARAMETERS.Q_PTS<NUMPOINTS+1);j++)
        {
            double z = get_quadrant(X[j],Y[j]);
            if(z == 1)
                q1++;
            else if(z == 2)
                q2++;
            else if(z == 3)
                q3++;
            else
                q4++;           
        }//closes inner for loop for Q_PTS
        switch(PARAMETERS.QUADS)
        {
            //more than 1 Quad
            case 1:
                if(q1*q2>0||q1*q3>0||q1*q4>0||q2*q3>0||q2*q4>0||q3*q4>0) //atleast two out q1,q2,q3 and q4 need to be non-zero
                {
                    CMV[4]=TRUE;
                    break;
                }
                //more than 2 Quad
            case 2:
                if(q1*q2*q3!=0||q2*q3*q4!=0||q3*q4*q1!=0||q1*q2*q4!=0) //atleast three out q1,q2,q3 and q4 need to be non-zero
                {
                    CMV[4]=TRUE;
                    break;
                }
                //more than 3 Quad
            case 3:
                if((q1*q2*q3*q4)>0)
                {
                    CMV[4]=TRUE;
                    break;
                }
        }
    }//closes for loop
}// end of LIC4 function LIC4 function

//check for the condition (X[i+1]-X[i])<0
void LIC_5()
{
    CMV[5] = FALSE;
    // Loop through X and Y
    int i;
    for (i=0;i<NUMPOINTS-1;++i)  // Get the coordinates for two points 
    {
        if(DOUBLECOMPARE(X[i+1],X[i])==LT)
        {
            CMV[5]= TRUE;
            break;
        }
    }//closes for
}//end of LIC_5()

// *****************LIC_6************************ 
void LIC_6()
{
    CMV[6] = FALSE;
    if(NUMPOINTS <3) //return if NUMPOINTS <3
        return;
    int i,j;
    double m,c,d;
    for(i=0;(i+PARAMETERS.N_PTS-1< NUMPOINTS);i++)
    {
        for(j=i+1;j<i+PARAMETERS.N_PTS-1;j++)
        { 
            // if two points are coincident
            if((DOUBLECOMPARE(X[i],X[i+PARAMETERS.N_PTS-1])==EQ) &&
                    (DOUBLECOMPARE(Y[i],Y[i+PARAMETERS.N_PTS-1])==EQ))
            {
                d = get_distance(X[i],X[j],Y[i],Y[j]);
                if (DOUBLECOMPARE(d,PARAMETERS.DIST)==GT) 
                {
                    CMV[6] = TRUE;
                    return;
                }
            }//closes if statement which tests whether two points are coincident.
            else
            {
                m = get_slope(X[i],X[i+PARAMETERS.N_PTS-1],Y[i],Y[i+PARAMETERS.N_PTS-1]);
                c = Y[i]-m*X[i];
                d = pt_line_distance(m,c,X[j],Y[j]);
                if (DOUBLECOMPARE(d,PARAMETERS.DIST)==GT) 
                {
                    CMV[6] = TRUE;
                    return;
                }
            }//closes else
        }//closes inner for loop j
    }//closes outer for loop i
}//end of LIC_6()


 //*****************LIC7()*********************

void LIC_7()
{
CMV[7]=FALSE;
if(NUMPOINTS <3) //return if NUMPOINTS <3
return;
int i;
float d;
for(i=0;(i+PARAMETERS.K_PTS+1< NUMPOINTS);i++)
{
d= get_distance(X[i],X[i+PARAMETERS.K_PTS+1],Y[i],Y[i+PARAMETERS.K_PTS+1]);
if (DOUBLECOMPARE(d,PARAMETERS.LENGTH1)==GT)
{
CMV[7]=TRUE;
return;
} 
}
}//end of LIC_7()

// Checks to see if three points can be contained inside a circle of radius
// RADIUS1. The points that are checked are separated by A_PTS and B_PTS.
void LIC_8()
{
    CMV[8] = FALSE;
    // If NUMPOINTS is less than 5, then false
    if (NUMPOINTS<5) {
        CMV[8] = FALSE;
        return;
    }

    int max_point = NUMPOINTS-PARAMETERS.A_PTS-PARAMETERS.B_PTS-2;
    for (int i=0;i<max_point;++i) {

        int index1 = i+PARAMETERS.A_PTS+1;
        int index2 = index1+PARAMETERS.B_PTS+1;
        double x1 = X[i];
        double x2 = X[index1];
        double x3 = X[index2];
        double y1 = Y[i];
        double y2 = Y[index1];
        double y3 = Y[index2];
        CMV[8] = cannot_be_contained_in_circle(x1,y1,x2,y2,x3,y3,
                PARAMETERS.RADIUS1);
        if (CMV[8] == TRUE)
            return;
    }
    return;
}
//********************LIC_9()**********************************
void LIC_9() 
{
CMV[9]=FALSE;
if(NUMPOINTS < 5)
return;
int i;
for(i=0;(i+PARAMETERS.C_PTS+PARAMETERS.D_PTS+1+1< NUMPOINTS);i++)
{
double x1 = X[i];
double x2 = X[i+PARAMETERS.C_PTS+1];
double x3 = X[i+PARAMETERS.C_PTS+1+PARAMETERS.D_PTS+1];
double y1 = Y[i];
double y2 = Y[i+PARAMETERS.C_PTS+1];
double y3 = Y[i+PARAMETERS.C_PTS+1+PARAMETERS.D_PTS+1];
double l12 = get_distance(x1,x2,y1,y2);
double l13 = get_distance(x1,x3,y1,y3);
double l23 = get_distance(x2,x3,y2,y3);	
double angle = get_angle(l12,l23,l13);//vertex of angle is Pt. 2 (angle opp to line 13)
//To include a case where any two points are the same as vertex. Check if x1==x2 && y1==y2  || x2==x3 && y2==y3
        if(((DOUBLECOMPARE(x1,x2)==EQ)&&(DOUBLECOMPARE(y1,y2)==EQ))||
                ((DOUBLECOMPARE(x2,x3)==EQ)&&(DOUBLECOMPARE(y2,y3)==EQ)))
        continue;// 
	
// checks for angle < (PI − EPSILON)or angle > (PI + EPSILON) works even when angle is zero. i.e P1&P3 are same.
        if((DOUBLECOMPARE(angle,(PI+PARAMETERS.EPSILON))==GT)||
                (DOUBLECOMPARE(angle,(PI-PARAMETERS.EPSILON))==LT))
        {     
	CMV[9]=TRUE; 
        }
    }//closes for
}//end of LIC_9()






//*****************LIC10()*********************
void LIC_10()
{
    CMV[10]=FALSE;
    if(NUMPOINTS < 5)
        return;
    int i;
    for(i=0;(i+PARAMETERS.E_PTS+PARAMETERS.F_PTS+1+1< NUMPOINTS);i++)
    {
        double x1 = X[i];
        double x2 = X[i+PARAMETERS.E_PTS+1];
        double x3 = X[i+PARAMETERS.E_PTS+1+PARAMETERS.F_PTS+1];
        double y1 = Y[i];
        double y2 = Y[i+PARAMETERS.E_PTS+1];
        double y3 = Y[i+PARAMETERS.E_PTS+1+PARAMETERS.F_PTS+1];
        //double l12 = get_distance(x1,x2,y1,y2);
        //double l13 = get_distance(x1,x3,y1,y3);
        //double l23 = get_distance(x2,x3,y2,y3);
        //double hp = (l12+l13+l23)/2; // calculating half-perimeter
        //double result = hp*(hp-l12)*(hp-l13)*(hp-l23);
        //if (DOUBLECOMPARE(result,0)==LT)
        //    result = 0;
        //double area = sqrt(result);
        double area = get_area(x1,y1,x2,y2,x3,y3);
        if(DOUBLECOMPARE(area,PARAMETERS.AREA1)==GT)
        {
            CMV[10]=TRUE;
            return;
        }
    }
}//End of LIC_10
//*************LIC_11()****************************
void LIC_11()
{
CMV[11] = FALSE;
if(NUMPOINTS < 3)
return;
int i;
for(i=0;(i+PARAMETERS.G_PTS+1< NUMPOINTS);i++)  
{
if(DOUBLECOMPARE(X[i+PARAMETERS.G_PTS+1],X[i])==LT)
{
CMV[11]= TRUE;
return;
}
}
}//end of LIC11



//*********************LIC12()*************************
void LIC_12()
{
CMV[12]=FALSE;
if(NUMPOINTS <3) //return if NUMPOINTS <3
return;
int i,j;
float d;
for(i=0;(i+PARAMETERS.K_PTS+1< NUMPOINTS);i++)
{
  d= get_distance(X[i],X[i+PARAMETERS.K_PTS+1],Y[i],Y[i+PARAMETERS.K_PTS+1]);
	if (DOUBLECOMPARE(d,PARAMETERS.LENGTH1)==GT)
	{
	  for(j=0;(j+PARAMETERS.K_PTS+1< NUMPOINTS);j++)
          {
	    d= get_distance(X[j],X[j+PARAMETERS.K_PTS+1],Y[j],Y[j+PARAMETERS.K_PTS+1]);
	      if (DOUBLECOMPARE(d,PARAMETERS.LENGTH2)==LT)
	      {
		CMV[12]=TRUE;
		return;
	      }
	  } 
	}
}
}//End of LIC_12

// Does the same thing as LIC_8 except it also checks to see if the points can
// be contained in a circle of RADIUS2.
void LIC_13()
{
    CMV[13] = FALSE;
    // LIC_8 will have already run. Check to see if the result from that
    // function is TRUE
    if (CMV[8]==TRUE) {
        // Check to see if there is a set that cannot be contained in a circle
        // of RADIUS2
        int max_point = NUMPOINTS-PARAMETERS.A_PTS-PARAMETERS.B_PTS-2;
        for (int i=0;i<max_point;++i) {

            int index1 = i+PARAMETERS.A_PTS+1;
            int index2 = index1+PARAMETERS.B_PTS+1;
            double x1 = X[i];
            double x2 = X[index1];
            double x3 = X[index2];
            double y1 = Y[i];
            double y2 = Y[index1];
            double y3 = Y[index2];
            boolean result = cannot_be_contained_in_circle(x1,y1,x2,y2,x3,y3,
                    PARAMETERS.RADIUS2);
            // Swap the result because we want to know if the points can be
            // contained
            if (result==TRUE)
                result=FALSE;
            else
                result=TRUE;

            if (result==TRUE) {
                CMV[13]=result;
                return;
            }
        }
    }
    return;
}

//********************LIC_14()****************************************
void LIC_14()
{
    CMV[14]=FALSE;
    int j;
    //for(i=0;(i+PARAMETERS.E_PTS+PARAMETERS.F_PTS+1+1< NUMPOINTS);i++)
    //{
    //double x1 = X[i];
    //double x2 = X[i+PARAMETERS.E_PTS+1];
    //double x3 = X[i+PARAMETERS.E_PTS+1+PARAMETERS.F_PTS+1];
    //double y1 = Y[i];
    //double y2 = Y[i+PARAMETERS.E_PTS+1];
    //double y3 = Y[i+PARAMETERS.E_PTS+1+PARAMETERS.F_PTS+1];
    //double l12 = get_distance(x1,x2,y1,y2);
    //double l13 = get_distance(x1,x3,y1,y3);
    //double l23 = get_distance(x2,x3,y2,y3);
    //double hp = (l12+l13+l23)/2; // calculating half-perimeter
    //double area = sqrt(hp*(hp-l12)*(hp-l13)*(hp-l23));
    //find if the area formed by the points is greater than PARAMETER.AREA1
    //if(DOUBLECOMPARE(area,PARAMETERS.AREA1)==GT)
    if (CMV[10]==TRUE) {
        for(j=0;(j+PARAMETERS.E_PTS+PARAMETERS.F_PTS+1+1< NUMPOINTS);j++)
        {
            double x1 = X[j];
            double x2 = X[j+PARAMETERS.E_PTS+1];
            double x3 = X[j+PARAMETERS.E_PTS+1+PARAMETERS.F_PTS+1];
            double y1 = Y[j];
            double y2 = Y[j+PARAMETERS.E_PTS+1];
            double y3 = Y[j+PARAMETERS.E_PTS+1+PARAMETERS.F_PTS+1];
            //double l12 = get_distance(x1,x2,y1,y2);
            //double l13 = get_distance(x1,x3,y1,y3);
            //double l23 = get_distance(x2,x3,y2,y3);
            //double hp = (l12+l13+l23)/2; // calculating half-perimeter
            //double result = hp*(hp-l12)*(hp-l13)*(hp-l23);
            //// result shouldn't be a negative number, it should be multiplying
            //// all positive numbers. If it is negative, some weird overflow
            //// occured
            //if (DOUBLECOMPARE(result,0)==LT)
            //    result = 0;
            //double area = sqrt(result);
            double area = get_area(x1,y1,x2,y2,x3,y3);
            //find if the area formed by the points is less than PARAMETER.AREA2
            if(DOUBLECOMPARE(area,PARAMETERS.AREA2)==LT)
            {       
                CMV[14]=TRUE;
                return;
            }
        }
    }
}//End of LIC_14

void DECIDE(void) {

    // Call launch interceptor condition functions
    LIC_0();
    LIC_1();
    LIC_2();
    LIC_3();
    LIC_4();
    LIC_5();
    LIC_6();
    LIC_7();
    LIC_8();
    LIC_9();    
    LIC_10();
    LIC_11();
    LIC_12();
    LIC_13();
    LIC_14();


    // Loop through and populate the PUM
    int row, col;
    for(row = 0; row < 15; row++) {
        for(col = 0; col < 15; col++) {
            //Skip diagonal elements of the PUM, they are inputs.
            if(row == col) {
                continue;
            }
            // Check conditions in the LCM and CMV to populate the PUM
            switch(LCM[row][col]) {
                case NOTUSED:
                    PUM[row][col] = TRUE;
                    break;
                case ANDD:
                    if(CMV[row] && CMV[col]) {
                        PUM[row][col] = TRUE;
                    } else {
                        PUM[row][col] = FALSE;
                    }
                    break;
                case ORR:
                    if(CMV[row] || CMV[col]) {
                        PUM[row][col] = TRUE;
                    } else {
                        PUM[row][col] = FALSE;
                    }
                    break;
            } // End switch
        } // End col 'for' loop
    } // End row 'for' loop

    /* Populate the FUV from the PUM
     * If the PUM diagonal element is FALSE, automatically set FUV element
     * Otherwise set only if entire PUM row is TRUE 
     */
    for(row = 0; row < 15; row++) {
        //Test diagonal elements of the PUM
        if(PUM[row][row] == FALSE ) {
            FUV[row] = TRUE;
            continue; 
        }
        
        boolean flag = TRUE; 
        for(col = 0; col < 15; col++) {
            if(PUM[row][col] == FALSE) {
                flag = FALSE;
            }
        }
        FUV[row] = flag;
    }

    // Set the final launch enable if the FUV elements are all TRUE
    LAUNCH = TRUE;
    for(row = 0; row < 15; row++) {
        if(FUV[row] == FALSE) {
            LAUNCH = FALSE;
        }
    }

} // End function DECIDE()


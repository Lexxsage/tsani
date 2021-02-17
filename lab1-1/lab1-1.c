//==============================================================================
//
// Title:       lab1-1
// Purpose:     A short description of the command-line tool.
//
// Created on:  01.01.2008 at 0:18:01 by Gennady Kuzin.
// Copyright:   NSU. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include <utility.h>
#include <userint.h> 
#include <ansi_c.h>
//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

/// HIFN  Explain how to use this command-line tool.
/// HIPAR name/The name of the command-line tool.
static void usage (char *name)
{
    fprintf (stderr, "usage: %s <argument>\n", name);
    fprintf (stderr, "A short summary of the functionality.\n");
    fprintf (stderr, "    <argument>    is an argument\n");
    exit (1);
}

//==============================================================================
// Global variables

//==============================================================================
// Global functions

/// HIFN  The main entry-point function.
/// HIPAR argc/The number of command-line arguments.
/// HIPAR argc/This number includes the name of the command-line tool.
/// HIPAR argv/An array of command-line arguments.
/// HIPAR argv/Element 0 contains the name of the command-line tool.
/// HIRET Returns 0 if successful.
int CalculateRoots(float A, float B, float C, float *pD, float *pValue1, float *pValue2)
{
	double DA = A, DB = B, DC = C, D = 0;	
	int result;
	
	if(DA == 0)
	{
		if(DB == 0)
		{
			D = 0;
			if(DC != 0)
			{
				result = 5; //no roots
			}
			else // DC == 0
			{
				result = 6; //oo number of roots
			}
		}
		else //DB != 0
		{
			D = B*B;
			*pValue1 = (float)(-DC/DB);
			result = 4; //Line, one root
		}
	}
	else // DA != 0
	{
		D = DB*DB - 4*DA*DC;
		if(D > 0)
		{
			*pValue1 = (float)((-DB - sqrt(D))/(2*DA));
			*pValue2 = (float)((-DB + sqrt(D))/(2*DA));
			result = 2; //2 real roots
		}
		else if(D == 0)
		{
			*pValue1 = (float)(-DB/(2*DA));
			result = 1; //1 root
		}
		else
		{
			*pValue1 = (float)(-DB/(2*DA));
			*pValue2 = (float)(sqrt(-D)/(2*DA));
			result = 3; //2 complex roots
		}
	}
	
	*pD = (float)D;
	return result;
};

int main ()
{
	float A, B, C, D, Value1, Value2;
	int but = 0, result;
	
	while(( but != 0x0051) && (but != 0x0071)) //q or Q
	{
		printf("Enter A B C:\n");
	    scanf("%f %f %f", &A, &B, &C);
		result = CalculateRoots(A, B, C, &D, &Value1, &Value2);
		switch(result)
	    {
	        case 1:
	            printf("1 root:\nX = %f\n",Value1);
	            break;

	        case 2:
	            printf("2 real roots:\nX1 = %f\nX2 = %f\n",Value1,Value2);
		    	printf("Parabola's vertex:\nX = %f\nY = %f\n", (-B/(2*A)), ((4*A*C - B*B)/(4*A)));
	            break;

	        case 3:
	            printf("2 complex roots:\nX1 = %f + i*%f\nX2 = %f - i*%f\n",Value1,Value2,Value1,Value2);
	            break;

	        case 4:
	            printf("A linear equitation, 1 root:\nX = %f\n",Value1);
	            break;
			
			case 5:
	            printf("No roots.\n");
	            break;
			
			case 6:
	            printf("Infinite number of roots.\n");
	            break;

	        default:
	            printf("Error");
	    }
		
		printf("\nPress q or Q to quit.\n");
		but = GetKey();
	};
	
    return 0;
}

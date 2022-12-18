/*
**  COPYRIGHT NOTICE:
**	This computer software is the confidential and proprietary product of
**	Maximum Research & Development (P.O. Box 767849, Roswell, GA  30076) and
**	Advanced Software Research (2345 Farmbell Lane, Lawrenceville, GA  30244).
**	Any unauthorized use, reproduction, or transfer of this software is
**	strictly prohibited.  Copyright 1993 by Maximum Research & Development and
**	Advanced Software Research.  (Subject to limited distribution and restricted
**	disclosure only.)  All rights reserved.
*/


#include "rwfsg.hpp"
#include "rwflist.hpp"
#include "rwfgraph.hpp"

#include <string.h>
#include <conio.h>



int main(int argc,char *argv[])
{
	float x,xmin,xmax,ymin,ymax;
	RWFsg_ReverseGasket rcgasket;

	rwf_graphInitialize(0);
	x = 1.25 / 2.0;
	xmin = -1.333 * x;
	xmax = 1.333 * x;
	ymin = -1.0 * x;
	ymax = 1.0 * x;
	rwf_graphSetWindow(xmin,ymin,xmax,ymax);

/*
**	Draw Sierpinski Gasket by Reverse Method (Color)
*/
	rcgasket.setMaximumLevel(8);
	rcgasket.setMinimumSize(0.01);
	rwf_graphEraseScreen();
	rcgasket.fillColor();
	rwf_miscPressAnyKey();

/*
**	Return
*/
	rwf_graphClose();
	return 0;
}
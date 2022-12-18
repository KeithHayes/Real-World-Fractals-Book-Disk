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
	RWFsg_ReverseGasket rc1gasket;
	RWFsg_ReverseCarpet rfcarpet;
	RWFlist_ObjectPtrTable etable;

	rwf_graphInitialize(0);
	x = 1.25 / 2.0;
	xmin = -1.333 * x;
	xmax = 1.333 * x;
	ymin = -1.0 * x;
	ymax = 1.0 * x;
	rwf_graphSetWindow(xmin,ymin,xmax,ymax);

/*
**	Draw Sierpinski Carpet filled with Sierpinski Gaskets
*/
	rc1gasket.scale(1/3.0,1/3.0);
	rc1gasket.setColor(RWFGRAPH_COLOR_WHITE);
	rc1gasket.setMaximumLevel(8);
	rc1gasket.setMinimumSize(0.003);
	rc1gasket.setAction(&rwf_afFillColorCycle);
	etable[0] = &rc1gasket;

	rfcarpet.setEraseInitiator(etable);
	rfcarpet.setColor(RWFGRAPH_COLOR_LIGHT_GRAY);
	rfcarpet.setMaximumLevel(4);
	rfcarpet.setMinimumSize(0.003);

	rwf_graphEraseScreen();
	rfcarpet.fill();
	rwf_miscPressAnyKey();

/*
**	Return
*/
	rwf_graphClose();
	return 0;
}
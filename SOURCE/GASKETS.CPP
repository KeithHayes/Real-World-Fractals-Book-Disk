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
#include "rwfrand.hpp"

#include <string.h>
#include <conio.h>
#include <dos.h>



int main(int argc,char *argv[])
{
	int i,numlevels,numvertices;
	float x,y,xmin,xmax,ymin,ymax,height,r,angle;
	RWFgraph_Palette pal(16), pal2(16);

	RWFsg_Gasket gasket, igasket;
	RWFsg_Carpet carpet, icarpet;
	RWFsg_ReverseGasket rgasket, irgasket;
	RWFsg_ReverseCarpet rcarpet, ircarpet;

	RWFlist_ObjectPtrTable otable;
	RWFmisc_Boolean palette_cycle = RWFMISC_FALSE;
	RWFmisc_Boolean random_mode = RWFMISC_TRUE;
	long seed = 989899L;

	RWFaf_Object *object = &gasket;
	RWFgraph_Polygon *triangle = new RWFgraph_Polygon(3);
	RWFgraph_Polygon *square   = new RWFgraph_Polygon(4);
	RWFgraph_Circle  *circle   = new RWFgraph_Circle(0.0, 0.0, 0.5);
	RWFgraph_Polygon *ran_poly = new RWFgraph_Polygon(6);

	// Set color action functions
	rgasket.setAction(rwf_afFillColorCycle);
	rcarpet.setAction(rwf_afFillColorCycle);

/*
**	Define basic object types
*/
	// Triangle
	height = sqrt(3.0)/2.0;
	(*triangle)[0].setCoord(-0.5,-height/2.0);
	(*triangle)[1].setCoord( 0.5,-height/2.0);
	(*triangle)[2].setCoord( 0.0, height/2.0);

	// Square
	(*square)[0].setCoord(-0.5,-0.5);
	(*square)[1].setCoord( 0.5,-0.5);
	(*square)[2].setCoord( 0.5, 0.5);
	(*square)[3].setCoord(-0.5, 0.5);

	// Hexagon
	numvertices = 6;
	for(i=0; i<numvertices; i++) {
		r = rwf_randRand();
		angle = ((i+rwf_randRand()-0.5)/numvertices) * RWFMISC_TWOPI;
		x = r*cos(angle);
		y = r*sin(angle);
		(*ran_poly)[i].setCoord(x,y);
	}

	otable[0] = triangle;
	numlevels = 5;

/*
**	Read command line options, if any
*/
	i = 1;
	if(argc > 1)
		random_mode = RWFMISC_FALSE;

	while (i<argc) {
		if (strcmp("-?",argv[(int) i])==0) {
			printf("Usage:  gaskets [-?]\n");
			printf("    -circle   :  Use circle object\n");
			printf("    -numlevels:  Set number of recursion levels\n");
			printf("    -randpoly :  Use random hexagon object\n");
			printf("    -square   :  Use square object\n");
			printf("    -triangle :  Use triangle object\n");
			printf("    -gasket   :  Draw a gasket\n");
			printf("    -carpet   :  Draw a carpet\n");
			printf("    -rgasket  :  Draw a reverse gasket\n");
			printf("    -rcarpet  :  Draw a reverse carpet\n");
			printf("    -random   :  Run in random mode\n");
			printf("    -seed n   :  Set new initial random number seed\n");
			printf("    -cycle    :  Cycle the color palette\n");
			printf("    -?        :  Display usage information (i.e., this message)\n");
			return 0;
		}
		else if (strcmp("-circle",argv[i])==0) {
			otable[0] = circle;
		}
		else if (strcmp("-numlevels",argv[i])==0) {
			i++;
			sscanf(argv[i],"%d",&numlevels);
		}
		else if (strcmp("-randpoly",argv[i])==0) {
			otable[0] = ran_poly;
		}
		else if (strcmp("-square",argv[i])==0) {
			otable[0] = square;
		}
		else if (strcmp("-triangle",argv[i])==0) {
			otable[0] = triangle;
		}
		else if (strcmp("-gasket",argv[i])==0) {
			object = &gasket;
			otable[0] = triangle;
		}
		else if (strcmp("-carpet",argv[i])==0) {
			object = &carpet;
			otable[0] = square;
		}
		else if (strcmp("-rgasket",argv[i])==0) {
			object = &rgasket;
			otable[0] = triangle;
		}
		else if (strcmp("-rcarpet",argv[i])==0) {
			object = &rcarpet;
			otable[0] = square;
		}
		else if (strcmp("-random", argv[i])==0) {
			random_mode = RWFMISC_TRUE;
		}
		else if (strcmp("-seed", argv[i])==0) {
			i++;
			sscanf(argv[i],"%ld",&seed);
		}
		else if (strcmp("-cycle", argv[i])==0) {
			palette_cycle = RWFMISC_TRUE;
		}
		else {
			printf("***Unrecognized command-line argument \"%s\"\n",argv[i]);
			return 1;
		}
		i++;
	}

	rwf_miscTitle("Gaskets", 6);

/*
**	Initialize display
*/
	rwf_graphInitialize(0);
	x = 1.25 / 2.0;
	xmin = -1.333 * x;
	xmax = 1.333 * x;
	ymin = -1.0 * x;
	ymax = 1.0 * x;
	rwf_graphSetWindow(xmin,ymin,xmax,ymax);


	// Setup color cycling palette
	rwf_graphSetPaletteIndex(8, 0, 255, 0);
	rwf_graphGetPalette(0, pal);
	rwf_graphGetPalette(0, pal2);

	rwf_randSrand(seed);

/*
**	Draw the selected object
*/
BIGLOOP:

	if(random_mode) {
		int which_object = rwf_randRand() * 4;
		switch(which_object) {
			case 0: object = &gasket; break;
			case 1: object = &carpet; break;
			case 2: object = &rgasket; break;
			case 3: object = &rcarpet; break;
			default: object = &gasket;
		}
		which_object = rwf_randRand() * 7;
		switch(which_object) {
			case 0: otable[0] = triangle; break;
			case 1: otable[0] = square; break;
			case 2: otable[0] = ran_poly; break;
			case 3: otable[0] = &igasket; break;
			case 4: otable[0] = &icarpet; break;
			case 5: otable[0] = &irgasket; break;
			case 6: otable[0] = &ircarpet; break;
			default: otable[0] = triangle;
		}
	}

	object->setInitiator(otable);
	int color = rwf_randRand() * 14 + 1;
	object->setColor(color);

	int start_level;
	if(random_mode)
		start_level = RWFMISC_MAX(0, numlevels - 3);
	else
		start_level = 0;

	for(int level=start_level; level <= numlevels; level++) {
		object->setMaximumLevel(level);
		object->setMinimumSize(0.005);
		rwf_graphEraseScreen();
		rwf_miscCheckEscape();
		object->fill();
		int index = 1;
		if(random_mode && level == numlevels && palette_cycle) {
			for(int loop=0; loop < 16*10; loop++) {
				for(i=1; i < 16; i++)
					pal2[i] = pal[(i+index)%15 + 1];
				rwf_graphSetPalette(0, pal2);
				delay(100);
				index = (index+1) % 16;
			}
			rwf_graphSetPalette(0, pal);
		} else if (random_mode) {
			delay(1000);
		}	else {
			rwf_miscPressAnyKey();
		}
	}

	object->setMaximumLevel(start_level);
	if(random_mode)
		goto BIGLOOP;

/*
**	Return
*/
	rwf_miscPressAnyKey();
	rwf_graphClose();
	return 0;
}
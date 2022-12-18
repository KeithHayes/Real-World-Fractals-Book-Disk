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


#include "rwfvk.hpp"
#include "rwfgraph.hpp"
#include "rwfrand.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int main(int argc,char *argv[])
{
	int i,numPointsInitiator;
	int numFractalLevels;
	float x,y;

	RWFvk_Object vk;
	RWFmisc_Boolean docircle = RWFMISC_FALSE;
	RWFmisc_Boolean dopeano  = RWFMISC_FALSE;
	RWFmisc_Boolean dosquare = RWFMISC_FALSE;
	RWFmisc_Boolean dopoly   = RWFMISC_FALSE;
	RWFmisc_Boolean random_mode = RWFMISC_TRUE;

	RWFlist_ObjectPtrTable initiator;
	RWFgraph_Line *line;
	RWFgraph_Circle *circle;
	RWFgraph_Polyline poly;
	RWFgraph_Polyline generator;

/*
**	Set default parameters for this program
*/
	numFractalLevels = 5;
	numPointsInitiator = 6;

/*
**	Read command line options, if any
*/
	i = 1;
	if(argc > 1)
		random_mode = RWFMISC_FALSE;

	while (i<argc) {
		if (strcmp("-fl",argv[i])==0) {
			i++;
			sscanf(argv[i],"%d",&numFractalLevels);
		}
		else if (strcmp("-circle", argv[i])==0) {
			docircle = RWFMISC_TRUE;
		}
		else if (strcmp("-peano",argv[i])==0) {
			dopeano = RWFMISC_TRUE;
		}
		else if (strcmp("-square",argv[i])==0) {
			dosquare = RWFMISC_TRUE;
		}
		else if (strcmp("-poly", argv[i])==0) {
			dopoly = RWFMISC_TRUE;
		}
		else if (strcmp("-random", argv[i])==0) {
			random_mode = RWFMISC_TRUE;
		}
		else if (strcmp("-inp",argv[i])==0) {
			i++;
			sscanf(argv[i],"%d",&numPointsInitiator);
		}
		else if (strcmp("-?",argv[(int) i])==0) {
			printf("Usage:  vonkoch [-?] [-fl fractalLevel]\n");
			printf("                [-inp numPointsInitiator]\n");
			printf("                [-peano] [-square] [-circle]\n");
			printf("    -fl    :  Set fractal level\n");
			printf("    -inp   :  Set number of vertices in initiator\n");
			printf("    -peano :  Draw peano curve\n");
			printf("    -square:  Draw square curve");
			printf("    -circle:  Draw circles for initiators\n");
			printf("    -poly  :  Draw polygons for the initiators\n");
			printf("    -?     :  Display usage information (i.e., this message)\n");
			return 0;
		}
		else {
			printf("***Unrecognized command-line argument \"%s\"\n",argv[(int) i]);
					return 1;
		}
		i++;
	}

	rwf_miscTitle("VonKoch", 5);

	rwf_graphInitialize(0);
	// Account for aspect ratio
	x = 1.3;
	rwf_graphSetWindow(-1.333*x, -1.0*x, 1.333*x, 1.0*x);
	rwf_randSrand(348788L);

/*
**	Draw snowflake
*/
	int npass = 0;


BIGLOOP:

	float angle = 0.0;
	float anginc = 360.0 / (float)numPointsInitiator;

	x = 1.0 - cosD(anginc);
	y = sinD(anginc);
	float radius = 1.0;
	float sidelength = sqrt(x*x + y*y);

	if(docircle) {
		numPointsInitiator--;
		if(numPointsInitiator == 1) {
			radius = 0.0;
			numPointsInitiator = 0;
		}
	} else if (numPointsInitiator == 2) {
		numPointsInitiator = 1;
	}

	generator.reset();
	for(i=0; i < initiator.size(); i++)
		delete initiator[i];
	initiator.reset();
	poly.reset();

	for(i=0; i <= numPointsInitiator; i++) {
		x = radius*cosD(angle);
		y = radius*sinD(angle);
		poly[i].setCoord(x, y);

		// Must map the position of the circle to that
		// of the unit line segment
		// We also want the circle to appear along the endpoints
		// of the generator, not in the middle of the generator
		// Thus, we set the center to (-0.5, 0.0) (over the first
		// point of the initiator and use transformations to move
		// it to the correct position on the polygon
		// Note that setInitiator() handles this when you use a polygon
		// initiator.

		circle = new RWFgraph_Circle(-0.5, 0.0, 0.5);
		if(radius > 0.0) {
			circle->translate(-0.5, 0.0);
			circle->scale(sidelength, sidelength);
			circle->rotate(-90.0 + anginc/2);
			circle->translate(radius, 0.0);
			circle->rotate(angle);
		}

		initiator[i] = circle;

		angle += anginc;
	}

	if(docircle)
		vk.setInitiator(initiator);
	else if (dopoly) {
		initiator.reset();
		initiator[0] = &poly;
		vk.setInitiator(initiator);
	} else
		vk.setInitiator(poly);

	if(dosquare) {
		generator[0].setCoord(0.0, 0.0);
		generator[1].setCoord(1.0, 0.0);
		generator[2].setCoord(1.0, 1.0);
		generator[3].setCoord(2.0, 1.0);
		generator[4].setCoord(2.0, 0.0);
		generator[5].setCoord(2.0,-1.0);
		generator[6].setCoord(3.0,-1.0);
		generator[7].setCoord(3.0, 0.0);
		generator[8].setCoord(4.0, 0.0);
		vk.setGenerator(generator);
	} else if (dopeano) {
		generator[0].setCoord(0.0, 0.0);
		generator[1].setCoord(1.0, 0.0);
		generator[2].setCoord(1.0, 1.0);
		generator[3].setCoord(2.0, 1.0);
		generator[4].setCoord(2.0, 0.0);
		generator[5].setCoord(1.0, 0.0);
		generator[6].setCoord(1.0,-1.0);
		generator[7].setCoord(2.0,-1.0);
		generator[8].setCoord(2.0, 0.0);
		generator[9].setCoord(3.0, 0.0);
		vk.setGenerator(generator);
	}

	int start_level;
	if(random_mode) {
		start_level = 2;
	} else {
		start_level = 0;
	}

	for(int level=start_level; level < numFractalLevels; level++) {
		vk.setMaximumLevel(level);
		vk.setMinimumSize(0.01);

		vk.setColor(RWFGRAPH_COLOR_WHITE);
		if(random_mode) {
			x = (rwf_randRand() - 0.5)*2.0;
			y = (rwf_randRand() - 0.5)*2.0;
			float scale = 0.5 + (rwf_randRand()-0.5)*0.5;
			vk.identity();
			vk.scale(scale, scale);
			vk.translate(x,y);
			int color = 8;
			while(color == 8)
				color = rwf_randRand()*14 + 1;
			vk.setColor(color);
		} else {
			rwf_graphEraseScreen();
		}
		rwf_miscCheckEscape();
		vk.draw();
		if(!random_mode)
			rwf_miscPressAnyKey();
		else
			npass++;
	}

	if(random_mode) {
		numPointsInitiator = rwf_randRand() * 10 + 2;
		if(npass > 20) {
			rwf_graphEraseScreen();
			npass = 0;
		}
		docircle = !docircle;
		if(dosquare) {
			dopeano = RWFMISC_TRUE;
			dosquare = RWFMISC_FALSE;
		} else if (dopeano) {
			dopeano = RWFMISC_FALSE;
			dosquare = RWFMISC_FALSE;
		} else {
			dosquare = RWFMISC_TRUE;
			dopeano = RWFMISC_FALSE;
		}

		goto BIGLOOP;
	}
/*
**	Return
*/

	rwf_miscPressAnyKey();
	rwf_graphClose();

	return 0;
}
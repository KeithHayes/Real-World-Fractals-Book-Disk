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

/*
** Mountain drawing program
*/

#include "rwfgrid.hpp"
#include "rwfrand.hpp"

#include <string.h>
#include <conio.h>
#include <ctype.h>
#include <stdio.h>


int main(int argc,char *argv[])
{
	int i;
	float x,xmin,xmax,ymin,ymax;
	RWFgraph_ViewingGeometry vg;

/*
**	Read command line options, if any
*/
	i = 1;
	while (i<argc) {
		if (strcmp("-?",argv[(int) i])==0) {
			printf("Usage:  mountain [-?]\n");
			printf("    -?           :  Display usage information (i.e., this message)\n");
			return 0;
		}
		else {
			printf("***Unrecognized command-line argument \"%s\"\n",argv[(int) i]);
			return 1;
		}
//		i++;
	}

	rwf_miscTitle("Mountain", 11);

	rwf_graphInitialize(0);
	x = 1.25 / 2.0;
	xmin = -1.333 * x;
	xmax = 1.333 * x;
	ymin = -1.0 * x;
	ymax = 1.0 * x;
	rwf_graphSetWindow(xmin,ymin,xmax,ymax);

	rwf_graphSetPaletteIndex(0, 150, 150, 180);

	RWFlist_ClassTable ctable;

	// 4 basic classes

	// 0 is water
	ctable[0].setStandardDeviation(0.0);
	ctable[0].setFractalDimension(1.0);
	ctable[0].setColor(1);	// Blue color
	rwf_graphSetPaletteIndex(1, 70, 70, 150);

	// 1 is bushes & shrubs
	ctable[1].setStandardDeviation(0.2);
	ctable[1].setFractalDimension(0.4);
	ctable[1].setColor(2);	// Green
	rwf_graphSetPaletteIndex(2, 80, 140, 70);

	// 2 is barren trees
	ctable[2].setStandardDeviation(0.1);
	ctable[2].setFractalDimension(0.7);
	ctable[2].setColor(6);	// Brown
	rwf_graphSetPaletteIndex(6, 150, 120, 100);

	// 3 is snow cap
	ctable[3].setStandardDeviation(0.05);
	ctable[3].setFractalDimension(1.0);
	ctable[3].setColor(15);	// White
	rwf_graphSetPaletteIndex(15, 180, 180, 180);

	rwf_graphSetColorScale();

	RWFvec_Vector light_vec(3);
	light_vec[0] = -1.0;
	light_vec[1] =  1.0;
	light_vec[2] =  3.0;
	vg.setLight(light_vec);

	int level = 32;
	RWFgraph_Grid *terrain = new RWFgraph_Grid(level, level);
	float angle = 30.0;

	terrain->setClassTable(ctable);
	terrain->setPixelSize(32.0 / level, 32.0 / level);

	// Find the midpoint of the grid

	float xmid = terrain->ncols() / 2.0;
	float ymid = terrain->nrows() / 2.0;
	float sigma2 = (xmid*xmid) / 4.0;

	float height;

	for(int j=0; j < terrain->nrows(); j++) {
		for(i=0; i < terrain->ncols(); i++) {
			float r2 = (i-xmid)*(i-xmid) + (j-ymid)*(j-ymid);
			(*terrain)[i][j].h = height = 10.0*exp(-(r2 / sigma2));
			if(height > 8.0)
				(*terrain)[i][j].class_value = 3;
			else if (height > 5.0)
				(*terrain)[i][j].class_value = 2;
			else if (height > 1.0)
				(*terrain)[i][j].class_value = 1;
			else
				(*terrain)[i][j].class_value = 0;
		}
	}

	terrain->setColor(0);
	float xdist = 32.0;
	float ydist = ((terrain->ncols()-1) * 32.0 / level) / 2.0;
	vg.setLightAttributes(0.3, 0.7);

	// Ok, draw the terrain

	char string[100];

	xmid = rwf_graphGetMaxX() - 1;
	ymid = rwf_graphGetMaxY() - 1;
	for(;;) {
		for(level = 0; level < 5; level++) {
			if(level == 0)
				rwf_graphEraseScreen();
			switch(level) {
				case 0: vg.setScreen(0, 0, xmid/2, ymid/2); break;
				case 1: vg.setScreen(xmid/2+1, 0, xmid, ymid/2); break;
				case 2: vg.setScreen(0, ymid/2+1, xmid/2, ymid); break;
				case 3: vg.setScreen(xmid/2+1, ymid/2+1, xmid, ymid); break;
				case 4:
					rwf_graphEraseScreen();
					vg.setScreen(0, 0, xmid, ymid);
					break;
			}
			terrain->identity();
			terrain->translate(-ydist, -ydist, 0.0);
			terrain->scale(1.5, 1.5, 1.2);
			terrain->rotate(0.0, 10.0, angle);
			terrain->translate(ydist, ydist, 0.0);
			terrain->translate(xdist, -ydist, -8.0);
			terrain->setMaximumRecursion(level);
			terrain->draw(vg);
			setcolor(255);
			sprintf(string, "Recursion level %d, Number of polygons = %ld",level,
							(2048L << (2*level)));

			switch(level) {
				case 0: outtextxy(0, ymid/2 - 20, string); break;
				case 1: outtextxy(xmid/2 + 1, ymid/2 - 20, string); break;
				case 2: outtextxy(0, ymid - 20, string); break;
				case 3: outtextxy(xmid/2 + 1, ymid - 20, string); break;
				case 4: outtextxy(0, ymid - 20, string); break;
			}
			if(level >= 3)
				rwf_miscPressAnyKey();
		}
		angle += 30.0;
	}

/*
**	Return
*/
	rwf_miscPressAnyKey();
	rwf_graphClose();
	return 0;
}
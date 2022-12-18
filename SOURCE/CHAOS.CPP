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


#include "rwfchaos.hpp"
#include "rwfgraph.hpp"
#include "rwfrand.hpp"

#include <string.h>
#include <stdio.h>



int main(int argc,char *argv[])
{
	int i,j,k,connect,numrad,numang,numr;
	int imin,imax,jmin,jmax;
	long iint,numpasses;
	float r,x,xmin,xmax,ymin,ymax,rmin,rmax,dr,pmin,pmax;
	complex c(0.5,0.5);
	RWFvec_Vector startpoint(3);

/*
**	Read command line options, if any
*/
	i = 1;
	while (i<argc) {
		if (strcmp("-?",argv[(int) i])==0) {
			printf("Usage:  chaos [-?]\n");
			printf("    -?        :  Display usage information (i.e., this message)\n");
			return 0;
		}
		else {
			printf("***Unrecognized command-line argument \"%s\"\n",argv[(int) i]);
			return 1;
		}
		i++;
	}

	rwf_miscTitle("Chaos", 13);

	rwf_graphInitialize(0);

	RWFgraph_2DPlotObject pobject;
	RWFchaos_LorenzData ldata;
	RWFchaos_PopulationData pdata;
	RWFchaos_Z2Data zdata(c);

/*
**	Plot Lorenz attractor curve
*/
	numpasses = 60000;
	connect = 1;
	startpoint[0] = .5;
	startpoint[1] = -.5;
	startpoint[2] = .25;

	x = 25.0;
	xmin = -1.333 * x;
	xmax = 1.333 * x;
	ymin = -1.0 * x + 25.0;
	ymax = 1.0 * x + 25.0;

	pobject.setWindow(xmin,ymin,xmax,ymax);
	pobject.setPlotFunction(&rwf_chaosPlotLorenzAttractor);
	pobject.setInitialPoint(startpoint,&ldata,connect);
	pobject.setNumIterations(numpasses);

	rwf_graphEraseScreen();
	pobject.plot();

	rwf_miscPressAnyKey("Lorenz Attractor for 60000 passes");

/*
**	Plot (Z**2 + c) orbits
*/
	numpasses = 8;
	connect = 0;
	startpoint[2] = 0.0;

	x = 2.0;
	xmin = -1.333 * x;
	xmax = 1.333 * x;
	ymin = -1.0 * x;
	ymax = 1.0 * x;

	pobject.setWindow(xmin,ymin,xmax,ymax);
	pobject.setPlotFunction(&rwf_chaosPlotZ2Attractor);
	pobject.setNumIterations(numpasses);

	rwf_graphEraseScreen();
	setcolor(RED);
	rwf_graphCircleDraw(0.0,0.0,1.0);
	pobject.setColor(0);

	// Interesting Colors
	rwf_graphSetPaletteIndex(1, 255, 255, 255);
	rwf_graphSetPaletteIndex(2, 255,   0,   0);
	rwf_graphSetPaletteIndex(3,   0, 255,   0);
	rwf_graphSetPaletteIndex(4,   0,   0, 255);
	rwf_graphSetPaletteIndex(5, 255, 255,   0);
	rwf_graphSetPaletteIndex(6,   0, 255, 255);
	rwf_graphSetPaletteIndex(7, 255,   0, 255);
	rwf_graphSetPaletteIndex(8, 128, 255,   0);
	rwf_graphSetPaletteIndex(9, 100, 128, 255);


	numrad = 20;
	numang = 2000;
	for (i=0; i<numrad; i++) {
		r = (i+1.0)/(numrad+1.0);
		for (j=0; j<numang; j++) {
			x = ((j+0.5)/numang) * RWFMISC_TWOPI;
			startpoint[0] = r * cos(x);
			startpoint[1] = r * sin(x);
			pobject.setInitialPoint(startpoint,&zdata,connect);
			pobject.plot();
		}
	}

	rwf_miscPressAnyKey("Orbits for z -> z**2 + c, 8 iterations");

/*
**	Plot Feigenbaum diagram
*/
	numpasses = 300;
	connect = 0;
	char string[100];

	rmin = 0.0;
	rmax = 4.0;
	dr = 0.005;
	pmin = 0.0;
	pmax = 1.0;

	pobject.setColor(RWFGRAPH_COLOR_WHITE);
	while(rmin < 3.76) {
		xmin = rmin;
		xmax = rmax;
		ymin = pmin;
		ymax = pmax;

		pobject.setWindow(xmin,ymin,xmax,ymax);
		pobject.setPlotFunction(&rwf_chaosPlotPopulationFeigenbaum);
		pobject.setNumIterations(numpasses);

		rwf_graphEraseScreen();

		setlinestyle(DOTTED_LINE,1,NORM_WIDTH);
		rwf_graphLineDraw(rmin,0.0,rmax,0.0);
		rwf_graphLineDraw(rmax,0.0,rmax,1.0);
		rwf_graphLineDraw(rmax,1.0,rmin,1.0);
		rwf_graphLineDraw(rmin,1.0,rmin,0.0);
		setlinestyle(SOLID_LINE,1,NORM_WIDTH);
		r = rmin;
		while (r<rmax) {
			pdata.r = r;
			startpoint[0] = 0.5;
			pobject.setInitialPoint(startpoint,&pdata,connect);
			pobject.plot();
			r += dr;
		}

		sprintf(string,"Feigenbaum Diagram from r = %4.2f to r = 4.0",rmin);
		rwf_miscPressAnyKey(string);

		rmin += (4.0 - rmin) / 2.0;
		pmin += (1.0 - pmin) / 4.0;
		dr = dr/2.0;
	}

	rwf_graphClose();
/*
**	Return
*/
	return 0;
}
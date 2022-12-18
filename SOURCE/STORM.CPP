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
** Mountain drawing program with trees and clouds
*/

#include "rwffbm.hpp"
#include "rwfrand.hpp"
#include "rwftree.hpp"
#include "svga256.h"

#include <string.h>
#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <dos.h>

int main(int argc,char *argv[])
{
	int i;
	float x,xmin,xmax,ymin,ymax;
	RWFgraph_ViewingGeometry vg;
	char string[100];

	RWFgraph_Palette savepal(256), temppal(16);

	int   numclouds       = 8;
	int   use_sound       = 1;

/*
**	Read command line options, if any
*/
	i = 1;
	while (i<argc) {
		if (strcmp("-?",argv[(int) i])==0) {
			printf("Usage:  storm [-?]\n");
			printf("    -?             :  Display usage information (i.e., this message)\n");
			printf("    -sound         :  Enable lightning sound\n");
			printf("    -nosound       :  Disable lightning sound\n");
			printf("    -numclouds <n> : Specify number of clouds\n");
			return 0;
		} else if(!strcmp("-numclouds", argv[i])) {
			i++;
			sscanf(argv[i],"%d",&numclouds);
		} else if(!strcmp("-sound", argv[i])) {
			use_sound = 1;
		} else if(!strcmp("-nosound", argv[i])) {
			use_sound = 0;
		} else {
			printf("***Unrecognized command-line argument \"%s\"\n",argv[(int) i]);
			return 1;
		}
		i++;
	}

	string[0] = 0;
	numclouds = RWFMISC_MAX(numclouds, 2);

	rwf_miscTitle("Storm", 10);

	rwf_graphInitialize(0);

	// Sky color
	rwf_graphSetPaletteIndex(0, 130, 130, 140);

	for(i=0; i < 8; i++) {
		int gray = (i/7.0) * 60.0 + 150;
		rwf_graphSetPaletteIndex(i+1, gray, gray, gray);
	}

	rwf_graphSetColorScale();

	for(i=140; i < 256; i++)
		rwf_graphSetPaletteIndex(i, 255, 255, 255);

	// Ok, save this palette away for lightning flash
	rwf_graphGetPalette(0, savepal);
	for(i=1; i < 256; i++) {
		int red, green, blue, gray;
		savepal[i].getRGB(red, green, blue);
		gray = RWFMISC_MAX(red, 255 - red);
		savepal[i].setRGB(gray, gray, gray);
	}

	RWFvec_Vector light_vec(3);
	vg.setEyepoint(0.0, 0.0, 0.0);
	light_vec[0] = -1.0;
	light_vec[1] =  1.0;
	light_vec[2] =  1.5;
	vg.setLight(light_vec);
	vg.setLightAttributes(0.4, 0.6);

	rwf_graphEraseScreen();

	// Draw the clouds
	RWFtree_3DCraterObject cloud;
	cloud.setMaximumLevel(4);
	cloud.setMinimumSize(0.01);

	RWFvec_Vector *centers = new RWFvec_Vector [numclouds];
	RWFvec_Vector tvec(3);
	for(i=0; i < numclouds; i++)
		centers[i] = tvec;

	rwf_randSrand(7878787L);

	for(i=0; i < numclouds; i++) {
		cloud.setColor((i%8) + 1);
		cloud.identity();
		cloud.scale(2000.0, 2000.0, 1000.0);
		centers[i][0] = rwf_randRand()*3000.0 + 6000.0;
		centers[i][1] = (rwf_randRand()-0.5)*6000.0;
		centers[i][2] = 5000.0*(rwf_randRand()-0.5);
		cloud.translate(centers[i]);
		cloud.draw(vg);
	}

	// Generate some lightning

	int nlevels = 7;
	int npoints = (1 << nlevels) + 1;
	float *offsets = new float [npoints];

	// Now to make it 3D
	int i1, i2;
	RWFvec_Vector stepvec(3), normvec(3), center1(3), center2(3);
	int x0, y0, x1, y1;

	setwritemode(XOR_PUT);
	setcolor(255);

	for(int bolt=0; bolt < numclouds*10; bolt++) {
		do {
			i1 = rwf_randRand()*numclouds;
			i2 = rwf_randRand()*numclouds;
		} while (i1 == i2);

		// Bolt goes between centers of clouds
		center1 = centers[i1];
		center1[0] += (rwf_randRand() - 0.5)*500.0;
		center1[1] += (rwf_randRand() - 0.5)*500.0;
		center1[2] += (rwf_randRand() - 0.5)*500.0;

		center2 = centers[i2];
		center2[0] += (rwf_randRand() - 0.5)*500.0;
		center2[1] += (rwf_randRand() - 0.5)*500.0;
		center2[2] += (rwf_randRand() - 0.5)*500.0;

		float stdev = (center1 - center2).length() / 6.0;
		rwf_fbm1DGenerate(0.0, 0.0, NULL, stdev, 1.0 / sqrt(2.0), nlevels, offsets);

		// normvec is perpendicular to the line between the 2 centers
		stepvec = (center2 - center1) * (1.0/(npoints-1));
		if(ABS(stepvec[0]) > ABS(stepvec[1])) {
			normvec[0] = -stepvec[2];
			normvec[1] =  0.0;
			normvec[2] =  stepvec[0];
		} else {
			normvec[0] = 0.0;
			normvec[1] = -stepvec[2];
			normvec[2] = stepvec[1];
		}
		normvec.normalize();

		tvec = centers[i1];
		vg.projectCoords(tvec, x0, y0);

		// Now step along the line between the centers
		// Use the fBM to deviate from the straight line
		// normvec provides the direction to offset the point

		for(i=0; i < npoints; i++) {
			tvec += stepvec;
			vg.projectCoords((tvec + normvec*offsets[i]), x1, y1);
			line(x0, y0, x1, y1);
			x0 = x1;
			y0 = y1;
		}

		rwf_miscCheckEscape();

		// Make the lightning flash!
		int k=rwf_randRand() * 4 + 1;
		for(i=0; i < temppal.size(); i++)
			temppal[i] = savepal[i+k+(i1+1)*16];

		rwf_graphSetPalette((i1 + 1)*16, temppal);

		if(use_sound)
			sound((int)(75 + rwf_randRand()*50));
		delay(200);
		rwf_graphSetPalette(0, savepal);

		tvec = centers[i1];
		vg.projectCoords(tvec, x0, y0);
		for(i=0; i < npoints; i++) {
			tvec += stepvec;
			vg.projectCoords((tvec + normvec*offsets[i]), x1, y1);
			line(x0, y0, x1, y1);
			x0 = x1;
			y0 = y1;
		}

		if(use_sound)
			sound((int)(25 + rwf_randRand()*50));
		delay(200);
		nosound();

		delay((int)(200 + (rwf_randRand()*600)));


	}

/*
**	Return
*/
	rwf_miscPressAnyKey();
	rwf_graphClose();
	return 0;
}



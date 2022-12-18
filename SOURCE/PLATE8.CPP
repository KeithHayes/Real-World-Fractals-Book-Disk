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


#include "rwftree.hpp"
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

	int numclouds = 5;

/*
**	Read command line options, if any
*/
	i = 1;
	while (i<argc) {
		if (strcmp("-?",argv[(int) i])==0) {
			printf("Usage:  plate8 [-?]\n");
			printf("    -?        :  Display usage information (i.e., this message)\n");
			return 0;
		}
		else {
			printf("***Unrecognized command-line argument \"%s\"\n",argv[(int) i]);
			return 1;
		}
//		i++;
	}

	rwf_miscTitle("Plate8",7);

	rwf_graphInitialize(0);

	x = 1.2 / 2.0;
	xmin = -1.333 * x;
	xmax = 1.333 * x;
	ymin = -1.0 * x;
	ymax = 1.0 * x;
	rwf_graphSetWindow(xmin,ymin,xmax,ymax);
	rwf_graphEraseScreen();

	// Set up the palette
	rwf_graphSetPaletteIndex(0, 130, 170, 200);
	rwf_graphSetPaletteIndex(1, 130, 170, 100);
	rwf_graphSetPaletteIndex(2, 150, 130,  80);
	rwf_graphSetPaletteIndex(3, 150, 130, 100);
	rwf_graphSetPaletteIndex(4, 130, 110,  80);
	rwf_graphSetPaletteIndex(5, 140, 110,  50);
	for(i=6; i < 15; i++)
		rwf_graphSetPaletteIndex(i, 150, 120, 90);

	rwf_graphSetColorScale();

	for(i=0; i < 16; i++) {
		// Variable plant & tree colors
		int red   = i*(float)(200 - 100)/16.0 + 100;
		int green = i*(float)(150 - 140)/16.0 + 140;
		int blue  = i*(float)( 90 -  80)/16.0 +  80;
		rwf_graphSetPaletteIndex(i + 16, red, green, blue);
		red   = i*(float)(200 -  60)/16.0 +  60;
		green = i*(float)(170 - 100)/16.0 + 100;
		blue  = i*(float)(130 -  90)/16.0 +  90;
		rwf_graphSetPaletteIndex(i + 32, red, green, blue);
	}

	// Draw the grass
	int ytop = rwf_graphGetMaxY() - 250;
	setfillstyle(SOLID_FILL, 1);
	bar(0, ytop, rwf_graphGetMaxX()-1, rwf_graphGetMaxY()-1);

	// Draw the clouds

	RWFtree_3DCraterObject cloud;

	cloud.setMaximumLevel(4);
	cloud.setMinimumSize(0.01);
	cloud.setColor(15);

	RWFvec_Vector light_vec(3);
	vg.setEyepoint(0.0, 0.0, 100.0);
	light_vec[0] = -1.0;
	light_vec[1] =  1.0;
	light_vec[2] =  1.5;
	vg.setLight(light_vec);
	vg.setLightAttributes(0.6, 0.4);

	rwf_randSrand(818181L);
	float xtrans, ytrans, ztrans;

	for(i=0; i < numclouds; i++) {
		cloud.identity();
		cloud.scale(1500.0, 1500.0, 500.0);
		xtrans = rwf_randRand()*10000 + 8000;
		ytrans = (rwf_randRand()-0.5)*10000;
		ztrans = rwf_randRand()*1000.0 + 1000.0;
		cloud.translate(xtrans, ytrans, ztrans);
		cloud.draw(vg);
	}

	// Now do the background trees
	RWFaf_PolyTree ptree;
	RWFtree_Object ran_tree;

	ptree.setMaximumLevel(5);
	ptree.setMinimumSize(0.01);
	ptree.setAction(&rwf_treeLeafDraw2);
	ran_tree.setMaximumLevel(4);
	ran_tree.setMinimumSize(0.01);
	ran_tree.setAction(&rwf_treeLeafDraw3);
	rwf_randSrand(7272727L);

	xtrans = -1.0;
	ytrans = -0.2;

	int color = 0;
	RWFtree_BranchData btable[30];
	int ipass=0;

	for(;;) {
		RWFmat_MatrixTable mtable;
		RWFaf_UserDataTable udtable;
		int numbranches = rwf_randRand() * 6 + 6;
		mtable.reset();
		udtable.reset();
		for(int branch=0; branch < numbranches; branch++) {
			// Select random branch values
			float scale = -1.0;
			while(scale < 0.01 || scale > 0.9)
				scale = rwf_randRand()*0.4 + 0.2;
			mtable[branch].scale(scale, scale);
			float angle = rwf_randRand()*40.0 + 20.0;
			int iside = (int)(rwf_randRand()*10) % 2;
			if(iside)
				angle = -angle;
			mtable[branch].rotate(angle);
			float offset = rwf_randRand()*0.6 + 0.2;
			mtable[branch].translate(0.0, offset);

			// Now do random tree
			scale = -1.0;
			while(scale < 0.01 || scale > 0.9)
				scale = rwf_randRand()*0.4 + 0.2;
			btable[branch].length.setMean(scale);
			btable[branch].length.setDeviation(scale/3.0);
			btable[branch].angle.setMean(45.0);
			btable[branch].angle.setDeviation(30.0);
			offset = rwf_randRand()*0.6 + 0.1;
			btable[branch].offset.setMean(offset);
			udtable[branch] = &(btable[branch]);
		}

		ptree.setGenerator(mtable);
		color = (color + 1) % 7;
		float yoffset = rwf_randRand()*0.1 - 0.05;
		ptree.setIdentity();
		float tree_scale = 0.3 + rwf_randRand() * 0.1;
		ptree.scale(tree_scale, tree_scale);
		ptree.translate(xtrans, ytrans + yoffset);
		ptree.setColor(color + 2);

		rwf_afSetLastLevel(4);
		ran_tree.setGenerator(udtable);
		int numplants = rwf_randRand()*5 + 2;
		if(ytrans > -0.45)
			numplants += 2;

		for(int plant=0; plant < numplants; plant++) {
			rwf_miscCheckEscape();
			yoffset = rwf_randRand()*0.1 - 0.05;
			float xoffset = rwf_randRand()*0.2 - 0.1;
			ran_tree.setIdentity();
			float scale = tree_scale*(0.18 + (ipass*0.02) + rwf_randRand() * 0.07);
			ran_tree.scale(scale, scale);
			ran_tree.translate(xtrans + xoffset, ytrans + yoffset);
			ran_tree.draw();
		}

		rwf_afSetLastLevel(5);
		rwf_miscCheckEscape();
		if(ytrans > -0.45)
			ptree.fill();

		xtrans += 0.18;
		if(xtrans > 1.0) {
			ytrans -= 0.12;
			if(ytrans < -0.7)
				break;
			xtrans = -1.1 + rwf_randRand()*0.18;
			ipass++;
		}
	}

/*
**	Return
*/
	rwf_miscPressAnyKey();
	rwf_graphClose();
	return 0;
}
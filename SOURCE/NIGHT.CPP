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
** Creates a night scene using the techniques of Chapter 7
*/

#include "rwftree.hpp"
#include "rwfrand.hpp"

#include <string.h>
#include <conio.h>
#include <ctype.h>
#include <stdio.h>

// Some customized action functions
static void rwf_treeRandomLeaf1(RWFgraph_Object *, int);
static void rwf_treeRandomLeaf2(RWFgraph_Object *, int);

int main(int argc,char *argv[])
{
	int i;
	float x,xmin,xmax,ymin,ymax;
	long moon_seed = 7575757L, seed;

/*
**	Read command line options, if any
*/
	i = 1;
	while (i<argc) {
		if (strcmp("-?",argv[(int) i])==0) {
			printf("Usage:  night [-?]\n");
			printf("    -?        :  Display usage information (i.e., this message)\n");
			return 0;
		}
		else {
			printf("***Unrecognized command-line argument \"%s\"\n",argv[(int) i]);
			return 1;
		}
	}

	rwf_miscTitle("Night", 7);

	// Set default graphics window
	rwf_graphInitialize(0);
	x = 1.25 / 2.0;
	xmin = -1.333 * x;
	xmax = 1.333 * x;
	ymin = -1.0 * x;
	ymax = 1.0 * x;
	rwf_graphSetWindow(xmin,ymin,xmax,ymax);
	rwf_randSrand(87878787L);

	// Set bizarre tree colors
	rwf_graphSetPaletteIndex(1, 120, 120, 100);
	rwf_graphSetPaletteIndex(2,  80, 120,  80);
	rwf_graphSetPaletteIndex(3, 100, 120, 120);
	rwf_graphSetPaletteIndex(4, 130, 130, 130);
	rwf_graphSetPaletteIndex(5, 120, 120, 100);
	rwf_graphSetPaletteIndex(6, 120,  90,  90);
//	rwf_graphSetPaletteIndex(7, 160, 100, 100); // Keep gray the same
	rwf_graphSetPaletteIndex(8, 140, 120, 100);
	rwf_graphSetPaletteIndex(9, 110, 130,  90);
	rwf_graphSetPaletteIndex(10, 120, 140, 80);
	rwf_graphSetPaletteIndex(11, 100, 120, 60);
	rwf_graphSetPaletteIndex(12,  90, 110, 80);
	rwf_graphSetPaletteIndex(13, 100, 120, 60);
	rwf_graphSetPaletteIndex(14, 150, 140, 120);

	for(;;) {
		rwf_graphEraseScreen();

		// First, draw the stars
		// This is just a random scattering
		float xsmax = rwf_graphGetMaxX();
		float ysmax = rwf_graphGetMaxY() * 2.0 / 3.0;
		int ix, iy;
		for(i=0; i < 1000; i++) {
			ix = RWFMISC_NINT(rwf_randRand() * xsmax);
			iy = RWFMISC_NINT(rwf_randRand() * ysmax);
			rwf_graphPlot(ix, iy, 15);
		}

		// Draw the grass
		setfillstyle(SOLID_FILL, 2);
		rwf_graphBarDraw(xmin, ymin, xmax, ymin + (ymax - ymin) / 2.5);

		// Now draw the moon
		RWFtree_CraterObject moon;

		moon.setMaximumLevel(4);
		moon.setMinimumSize(0.01);
		moon.scale(0.35, 0.35);
		float xtrans = rwf_randRand() - 0.5;
		float ytrans = 0.25 + rwf_randRand()*0.15;
		moon.translate(xtrans, ytrans);
		seed = rwf_randGetSeed();
		rwf_randSrand(moon_seed);
		moon.fill();
		rwf_randSrand(seed);

		// Now do the forest
		RWFaf_PolyTree ptree;
		RWFtree_Object ran_tree;

		ptree.setMaximumLevel(5);
		ptree.setMinimumSize(0.01);
		ptree.setAction(&rwf_treeRandomLeaf1);
		ran_tree.setMaximumLevel(4);
		ran_tree.setMinimumSize(0.01);
		ran_tree.setAction(&rwf_treeRandomLeaf2);

		xtrans =  xmin;
		ytrans = -0.2;

		// Cycle through colors for trees
		int color = 0, ipass = 0, branch, numbranches;
		float offset, xoffset, yoffset, tree_scale;

		RWFtree_BranchData btable[30];

		long tree_count = 0;
		char string[100];
		string[0] = 0;

		for(;;) {
			RWFmat_MatrixTable mtable;
			RWFaf_UserDataTable udtable;
			int numbranches = rwf_randRand() * 6 + 6;
			rwf_afSetLastLevel(3);
			int numplants = rwf_randRand()*2 + 4;
			if(ytrans > -0.45)
				numplants += 2;

			for(int plant=0; plant < numplants; plant++) {
				rwf_miscCheckEscape();

				// Now do plant parameters
				numbranches = rwf_randRand() * 6 + 4;
				udtable.reset();
				for(branch=0; branch < numbranches; branch++) {
					float scale = -1.0;
					while(scale < 0.01 || scale > 0.9)
						scale = rwf_randRand()*0.3 + 0.1;
					btable[branch].length.setMean(scale);
					btable[branch].length.setDeviation(scale/3.0);
					btable[branch].angle.setMean(45.0);
					btable[branch].angle.setDeviation(30.0);
					offset = rwf_randRand()*0.6 + 0.1;
					btable[branch].offset.setMean(offset);
					udtable[branch] = &(btable[branch]);
				}

				ran_tree.setGenerator(udtable);
				color = (color + 1) % 4;
				ran_tree.setColor(color + 8);
				yoffset = rwf_randRand()*0.1 - 0.05;
				xoffset = rwf_randRand()*0.2 - 0.1;
				ran_tree.setIdentity();
				tree_scale = 0.15 + rwf_randRand() * 0.05;
				ran_tree.scale(tree_scale, tree_scale);
				ran_tree.translate(xtrans + xoffset, ytrans + yoffset);
				ran_tree.draw();
			}

			// Now handle the real trees
			mtable.reset();
			numbranches = rwf_randRand() * 6 + 4;
			for(branch=0; branch < numbranches; branch++) {
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
			}

			ptree.setGenerator(mtable);
			color = (color + 1) % 4;
			yoffset = rwf_randRand()*0.1 - 0.05;
			ptree.setIdentity();
			tree_scale = 0.2 + rwf_randRand() * 0.1;
			ptree.scale(tree_scale, tree_scale);
			ptree.translate(xtrans, ytrans + yoffset);
			ptree.setColor(color + 8);

			rwf_afSetLastLevel(4);
			if(ytrans > -0.45)
				ptree.fill();

			rwf_miscCheckEscape();
			setcolor(2);
			outtextxy(0, rwf_graphGetMaxY() - 20, string);
			tree_count += numplants + 1;
			sprintf(string,"%ld trees drawn",tree_count);
			setcolor(255);
			outtextxy(0, rwf_graphGetMaxY() - 20, string);

			xtrans += 0.15;
			if(xtrans > 1.0) {
				ytrans -= 0.1;
				if(ytrans < -0.6)
					break;
				xtrans = xmin - 0.1 + rwf_randRand()*0.15;
				ipass++;
			}
		}

		rwf_miscPressAnyKey();
	}

/*
**	Return
*/
	rwf_miscPressAnyKey();
	rwf_graphClose();
	return 0;
}

// Specialized action functions for randomizing colors

static RWFgraph_Polygon *leaf = NULL;

static void
rwf_treeRandomLeaf1(RWFgraph_Object *object, int level)
{

	if(!object)
		return;

	if(level < 1)
		return;

	if(level < rwf_afGetLastLevel()) {
		int savecolor = object->getColor();
		object->setColor(savecolor + level);
		object->fill();
		object->setColor(savecolor);
	} else {
		if(!leaf) {
			// Create the definition of the leaf
			leaf = new RWFgraph_Polygon(8);
			(*leaf)[0].setCoord( 0.0, 0.0);
			(*leaf)[1].setCoord(-0.1, 0.15);
			(*leaf)[2].setCoord(-0.075, 0.6);
			(*leaf)[3].setCoord(-0.025, 0.8);
			(*leaf)[4].setCoord( 0.0 , 1.0);
			(*leaf)[5].setCoord( 0.025, 0.8);
			(*leaf)[6].setCoord( 0.075, 0.6);
			(*leaf)[7].setCoord( 0.1 , 0.15);
		}
		leaf->setMatrix(object->getMatrix());
		int color = RWFGRAPH_COLOR_LIGHT_GRAY;
		while(color == RWFGRAPH_COLOR_LIGHT_GRAY)
			color = RWFMISC_NINT(rwf_randRand() * 6.0) + 8;
		leaf->setColor(color);
		leaf->fill();
	}

}

static void
rwf_treeRandomLeaf2(RWFgraph_Object *object, int level)
{

	if(!object)
		return;

	if(level < 1)
		return;

	if(level < rwf_afGetLastLevel()) {
		int savecolor = object->getColor();
		object->setColor(savecolor + level);
		object->fill();
		object->setColor(savecolor);
	} else {
		if(!leaf) {
			// Create the definition of the leaf
			leaf = new RWFgraph_Polygon(8);
			(*leaf)[0].setCoord( 0.0, 0.0);
			(*leaf)[1].setCoord(-0.1, 0.15);
			(*leaf)[2].setCoord(-0.075, 0.6);
			(*leaf)[3].setCoord(-0.025, 0.8);
			(*leaf)[4].setCoord( 0.0 , 1.0);
			(*leaf)[5].setCoord( 0.025, 0.8);
			(*leaf)[6].setCoord( 0.075, 0.6);
			(*leaf)[7].setCoord( 0.1 , 0.15);
		}
		leaf->setMatrix(object->getMatrix());
		int color = RWFMISC_NINT(rwf_randRand() * 13.0) + 1;
		leaf->setColor(color);
		leaf->fill();
	}

}

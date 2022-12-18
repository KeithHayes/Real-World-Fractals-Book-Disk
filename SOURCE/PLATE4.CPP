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

/*
**	Read command line options, if any
*/
	i = 1;
	while (i<argc) {
		if (strcmp("-?",argv[(int) i])==0) {
			printf("Usage:  plate4 [-?]\n");
			printf("    -?        :  Display usage information (i.e., this message)\n");
			return 0;
		}
		else {
			printf("***Unrecognized command-line argument \"%s\"\n",argv[(int) i]);
			return 1;
		}
//		i++;
	}

	rwf_miscTitle("Plate4",7);

	rwf_graphInitialize(0);

	RWFaf_SimpleTree tree;
	RWFaf_PolyTree ptree;
	RWFtree_Object ran_tree;

	tree.setMaximumLevel(5);
	tree.setMinimumSize(0.01);

	ptree.setMaximumLevel(5);
	ptree.setMinimumSize(0.01);
	ptree.setAction(&rwf_treeLeafDraw);

	ran_tree.setMaximumLevel(5);
	ran_tree.setMinimumSize(0.01);

	rwf_afSetLastLevel(5);

	x = (2.7 * 0.8);
	xmin = -1.333 * 0.5 * x;
	xmax = -xmin;
	ymin =  0.0 * x;
	ymax =  1.0 * x;
	rwf_graphSetWindow(xmin, ymin, xmax, ymax);

	// Okay, ready to draw!
	rwf_randSrand(838383L);
	int color = 0, leaf_color = RWFGRAPH_COLOR_GREEN - 1;
	RWFtree_BranchData btable[30];
	int ipass = 0;

	for(;;) {

	// Now do the color plate

	rwf_graphEraseScreen();

	// Scale the plants to the right positions
	tree.identity();
	tree.scale(1/4.0, 1/4.0);
	tree.translate(xmin * (7.0/8.0) + 0.1, 0.0);
	ptree.identity();
	ptree.scale(1/4.0, 1/4.0);
	ptree.translate(xmin * (7.0/8.0) + 0.1, 0.0);
	ran_tree.identity();
	ran_tree.scale(1/4.0, 1/4.0);
	ran_tree.translate(xmin * (7.0/8.0) + 0.1, 0.0);

	for(int ytree=0; ytree < 4; ytree++) {
		for(int xtree=0; xtree < 4; xtree++) {
			float total_length = 1.0;
			RWFmat_MatrixTable mtable;
			RWFaf_UserDataTable udtable;
			int numbranches = rwf_randRand() * 5 + 4;
			mtable.reset();
			udtable.reset();
			for(int branch=0; branch < numbranches; branch++) {
				// Select random branch values
				float scale = -1.0;
				while(scale < 0.2 || scale > 0.9)
					scale = rwf_randGaussian()*0.3 + 0.4;
				mtable[branch].scale(scale, scale);
				btable[branch].length.setMean(scale);
				btable[branch].length.setDeviation(scale/3.0);
				float angle = rwf_randRand()*80.0 - 40.0;
				mtable[branch].rotate(angle);
				float offset = rwf_randRand()*0.6 + 0.2;
				mtable[branch].translate(0.0, offset);
				btable[branch].offset.setMean(offset);
				float new_length =
					scale*cos(angle*RWFMISC_DEGREES_TO_RADIANS) + offset;
				total_length += total_length*RWFMISC_MAX(new_length - 1.0, 0.0);
				udtable[branch] = &(btable[branch]);
			}

			ptree.setGenerator(mtable);
			tree.setGenerator(mtable);
			ran_tree.setGenerator(udtable);
			color = (color + 1) % 7;
			if((xtree + ytree) % 3 != 1) {
				ptree.setColor(color + 9);
				rwf_treeSetLeafColor(leaf_color + 1);
				ptree.fill();
			} else if ((xtree + ytree) % 3 == 1) {
				ran_tree.setColor(color + 9);
				ran_tree.draw();
			} else {
				tree.setColor(color + 9);
				tree.draw();
			}
			leaf_color = (leaf_color + 2) % 14;
			if(leaf_color == 7) leaf_color += 2;

			rwf_miscCheckEscape();

			if(xtree < 3) {
				tree.translate((xmax - xmin)*1/4.0, 0.0);
				ptree.translate((xmax - xmin)*1/4.0, 0.0);
				ran_tree.translate((xmax - xmin)*1/4.0, 0.0);
			} else {
				tree.translate( -(xmax - xmin)*3/4.0, (ymax - ymin) * 1/4.0);
				ptree.translate(-(xmax - xmin)*3/4.0, (ymax - ymin) * 1/4.0);
				ran_tree.translate(-(xmax-xmin)*3/4.0, (ymax - ymin) * 1/4.0);
			}
		}
	}

	ipass++;
	if(ipass > 100)
		break;

	rwf_miscPressAnyKey();
	}

/*
**	Return
*/
	rwf_miscPressAnyKey();
	rwf_graphClose();
	return 0;
}
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

#include "rwfgrid.hpp"
#include "rwfrand.hpp"
#include "rwftree.hpp"

#include <string.h>
#include <conio.h>
#include <ctype.h>
#include <stdio.h>

// Special branching function for pine trees
int rwf_tree3DFindPineMatrix(void *data, int level, RWFmat_Matrix &);

int main(int argc,char *argv[])
{
	int i;
	float x,xmin,xmax,ymin,ymax;
	RWFgraph_ViewingGeometry vg;
	char string[100];

	float avg_tree_height = 300.0;
	float tree_deviation  = 25.0;
	float tree_spacing    = 180.0;
	int   numclouds       = 15;
	int   max_terrain_level = 4;

/*
**	Read command line options, if any
*/
	i = 1;
	while (i<argc) {
		if (strcmp("-?",argv[(int) i])==0) {
			printf("Usage:  mountree [-?]\n");
			printf("    -?           :  Display usage information (i.e., this message)\n");
			return 0;
		} else {
			printf("***Unrecognized command-line argument \"%s\"\n",argv[(int) i]);
			return 1;
		}
//		i++;
	}

	string[0] = 0;

	rwf_miscTitle("MounTree", 11);

	rwf_graphInitialize(0);

	// Sky color
	rwf_graphSetPaletteIndex(0, 130, 140, 180);

	RWFlist_ClassTable ctable;

	// 4 basic classes

	// 0 is water
	ctable[0].setStandardDeviation(0.03);
	ctable[0].setFractalDimension(2.0);
	ctable[0].setColor(1);	// Water color
	rwf_graphSetPaletteIndex(1, 110, 110, 140);

	// 1 is bushes & shrubs
	ctable[1].setStandardDeviation(0.2);
	ctable[1].setFractalDimension(0.5);
	ctable[1].setColor(2);	// Green
	rwf_graphSetPaletteIndex(2, 100, 130,  90);

	// 2 is barren trees
	ctable[2].setStandardDeviation(0.04);
	ctable[2].setFractalDimension(1.0);
	ctable[2].setColor(3);	// Brown
//	rwf_graphSetPaletteIndex(3, 150, 120, 100);
	rwf_graphSetPaletteIndex(3, 140, 140, 140);

	// 3 is snow cap
	ctable[3].setStandardDeviation(0.04);
	ctable[3].setFractalDimension(1.5);
	ctable[3].setColor(4);	// White
	rwf_graphSetPaletteIndex(4, 220, 220, 220);

	// Setup tree colors
	rwf_graphSetPaletteIndex(8, 140, 120, 100);
	rwf_graphSetPaletteIndex(9, 130, 110,  90);
	rwf_graphSetPaletteIndex(10, 120, 100, 80);
	rwf_graphSetPaletteIndex(11, 100,  90, 60);
	rwf_graphSetPaletteIndex(12,  90, 110, 80);
	rwf_graphSetPaletteIndex(13, 100, 120, 60);
	rwf_graphSetPaletteIndex(14, 150, 140, 120);

	rwf_graphSetColorScale();

	// Draw the terrain
	RWFgraph_Grid terrain(32, 32);

	terrain.setClassTable(ctable);
	float cell_size = 200.0;
	terrain.setPixelSize(cell_size, cell_size);

	// Find the midpoint of the grid

	float xmid = terrain.ncols() / 10.0;
	float ymid = terrain.nrows() / 10.0;
	float sigma2 = (terrain.ncols() * terrain.nrows()) / 10.0;

	float height;

	for(int j=0; j < terrain.nrows(); j++) {
		for(i=0; i < terrain.ncols(); i++) {
			float r2 = ((i-xmid)*(i-xmid) + (j-ymid)*(j-ymid));
			terrain[i][j].h = height = 10.0*exp(-(r2 / sigma2));
			if(height > 8.0)
				terrain[i][j].class_value = 3;
			else if (height > 5.0)
				terrain[i][j].class_value = 2;
			else if (height > 0.5)
				terrain[i][j].class_value = 1;
			else
				terrain[i][j].class_value = 0;
		}
	}

	terrain.setColor(0);
	float xdist = ((terrain.ncols()-1) * cell_size) / 2.0;
	float ydist = ((terrain.nrows()-1) * cell_size) / 2.0;

	RWFvec_Vector light_vec(3);
	vg.setEyepoint(0.0, 0.0, 100.0);
	light_vec[0] = -1.0;
	light_vec[1] =  1.0;
	light_vec[2] =  1.5;
	vg.setLight(light_vec);
	vg.setLightAttributes(0.6, 0.4);

	rwf_graphEraseScreen();

	// Draw the clouds
	RWFtree_3DCraterObject cloud;
	cloud.setMaximumLevel(4);
	cloud.setMinimumSize(0.01);
	cloud.setColor(15);

	float xtrans, ytrans, ztrans;

	for(i=0; i < numclouds; i++) {
		cloud.identity();
		cloud.scale(1500.0, 1500.0, 500.0);
		xtrans = rwf_randRand()*xdist*6 + xdist;
		ytrans = (rwf_randRand()-0.5)*ydist*4;
		ztrans = rwf_randRand()*1000.0 + 1000.0;
		cloud.translate(xtrans, ytrans, ztrans);
		cloud.draw(vg);
	}

	// Now draw the terrain

	light_vec[0] = -1.0;
	light_vec[1] =  1.0;
	light_vec[2] =  0.5;
	vg.setLight(light_vec);
	vg.setLightAttributes(0.4, 0.6);

	terrain.identity();
	terrain.scale(1.0, 1.0, 150.0);
	terrain.rotate(0.0, 0.0, 135.0);
	terrain.rotate(0.0, 10.0, 0.0);
	terrain.translate(xdist * sqrt(2.0) * 2.1, 0.0, 0.0);
	long numpolys = terrain.ncols() * terrain.nrows() * 2;
	for(int level=0; level <= max_terrain_level; level++) {
		terrain.setMaximumRecursion(level);
		setcolor(0);
		outtextxy(10, 20, string);
		sprintf(string,"Terrain recursion level %d of %d, Number of Polygons = %ld",
									 level, max_terrain_level, numpolys);
		setcolor(255);
		outtextxy(10, 20, string);
		numpolys *= 4;
		terrain.draw(vg);
	}

	outtextxy(10, 20, string);

	// Ok, draw the trees
	RWFtree_3DObject tree;
	RWFtree_BranchData btable[30];
	RWFaf_UserDataTable udtable;

	tree.setMaximumLevel(3);
	tree.setMinimumSize(0.01);
	tree.setColor(8);
	tree.setMatrixFunction(rwf_tree3DFindPineMatrix);

	rwf_afSetLastLevel(3);
	light_vec[0] = -1.0;
	light_vec[1] = 0.0;
	light_vec[2] = 0.0;
	vg.setLight(light_vec);
	vg.setLightAttributes(0.6, 0.4);

	// Set the branching attributes for pine trees
	for(int branch=0; branch < 30; branch++) {
		btable[branch].id = branch;
		btable[branch].length.setMean(0.5);
		btable[branch].length.setDeviation(0.1);
		btable[branch].angle.setMean(25.0);
		btable[branch].angle.setDeviation(3.0);
		btable[branch].offset.setMean(0.9);
		btable[branch].offset.setDeviation(0.1);
		udtable[branch] = &(btable[branch]);
	}
	tree.setGenerator(udtable);

	for(float yoffset = 0.0; yoffset < ydist*2.0; yoffset += tree_spacing) {
		float xstart = rwf_randRand()*tree_spacing/2.0;
		for(float xoffset = xstart; xoffset < xdist*2.0; xoffset += tree_spacing) {
			rwf_miscCheckEscape();

			// Find the terrain cell
			i = RWFMISC_NINT(xoffset / cell_size);
			j = RWFMISC_NINT(yoffset / cell_size);
			int iclass = terrain[i][j].class_value;
			if(iclass < 1 || iclass > 2)
				 continue;

			// Now do tree parameters
			float tree_scale = avg_tree_height + rwf_randGaussian() * tree_deviation;
			tree.identity();
			tree.scale(tree_scale, tree_scale, tree_scale);
			height = terrain[i][j].h * 150.0;
			tree.translate(xoffset, yoffset, height);
			tree.rotate(0.0, 0.0, 135.0);
			tree.rotate(0.0, 10.0, 0.0);
			tree.translate(xdist * sqrt(2.0) * 2.1, 0.0, 0.0);
			tree.draw(vg);
		}
	}

	rwf_miscPressAnyKey();

/*
**	Return
*/
	rwf_miscPressAnyKey();
	rwf_graphClose();
	return 0;
}

int
rwf_tree3DFindPineMatrix(void *data, int level, RWFmat_Matrix &bmatrix)
{
	RWFtree_BranchData *branch = (RWFtree_BranchData *) data;
	float factor;

	// Number of branches is a function of the level
	if(level == 1) {
		if(branch->id > 10)
			return 0;
	} else if (level == rwf_afGetLastLevel() - 1) {
		if(branch->id > 10)
    	return 0;
	} else {
		if(branch->id > 5)
			return 0;
	}

	bmatrix.identity();
	factor = branch->length.next();
	// Scale only in the z-direction
	if(level == rwf_afGetLastLevel()-1)
		factor = factor / 3.0;
	bmatrix.scale(factor, factor, factor);

	// Add an initial orientation matrix
	factor = branch->side.next();
	bmatrix.rotate(0.0, 0.0, factor * 180.0);

	factor = branch->angle.next();
	// First, rotate about the y-axis
	if(level == rwf_afGetLastLevel()-1) {
		// Make the leaves perpendicular to the branch
		factor = 70.0;
	} else if (level == 1) {
		// Simulate pine trees, make first level branch point down
		factor = 180.0 - factor;
	}
	bmatrix.rotate(0.0, factor, 0.0);
	// Use the side paramter to determine a rotation
	// angle about the z-axis
	// Now, rotate about the z-axis
	factor = branch->side.next();
	bmatrix.rotate(0.0, 0.0, factor * 180.0);
	// Finally, translate the branch along the z-axis
	factor = RWFMISC_MAX(RWFMISC_MIN(branch->offset.next(), 1.0), 0.0);

	bmatrix.translate(0.0, 0.0, factor);

	return 1;
}


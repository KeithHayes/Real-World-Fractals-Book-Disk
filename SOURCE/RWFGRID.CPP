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

#include "rwfgrid.hpp"
#include "rwffbm.hpp"

RWFgraph_Grid::RWFgraph_Grid(void)
{
	data = NULL;
	zfbm = NULL;
	class_fbm = NULL;
	numRows = 0;
	numCols = 0;
	x_cell_size = 1.0;
	y_cell_size = 1.0;
	max_recursion = 5;
	nlevels = -1;
	setColor(255);
	make3D();
}

RWFgraph_Grid::RWFgraph_Grid(int nx, int ny)
{
	data = new RWFgraph_GridElement * [ny];
	for(int i=0; i < ny; i++)
		data[i] = new RWFgraph_GridElement [nx];

	zfbm = NULL;
	class_fbm = NULL;
	max_recursion = 5;
	nlevels = -1;

	numRows = ny;
	numCols = nx;

	x_cell_size = 1.0;
	y_cell_size = 1.0;

	make3D();
	setColor(255);

	return;
}

RWFgraph_Grid::~RWFgraph_Grid(void)
{
	if(data) {
		for(int i=0; i < numRows; i++)
			delete data[i];
		delete data;
	}

	data = NULL;
	numRows = 0;
	numCols = 0;

	if(zfbm) {
		int nsubgrid = 1 << nlevels;
		for(int i=0; i < nsubgrid+1; i++) {
			delete zfbm[i];
			delete class_fbm[i];
		}
		delete zfbm;
		delete class_fbm;
	}
	zfbm = NULL;
	class_fbm = NULL;

	return;
}

#define BIG_CONSTANT 1010101

/*
This is the main drawing function for grids

This version incorporates 2D fBM for both the z values
and the class values

Note that instead of having 8 separate cases for the scanning orders,
draw() has two cases, row order and column order.  The top_to_bottom
and left_to_right flags are used to set increments both horizontally
and vertically through the grid.
*/

// Specialized fbm routine that lets you pass all class information
// Instead of passing individual parameters

void rwf_fbm2DGenerate(float corners[], long seed_corners[],
											 int class_corners[], RWFlist_ClassTable &ctable,
											 float (*random_func)(void), int nlevels,float **z);

void
RWFgraph_Grid::draw(RWFgraph_ViewingGeometry &vg)
{
	RWFgraph_Polygon triangle(3);
	float x1, y1, x2, y2, z11, z12, z21, z22;
	float zcorners[4];
	long  seed_corners[4];
	int   class_corners[4], class_choice;
	float xsub1, xsub2, ysub1, ysub2;

	int numrows = nrows();
	int numcols = ncols();

	triangle.setMatrix(getMatrix());

	// Now simply loop through all of the elements
	// of the grid.

	int column_order, left_to_right, top_to_bottom;
	findScanningOrder(vg, left_to_right, top_to_bottom, column_order);

	int zindex_y, zindex_x;
	int zindex_suby, zindex_subx;

	setRecursionLevel(vg);

	if(!column_order) {
		for(int j=0; j < numrows-1; j++) {
			if(top_to_bottom) {
				y2 = (numrows - j - 1)*y_cell_size;
				y1 = y2 - y_cell_size;
				zindex_y = numrows - 1 - j - 1;
			} else {
				y1 = j*y_cell_size;
				y2 = y1 + y_cell_size;
				zindex_y = j;
			}
			for(int i=0; i < numcols-1; i++) {
				if(!left_to_right) {
					x2 = (numcols - i - 1)*x_cell_size;
					x1 = x2 - x_cell_size;
					zindex_x = numcols - 1 - i - 1;
				} else {
					x1 = i*x_cell_size;
					x2 = x1 + x_cell_size;
					zindex_x = i;
				}

				// See if they want to stop
				rwf_miscCheckEscape();

				// Since we have combined the scanning orders
				// into 2 cases, we use the variable
				// zindex_x and zindex_y to select
				// the appropriate values.  These indices always
				// let us scan left-to-right & top-to-bottom
				// as indicated

				zcorners[0] = ((*this)[zindex_x][zindex_y]).h;
				zcorners[1] = ((*this)[zindex_x+1][zindex_y]).h;
				zcorners[2] = ((*this)[zindex_x][zindex_y+1]).h;
				zcorners[3] = ((*this)[zindex_x+1][zindex_y+1]).h;

				class_corners[0] = ((*this)[zindex_x][zindex_y]).class_value;
				class_corners[1] = ((*this)[zindex_x+1][zindex_y]).class_value;
				class_corners[2] = ((*this)[zindex_x][zindex_y+1]).class_value;
				class_corners[3] = ((*this)[zindex_x+1][zindex_y+1]).class_value;

				// Now compute the seeds at the corners
				// You can use any method you like as long as
				// it is consistent.  In this case, we simply
				// compute (j+1)*BIG_CONSTANT + i
				seed_corners[0] = zindex_y * BIG_CONSTANT + zindex_x;
				seed_corners[1] = seed_corners[0] + 1;
				seed_corners[2] = seed_corners[0] + BIG_CONSTANT;
				seed_corners[3] = seed_corners[2] + 1;

				int type;
				rwf_fbm2DGenerate(zcorners, seed_corners, class_corners,
													classes, NULL, nlevels, zfbm);
				rwf_fbm2DDiscreteGenerate(class_corners, seed_corners, NULL,
																	nlevels, class_fbm);

				// Ok, now process the sub-grid
				int nsubgrid = 1 << nlevels;
				float y_subcell = y_cell_size / (float)nsubgrid;
				float x_subcell = x_cell_size / (float)nsubgrid;

				for(int k=0; k < nsubgrid; k++) {
					if(top_to_bottom) {
						ysub2 = y2 - k*y_subcell;
						ysub1 = ysub2 - y_subcell;
						zindex_suby = nsubgrid - k - 1;
					} else {
						ysub1 = y1 + k*y_subcell;
						ysub2 = ysub1 + y_subcell;
						zindex_suby = k;
					}

					for(int l=0; l < nsubgrid; l++) {
						if(!left_to_right) {
							xsub2 = x2 - l*x_subcell;
							xsub1 = xsub2 - x_subcell;
							zindex_subx = nsubgrid - l - 1;
						} else {
							xsub1 = x1 + l*x_subcell;
							xsub2 = xsub1 + x_subcell;
							zindex_subx = l;
						}

						// Fetch the z values
						// Since we have combined the scanning orders
						// into 2 cases, we use the variable
						// zindex_subx and zindex_suby to select
						// the appropriate values.
						z11 = zfbm[zindex_subx][zindex_suby];
						z12 = zfbm[zindex_subx][zindex_suby+1];
						z21 = zfbm[zindex_subx+1][zindex_suby];
						z22 = zfbm[zindex_subx+1][zindex_suby+1];

						// Find the class for this square
						type = class_fbm[zindex_subx][zindex_suby];
						triangle.setColor(classes[type].color);

						// Construct first triangle
						triangle[0].setCoord(xsub1, ysub1, z11);
						triangle[1].setCoord(xsub2, ysub1, z21);
						triangle[2].setCoord(xsub2, ysub2, z22);
						triangle.draw(vg);

						// Construct second triangle
						triangle[0].setCoord(xsub1, ysub1, z11);
						triangle[1].setCoord(xsub2, ysub2, z22);
						triangle[2].setCoord(xsub1, ysub2, z12);
						triangle.draw(vg);

					} // End of x subloop
				} // End of y subloop
			} // End of major x loop
		} // End of major y loop

	} else {	// Column order

		for(int i=0; i < numcols-1; i++) {
			if(!left_to_right) {
				x2 = (numcols - i - 1)*x_cell_size;
				x1 = x2 - x_cell_size;
				zindex_x = numcols - 1 - i - 1;
			} else {
				x1 = i*x_cell_size;
				x2 = x1 + x_cell_size;
				zindex_x = i;
			}

			for(int j=0; j < numrows-1; j++) {
				if(top_to_bottom) {
					y2 = (numrows - j - 1)*y_cell_size;
					y1 = y2 - y_cell_size;
					zindex_y = numrows - 1 - j - 1;
				} else {
					y1 = j*y_cell_size;
					y2 = y1 + y_cell_size;
					zindex_y = j;
				}

				// See if they want to stop
				rwf_miscCheckEscape();

				zcorners[0] = ((*this)[zindex_x][zindex_y]).h;
				zcorners[1] = ((*this)[zindex_x+1][zindex_y]).h;
				zcorners[2] = ((*this)[zindex_x][zindex_y+1]).h;
				zcorners[3] = ((*this)[zindex_x+1][zindex_y+1]).h;

				class_corners[0] = ((*this)[zindex_x][zindex_y]).class_value;
				class_corners[1] = ((*this)[zindex_x+1][zindex_y]).class_value;
				class_corners[2] = ((*this)[zindex_x][zindex_y+1]).class_value;
				class_corners[3] = ((*this)[zindex_x+1][zindex_y+1]).class_value;

				// Now compute the seeds at the corners
				// You can use any method you like as long as
				// it is consistent.  In this case, we simply
				// compute (j+1)*BIG_CONSTANT + i
				seed_corners[0] = zindex_y * BIG_CONSTANT + zindex_x;
				seed_corners[1] = seed_corners[0] + 1;
				seed_corners[2] = seed_corners[0] + BIG_CONSTANT;
				seed_corners[3] = seed_corners[2] + 1;

				int type;
				rwf_fbm2DGenerate(zcorners, seed_corners, class_corners,
													classes, NULL, nlevels, zfbm);
				rwf_fbm2DDiscreteGenerate(class_corners, seed_corners, NULL,
																	nlevels, class_fbm);

				// Ok, now process the sub-grid
				int nsubgrid = 1 << nlevels;
				float y_subcell = y_cell_size / (float)nsubgrid;
				float x_subcell = x_cell_size / (float)nsubgrid;

				for(int l=0; l < nsubgrid; l++) {
					if(!left_to_right) {
						xsub2 = x2 - l*x_subcell;
						xsub1 = xsub2 - x_subcell;
						zindex_subx = nsubgrid - l - 1;
					} else {
						xsub1 = x1 + l*x_subcell;
						xsub2 = xsub1 + x_subcell;
						zindex_subx = l;
					}

					for(int k=0; k < nsubgrid; k++) {
						if(top_to_bottom) {
							ysub2 = y2 - k*y_subcell;
							ysub1 = ysub2 - y_subcell;
							zindex_suby = nsubgrid - k - 1;
						} else {
							ysub1 = y1 + k*y_subcell;
							ysub2 = ysub1 + y_subcell;
							zindex_suby = k;
						}

						// Fetch the z values
						z11 = zfbm[zindex_subx][zindex_suby];
						z12 = zfbm[zindex_subx][zindex_suby+1];
						z21 = zfbm[zindex_subx+1][zindex_suby];
						z22 = zfbm[zindex_subx+1][zindex_suby+1];

						// Find the class for this square
						type = class_fbm[zindex_subx][zindex_suby];
						triangle.setColor(classes[type].color);

						// Construct first triangle
						triangle[0].setCoord(xsub1, ysub1, z11);
						triangle[1].setCoord(xsub2, ysub1, z21);
						triangle[2].setCoord(xsub2, ysub2, z22);
						triangle.draw(vg);

						// Construct second triangle
						triangle[0].setCoord(xsub1, ysub1, z11);
						triangle[1].setCoord(xsub2, ysub2, z22);
						triangle[2].setCoord(xsub1, ysub2, z12);
						triangle.draw(vg);

					} // End of y subloop
				} // End of x subloop

			} // End of x loop
		} // End of y loop

	}

	return;
}

void
RWFgraph_Grid::findScanningOrder(RWFgraph_ViewingGeometry &vg,
	int &left_to_right, int &top_to_bottom, int &column_order)
{
	RWFvec_Vector center(2), eyevec(2), tvec(2), tvec2(2);
	RWFmat_Matrix mat22(2, 2), mat33(3,3);

	eyevec[0] = vg.getEyepoint()[0];
	eyevec[1] = vg.getEyepoint()[1];
	float xmid = (ncols()-1)*x_cell_size / 2.0;
	float ymid = (nrows()-1)*y_cell_size / 2.0;

	mat33 = getMatrix();
	mat22[0][0] = mat33[0][0];
	mat22[0][1] = mat33[0][1];
	mat22[0][2] = mat33[0][3];
	mat22[1][0] = mat33[1][0];
	mat22[1][1] = mat33[1][1];
	mat22[1][2] = mat33[1][3];

	center[0] = xmid;
	center[1] = ymid;
	center *= mat22;

	tvec[0] = 0.0;
	tvec[1] = ymid;
	tvec *= mat22;
	tvec = tvec - center;
	eyevec = eyevec - center;

	if((tvec * eyevec) > 0.0)
		left_to_right = 0;
	else
		left_to_right = 1;

	tvec[0] = xmid;
	tvec[1] = 0.0;
	tvec *= mat22;
	tvec = tvec - center;

	if((tvec * eyevec) > 0.0)
		top_to_bottom = 1;
	else
		top_to_bottom = 0;

	tvec[0] = 0.0;
	tvec[1] = 0.0;
	tvec *= mat22;
	tvec = tvec - center;
	tvec2[0] = 0.0;
	tvec2[1] = ymid * 2.0;
	tvec2 *= mat22;
	tvec2 = tvec2 - center;

	column_order = 0;
	if((tvec * eyevec) > 0.0) {
		if((tvec2 * eyevec) > 0.0)
			column_order = 1;
	} else {
		if((tvec2 * eyevec) <= 0.0)
			column_order = 1;
	}

	return;
}

void
RWFgraph_Grid::setRecursionLevel(RWFgraph_ViewingGeometry &vg)
{
	int new_levels, nsubgrid;

	new_levels = max_recursion;

	if(new_levels == nlevels)
		return;

	if(zfbm) {
		nsubgrid = 1 << nlevels;
		for(int row=0; row < nsubgrid+1; row++) {
			delete zfbm[row];
			delete class_fbm[row];
		}
		delete zfbm;
		delete class_fbm;
	}

	nlevels = new_levels;

	// Create the fbm array
	nsubgrid = 1 << nlevels;
	zfbm = new float * [nsubgrid + 1];
	class_fbm = new int * [nsubgrid + 1];
	for(int row=0; row < nsubgrid + 1; row++) {
		zfbm[row] = new float [nsubgrid + 1];
		class_fbm[row] = new int [nsubgrid + 1];
	}

	return;
}

/*
Specialized fbm routine that lets you pass all class information
Instead of passing individual parameters.  This is identical to
the other versions of rwf_fbm2DGenerate, except a class table is passed.
This allows for the selection of a consistent level_scale and
standard deviation along the 4 edges where 1D fBM is used.  You only need
this version when there are different classes at the 4 corners, with
possibly different values of level_scale and standard deviation.
*/

void rwf_fbm2DGenerate(float corners[], long seed_corners[],
											 int class_corners[], RWFlist_ClassTable &ctable,
											 float (*random_func)(void), int nlevels,float **z)
{
	int i,npoints;
	long seed;
	float stdev, level_scale;
  float scale1, scale2, stdev1, stdev2;

	if (nlevels==0) {
		//	OK, fill in the corners
		z[0][0] = corners[0];
		z[1][0] = corners[1];
		z[0][1] = corners[2];
		z[1][1] = corners[3];
		return;
	}

	if(!random_func)
		random_func = rwf_randGaussian;

	npoints = (1 << nlevels);
	float *temp_z = new float [npoints+1];
	//	First, take care of the outside edges

	//	Columns are easiest
	//	First column

	seed = seed_corners[0] + seed_corners[2];
	scale1 = ctable[class_corners[0]].level_scale;
	scale2 = ctable[class_corners[2]].level_scale;
	level_scale = RWFMISC_MAX(scale1, scale2);
	stdev1 = ctable[class_corners[0]].stdev;
	stdev2 = ctable[class_corners[2]].stdev;
	stdev = RWFMISC_MIN(stdev1, stdev2);

	rwf_randSrand(seed);
	rwf_fbm1DGenerate(corners[0],corners[2],random_func,stdev,level_scale,
		nlevels,&(z[0][0]));

	//	Last column
	seed = seed_corners[1] + seed_corners[3];
	scale1 = ctable[class_corners[1]].level_scale;
	scale2 = ctable[class_corners[3]].level_scale;
	level_scale = RWFMISC_MAX(scale1, scale2);
	stdev1 = ctable[class_corners[1]].stdev;
	stdev2 = ctable[class_corners[3]].stdev;
	stdev = RWFMISC_MIN(stdev1, stdev2);

	rwf_randSrand(seed);
	rwf_fbm1DGenerate(corners[1],corners[3],random_func,stdev,level_scale,
		nlevels,&(z[npoints][0]));

	//	Filling the top and bottom edges requires using
	//	the temporary array
	//	Bottom edge
	seed = seed_corners[0] + seed_corners[1];
	scale1 = ctable[class_corners[0]].level_scale;
	scale2 = ctable[class_corners[1]].level_scale;
	level_scale = RWFMISC_MAX(scale1, scale2);
	stdev1 = ctable[class_corners[0]].stdev;
	stdev2 = ctable[class_corners[1]].stdev;
	stdev = RWFMISC_MIN(stdev1, stdev2);

	rwf_randSrand(seed);
	rwf_fbm1DGenerate(corners[0],corners[1],random_func,stdev,level_scale,
		nlevels,temp_z);
	for (i=1; i<npoints; i++) z[i][0] = temp_z[i];

	//	Top edge
	seed = seed_corners[2] + seed_corners[3];
	scale1 = ctable[class_corners[2]].level_scale;
	scale2 = ctable[class_corners[3]].level_scale;
	level_scale = RWFMISC_MAX(scale1, scale2);
	stdev1 = ctable[class_corners[2]].stdev;
	stdev2 = ctable[class_corners[3]].stdev;
	stdev = RWFMISC_MIN(stdev1, stdev2);

	rwf_randSrand(seed);
	rwf_fbm1DGenerate(corners[2],corners[3],random_func,stdev,level_scale,
		nlevels,temp_z);
	for (i=1; i<npoints; i++) z[i][npoints] = temp_z[i];

	delete temp_z;

	//	Now fill in the rest of the array

	//	Loop over recursion levels
	seed = seed_corners[0] + seed_corners[1] + seed_corners[2] + seed_corners[3];
	level_scale = ctable[class_corners[0]].level_scale;
	stdev = ctable[class_corners[0]].stdev;
	rwf_randSrand(seed);

	int nhalf = npoints;
	float cur_stdev = stdev;
	int x,y;
	for (int level=0; level<nlevels; level++) {
		int nacross = nhalf;
		nhalf = nhalf / 2;
		//	Compute all of the midpoints for this level
		//	These are the points at the centers of the
		//	new square
		for (x=nhalf; x<npoints; x+=nacross) {
			for (y=nhalf; y<npoints; y+=nacross) {
				z[x][y] = (z[x-nhalf][y-nhalf] +
					z[x-nhalf][y+nhalf] +
					z[x+nhalf][y-nhalf] +
					z[x+nhalf][y+nhalf]) / 4.0;
				z[x][y] += cur_stdev * (*random_func)();
			}
		}
		//	Okay, now compute midpoints of subdivided edges
		//	These points correspond to the circled points
		//	of figure 9-7.
		if (level>0) {
			//	Must do points along x and y separately
			int xs = nhalf;
			int ys = nacross;
			for (x=xs; x<npoints; x+=nacross) {
				for (y=ys; y<npoints; y+=nacross) {
					z[x][y] = (z[x][y-xs] +
						z[x][y+xs] +
						z[x-xs][y] +
						z[x+xs][y]) / 4.0;
					z[x][y] += cur_stdev * (*random_func)();
				}
			}

			xs = nacross;
			ys = nhalf;
			for (x=xs; x<npoints; x+=nacross) {
				for (y=ys; y<npoints; y+=nacross) {
					z[x][y] = (z[x][y-ys] +
						z[x][y+ys] +
						z[x-ys][y] +
						z[x+ys][y]) / 4.0;
					z[x][y] += cur_stdev * (*random_func)();
				}
			}
		}	//	Okay, finished with the other midpoints

		//	Update the standard deviation
		cur_stdev *= level_scale;
	}	//	Finished looping over all recursions

	return;
}


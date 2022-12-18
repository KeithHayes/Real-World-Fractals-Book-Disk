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

#include "rwffbm.hpp"

/*
**	Basic fbm functions
*/

void rwf_fbm1DGenerate(float y1,float y2,float (*random_func)(void),
	float stdev,float level_scale,int nlevels,float *values)
{
	if (nlevels==0) {
		//	Only the endpoints are set
		values[0] = y1;
		values[1] = y2;
		return;
	}

	//	OK, compute the midpoint
	float ymid = (y1 + y2) / 2.0;
	if (random_func) {
		ymid += stdev * (*random_func)();
	}
	else {
		ymid += stdev * rwf_randGaussian();
	}

	if (nlevels==1) {
		//	In this case, we can finish directly since
		//	there are only 3 points to process
		values[0] = y1;
		values[1] = ymid;
		values[2] = y2;
		return;
	}

	float new_stdev = stdev * level_scale;
	//	Number of points is 2**nlevels + 1
	int npoints = (1 << nlevels) + 1;
	//	values[nhalf] starts the second half of the array
	int nhalf = npoints/2;

	//	OK, now process the first half
	rwf_fbm1DGenerate(y1,ymid,random_func,new_stdev,level_scale,
		nlevels-1,&(values[0]));

	//	Now do the second half
	//	The trick here is to pass the address of the second
	//	half of the data array
	rwf_fbm1DGenerate(ymid,y2,random_func,new_stdev,level_scale,
		nlevels-1,&(values[nhalf]));

	return;
}

void rwf_fbm2DGenerate(float corners[],float (*random_func)(void),
	float stdev,float level_scale,int nlevels,float **z)
{
	int i,npoints;

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
	rwf_fbm1DGenerate(corners[0],corners[2],random_func,stdev,level_scale,
		nlevels,&(z[0][0]));

	//	Last column
	rwf_fbm1DGenerate(corners[1],corners[3],random_func,stdev,level_scale,
		nlevels,&(z[npoints][0]));

	//	Filling the top and bottom edges requires using
	//	the temporary array
	//	Bottom edge
	rwf_fbm1DGenerate(corners[0],corners[1],random_func,stdev,level_scale,
		nlevels,temp_z);
	for (i=1; i<npoints; i++) z[i][0] = temp_z[i];

	//	Top edge
	rwf_fbm1DGenerate(corners[2],corners[3],random_func,stdev,level_scale,
		nlevels,temp_z);
	for (i=1; i<npoints; i++) z[i][npoints] = temp_z[i];

	delete temp_z;

	//	Now fill in the rest of the array

	//	Loop over recursion levels
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

void rwf_fbm2DGenerate(float corners[], long seed_corners[],
											 float (*random_func)(void),
											 float stdev,float level_scale,int nlevels,float **z)
{
	int i,npoints;
	long seed;

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
	rwf_randSrand(seed);
	rwf_fbm1DGenerate(corners[0],corners[2],random_func,stdev,level_scale,
		nlevels,&(z[0][0]));

	//	Last column
	seed = seed_corners[1] + seed_corners[3];
	rwf_randSrand(seed);
	rwf_fbm1DGenerate(corners[1],corners[3],random_func,stdev,level_scale,
		nlevels,&(z[npoints][0]));

	//	Filling the top and bottom edges requires using
	//	the temporary array
	//	Bottom edge
	seed = seed_corners[0] + seed_corners[1];
	rwf_randSrand(seed);
	rwf_fbm1DGenerate(corners[0],corners[1],random_func,stdev,level_scale,
		nlevels,temp_z);
	for (i=1; i<npoints; i++) z[i][0] = temp_z[i];

	//	Top edge
	seed = seed_corners[2] + seed_corners[3];
	rwf_randSrand(seed);
	rwf_fbm1DGenerate(corners[2],corners[3],random_func,stdev,level_scale,
		nlevels,temp_z);
	for (i=1; i<npoints; i++) z[i][npoints] = temp_z[i];

	delete temp_z;

	//	Now fill in the rest of the array

	//	Loop over recursion levels
	seed = seed_corners[0] + seed_corners[1] + seed_corners[2] + seed_corners[3];
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


void rwf_fbm1DDiscreteGenerate(int class1,int class2,
	float (*random_func)(void),int nlevels,int *values)
{
	int classmid;
	float x;

	if (nlevels==0) {
		//	Only the endpoints are set
		values[0] = class1;
		values[1] = class2;
		return;
	}

	//	OK, compute the midpoint
	if (random_func) {
		x = (*random_func)();
	}
	else {
		x = rwf_randGaussian();
	}

	if (x>=0.0) {
		classmid = class1;
	}
	else {
		classmid = class2;
	}

	if (nlevels==1) {
		//	In this case, we can finish directly since
		//	there are only 3 points to process
		values[0] = class1;
		values[1] = classmid;
		values[2] = class2;
		return;
	}

	//	Number of points is 2**nlevels + 1
	int npoints = (1 << nlevels) + 1;
	//	values[nhalf] starts the second half of the array
	int nhalf = npoints/2;

	//	OK, now process the first half
	rwf_fbm1DDiscreteGenerate(class1,classmid,random_func,nlevels-1,&(values[0]));

	//	Now do the second half
	//	The trick here is to pass the address of the second
	//	half of the data array
	rwf_fbm1DDiscreteGenerate(classmid,class2,random_func,nlevels-1,&(values[nhalf]));

	return;
}


void rwf_fbm2DDiscreteGenerate(int corners[],float (*random_func)(void),
	int nlevels,int **classes)
{
	int i,npoints;

	if (nlevels==0) {
		//	OK, fill in the corners
		classes[0][0] = corners[0];
		classes[1][0] = corners[1];
		classes[0][1] = corners[2];
		classes[1][1] = corners[3];
		return;
	}

	if(!random_func)
		random_func = rwf_randGaussian;

	npoints = (1 << nlevels);
	int *temp_classes = new int [npoints+1];
	//	First, take care of the outside edges

	//	Columns are easiest
	//	First column
	rwf_fbm1DDiscreteGenerate(corners[0],corners[2],random_func,
		nlevels,&(classes[0][0]));

	//	Last column
	rwf_fbm1DDiscreteGenerate(corners[1],corners[3],random_func,
		nlevels,&(classes[npoints][0]));

	//	Filling the top and bottom edges requires using
	//	the temporary array
	//	Bottom edge
	rwf_fbm1DDiscreteGenerate(corners[0],corners[1],random_func,
		nlevels,temp_classes);
	for (i=1; i<npoints; i++) classes[i][0] = temp_classes[i];

	//	Top edge
	rwf_fbm1DDiscreteGenerate(corners[2],corners[3],random_func,
		nlevels,temp_classes);
	for (i=1; i<npoints; i++) classes[i][npoints] = temp_classes[i];

	delete temp_classes;

	//	Now fill in the rest of the array

	//	Loop over recursion levels
	int nhalf = npoints;
	int x,y;
	for (int level=0; level<nlevels; level++) {
		int nacross = nhalf;
		nhalf = nhalf / 2;
		//	Compute all of the midpoints for this level
		//	These are the points at the centers of the
		//	new square
		for (x=nhalf; x<npoints; x+=nacross) {
			for (y=nhalf; y<npoints; y+=nacross) {
				if ((*random_func)() >= 0.0) {
					if ((*random_func)() >= 0.0) {
						classes[x][y] = classes[x-nhalf][y-nhalf];
					}
					else {
						classes[x][y] = classes[x-nhalf][y+nhalf];
					}
				}
				else {
					if ((*random_func)() >= 0.0) {
						classes[x][y] = classes[x+nhalf][y-nhalf];
					}
					else {
						classes[x][y] = classes[x+nhalf][y+nhalf];
					}
				}
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
					if ((*random_func)() >= 0.0) {
						if ((*random_func)() >= 0.0) {
							classes[x][y] = classes[x][y-xs];
						}
						else {
							classes[x][y] = classes[x][y+xs];
						}
					}
					else {
						if ((*random_func)() >= 0.0) {
							classes[x][y] = classes[x-xs][y];
						}
						else {
							classes[x][y] = classes[x+xs][y];
						}
					}
				}
			}

			xs = nacross;
			ys = nhalf;
			for (x=xs; x<npoints; x+=nacross) {
				for (y=ys; y<npoints; y+=nacross) {
					if ((*random_func)() >= 0.0) {
						if ((*random_func)() >= 0.0) {
							classes[x][y] = classes[x][y-ys];
						}
						else {
							classes[x][y] = classes[x][y+ys];
						}
					}
					else {
						if ((*random_func)() >= 0.0) {
							classes[x][y] = classes[x-ys][y];
						}
						else {
							classes[x][y] = classes[x+ys][y];
						}
					}
				}
			}
		}	//	Okay, finished with the other midpoints

	}	//	Finished looping over all recursions

	return;
}

void rwf_fbm2DDiscreteGenerate(int corners[], long seed_corners[],
															 float (*random_func)(void),
															 int nlevels,int **classes)
{
	int i,npoints;
	long seed;

	if (nlevels==0) {
		//	OK, fill in the corners
		classes[0][0] = corners[0];
		classes[1][0] = corners[1];
		classes[0][1] = corners[2];
		classes[1][1] = corners[3];
		return;
	}

	if(!random_func)
		random_func = rwf_randGaussian;

	npoints = (1 << nlevels);
	int *temp_classes = new int [npoints+1];
	//	First, take care of the outside edges

	//	Columns are easiest
	//	First column
	seed = seed_corners[0] + seed_corners[2];
	rwf_randSrand(seed);
	rwf_fbm1DDiscreteGenerate(corners[0],corners[2],random_func,
		nlevels,&(classes[0][0]));

	//	Last column
	seed = seed_corners[1] + seed_corners[3];
	rwf_randSrand(seed);
	rwf_fbm1DDiscreteGenerate(corners[1],corners[3],random_func,
		nlevels,&(classes[npoints][0]));

	//	Filling the top and bottom edges requires using
	//	the temporary array
	//	Bottom edge
	seed = seed_corners[0] + seed_corners[1];
	rwf_randSrand(seed);
	rwf_fbm1DDiscreteGenerate(corners[0],corners[1],random_func,
		nlevels,temp_classes);
	for (i=1; i<npoints; i++) classes[i][0] = temp_classes[i];

	//	Top edge
	seed = seed_corners[2] + seed_corners[3];
	rwf_randSrand(seed);
	rwf_fbm1DDiscreteGenerate(corners[2],corners[3],random_func,
		nlevels,temp_classes);
	for (i=1; i<npoints; i++) classes[i][npoints] = temp_classes[i];

	delete temp_classes;

	//	Now fill in the rest of the array

	//	Loop over recursion levels

	seed = seed_corners[0] + seed_corners[1] + seed_corners[2] + seed_corners[3];
	rwf_randSrand(seed);

	int nhalf = npoints;
	int x,y;
	for (int level=0; level<nlevels; level++) {
		int nacross = nhalf;
		nhalf = nhalf / 2;
		//	Compute all of the midpoints for this level
		//	These are the points at the centers of the
		//	new square
		for (x=nhalf; x<npoints; x+=nacross) {
			for (y=nhalf; y<npoints; y+=nacross) {
				if ((*random_func)() >= 0.0) {
					if ((*random_func)() >= 0.0) {
						classes[x][y] = classes[x-nhalf][y-nhalf];
					}
					else {
						classes[x][y] = classes[x-nhalf][y+nhalf];
					}
				}
				else {
					if ((*random_func)() >= 0.0) {
						classes[x][y] = classes[x+nhalf][y-nhalf];
					}
					else {
						classes[x][y] = classes[x+nhalf][y+nhalf];
					}
				}
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
					if ((*random_func)() >= 0.0) {
						if ((*random_func)() >= 0.0) {
							classes[x][y] = classes[x][y-xs];
						}
						else {
							classes[x][y] = classes[x][y+xs];
						}
					}
					else {
						if ((*random_func)() >= 0.0) {
							classes[x][y] = classes[x-xs][y];
						}
						else {
							classes[x][y] = classes[x+xs][y];
						}
					}
				}
			}

			xs = nacross;
			ys = nhalf;
			for (x=xs; x<npoints; x+=nacross) {
				for (y=ys; y<npoints; y+=nacross) {
					if ((*random_func)() >= 0.0) {
						if ((*random_func)() >= 0.0) {
							classes[x][y] = classes[x][y-ys];
						}
						else {
							classes[x][y] = classes[x][y+ys];
						}
					}
					else {
						if ((*random_func)() >= 0.0) {
							classes[x][y] = classes[x-ys][y];
						}
						else {
							classes[x][y] = classes[x+ys][y];
						}
					}
				}
			}
		}	//	Okay, finished with the other midpoints

	}	//	Finished looping over all recursions

	return;
}


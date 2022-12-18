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

#include "rwfjulia.hpp"
#include "rwfgraph.hpp"

#include <complex.h>



/*
**	Functions to support class RWFJulia_Object
*/
void RWFJulia_Object::process(void)
{
	complex z;
	int level;

	if(!action_func || !escape_func || !transform_func)
		return;

	int xmin = getMinX(), xmax = getMaxX();
	int ymin = getMinY(), ymax = getMaxY();
	for (int j=ymin; j<=ymax; j++) {
		// Did the user interrupt?
		if(rwf_miscCheckBreak())
			return;

		for (int i=xmin; i<=xmax; i++) {
			z = getComplex(i,j);	//Get complex coordinates
			level = 0;
			while (!(*escape_func)(z,level,escape_data) &&
				level<max_level) {
				z = (*transform_func)(z,level,transform_data);
				level++;
			}
			//	Finally, draw the pixel
			(*action_func)(i,j,level,max_level,action_data);
		}	// End of x loop
	}	// End of y loop

	return;
}



/*
**	Utility Functions
*/
complex rwf_juliaZPowerTransform(complex &z, int level, void *data)
{
	RWFJulia_zpowerdata *zdata = (RWFJulia_zpowerdata *) data;
	complex result(0.0, 0.0);

	if (!zdata || zdata->power < 1) return result;

	result = z;
	for (int i=1; i<zdata->power; i++) result *= z;

	return (result + zdata->c);
}


int rwf_juliaZPowerEscape(complex &z, int level, void *escape_data)
{
	RWFJulia_zpowerdata *zdata = (RWFJulia_zpowerdata *) escape_data;
	float threshold;

	if (!zdata) threshold = 2.0;
	else threshold = zdata->cutoff;

	if (abs(z) < threshold) return 0; // Indeterminate
	else return 1; // It's in the escape set
}


void rwf_juliaDrawColor(int i, int j, int level, int max_level,
	                      void *action_data)
{
	RWFJulia_zpowerdata *zdata = (RWFJulia_zpowerdata *) action_data;

	//	More interesting if colors are modulated
	int color_level = (level % 15) + 1;

	if (level < max_level)
		rwf_graphPlot(i,j,color_level);
	else
		rwf_graphPlot(i,j,0);

	return;
}


complex rwf_juliaMandelbrotTransform(complex &z, int level, void *data)
{
	RWFJulia_zpowerdata *zdata = (RWFJulia_zpowerdata *) data;
	complex result(0.0, 0.0);

	if (!zdata || zdata->power < 1) return result;

	//	First iteration of this point
	if (level==0) {
		//	Save the initial value to be used as the offset
		zdata->c = z;
		//	Set the cutoff threshold correspondingly
		zdata->cutoff = RWFMISC_MAX(2.0,abs(z));
		return z;
	}

	result = z;
	for (int i=1; i<zdata->power; i++) result *= z;

	return (result + zdata->c);
}

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
#include "rwfvec.hpp"
#include "rwfrand.hpp"



/*
**	Utility Functions
*/
int rwf_chaosPlotLorenzAttractor(RWFvec_Vector &xyz,RWFvec_Vector &xy,
	void *user_data)
{
	RWFchaos_LorenzData *ldata = (RWFchaos_LorenzData *) user_data;

	if (!ldata) return 0;	//	No valid data

	float x = xyz[0];
	float y = xyz[1];
	float z = xyz[2];
	float new_x,new_y,new_z;
	float dt = ldata->dt;

	//	Now evaluate the next points using Euler's method
	new_x = x + dt*(ldata->sigma * (y-x));
	new_y = y + dt*(ldata->r*x - y - x*z);
	new_z = z + dt*((-ldata->b)*z + x*y);

	//	Generate x-z plot
	xy[0] = x;
	xy[1] = z;

	//	Updata the coordinates for the next pass
	xyz[0] = new_x;
	xyz[1] = new_y;
	xyz[2] = new_z;

	//	Everything is OK, return
	return 1;
}


int rwf_chaosPlotPopulationFeigenbaum(RWFvec_Vector &pvec,RWFvec_Vector &xy,
	void *user_data)
{
	RWFchaos_PopulationData *pdata = (RWFchaos_PopulationData *) user_data;

	if (!pdata) return 0;	//	No valid data

	//	Only one value is needed in the passed data
	//	vector, representing the current population
	pvec[0] = pdata->r * pvec[0] * (1.0 - pvec[0]);
	pvec[0] = RWFMISC_MAX(0.0,pvec[0]);
	pvec[0] = RWFMISC_MIN(1.0,pvec[0]);

	//	OK, now plot (r,p)
	xy[0] = pdata->r;
	xy[1] = pvec[0];

	//	All done, return
	return 1;
}


int rwf_chaosPlotZ2Attractor(RWFvec_Vector &xyz,RWFvec_Vector &xy,
	void *user_data)
{
	complex z,znew,c;
	RWFchaos_Z2Data *zdata = (RWFchaos_Z2Data *) user_data;

	if (!zdata) return 0;	//	No valid data

	z = complex(xyz[0],xyz[1]);
	c = zdata->c;

	znew = z*z + c;

	//	Generate x-z plot
	xy[0] = real(z);
	xy[1] = imag(z);

	//	Update the coordinates for the next pass
	xyz[0] = real(znew);
	xyz[1] = imag(znew);

	//	Everything is OK, return
	return 1;
}


int rwf_chaosPlotPopulationState(RWFvec_Vector &pvec,RWFvec_Vector &xy,
	void *user_data)
{
	RWFchaos_PopulationData *pdata = (RWFchaos_PopulationData *) user_data;

	if (!pdata) return 0;	//	No valid data

	//	Only one value is needed in the passed data
	//	vector, representing the current population
	xy[1] = pvec[0];
	pvec[0] = pdata->r * pvec[0] * (1.0 - pvec[0]);
	pvec[0] = RWFMISC_MAX(0.0,pvec[0]);
	pvec[0] = RWFMISC_MIN(1.0,pvec[0]);

	//	OK, now plot (p(k),p(k-1))
	xy[0] = pvec[0];

	//	All done, return
	return 1;
}


int rwf_chaosPlotRandomState(RWFvec_Vector &pvec,RWFvec_Vector &xy,
	void *user_data)
{
	RWFchaos_PopulationData *pdata = (RWFchaos_PopulationData *) user_data;

	if (!pdata) return 0;	//	No valid data

	//	Only one value is needed in the passed data
	//	vector, representing the current population
	xy[1] = pvec[0];
	pvec[0] = rwf_randRand();
	pvec[0] = RWFMISC_MAX(0.0,pvec[0]);
	pvec[0] = RWFMISC_MIN(1.0,pvec[0]);

	//	OK, now plot (p(k),p(k-1))
	xy[0] = pvec[0];

	//	All done, return
	return 1;
}

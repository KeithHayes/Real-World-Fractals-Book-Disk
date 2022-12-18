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

#ifndef RWFCHAOS_HPP
#define RWFCHAOS_HPP 1

#include "rwfvec.hpp"

#include <complex.h>



class RWFchaos_LorenzData {
private:

protected:

public:
	float sigma;
	float b;
	float r;
	float dt;

	RWFchaos_LorenzData(void) {
		sigma = 10.0;
		b = 8.0 / 3.0;
		r = 28.0;
		dt = 0.001;
	};
};


class RWFchaos_PopulationData {
private:

protected:

public:
	float r;

	RWFchaos_PopulationData(void) {
		r = 1.0;
	};
};


class RWFchaos_Z2Data {
private:

protected:

public:
	complex c;

	RWFchaos_Z2Data(void) {
		c = complex(0.0,0.0);
	};
	RWFchaos_Z2Data(complex cIn) {
		c = cIn;
	};
};



int rwf_chaosPlotLorenzAttractor(RWFvec_Vector &,RWFvec_Vector &,
	void *);
int rwf_chaosPlotPopulationFeigenbaum(RWFvec_Vector &,RWFvec_Vector &,
	void *);
int rwf_chaosPlotZ2Attractor(RWFvec_Vector &,RWFvec_Vector &,
	void *);
int rwf_chaosPlotPopulationState(RWFvec_Vector &,RWFvec_Vector &,
	void *);
int rwf_chaosPlotRandomState(RWFvec_Vector &,RWFvec_Vector &,
	void *);



#endif

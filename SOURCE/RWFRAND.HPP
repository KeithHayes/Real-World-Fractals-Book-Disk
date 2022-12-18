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

#ifndef RWFRAND_HPP
#define RWFRAND_HPP 1

#include <math.h>



float rwf_randRand(void);
void rwf_randSrand(unsigned long);
float rwf_randBiUniform(void);
float rwf_randExponential(void);
float rwf_randBiExponential(void);
float rwf_randGaussian(void);
int rwf_randGetSeed(void);



class RWFrand_RandomVariable {
private:
	float mean;		// Mean
	float stdev;	//	Standard Deviation
	long seed;

protected:

public:
	RWFrand_RandomVariable(void)
		{mean = 0.0; stdev = 1.0; seed = 1010101L;};
	RWFrand_RandomVariable(float new_mean,float new_stdev)
		{mean = new_mean; stdev = fabs(new_stdev); seed = 1010101L;};
	void setSeed(long new_seed) {seed = new_seed;};
	void setMean(float new_mean) {mean = new_mean;};
	void setDeviation(float new_stdev) {stdev = fabs(new_stdev);};
	float getMean(void) {return mean;};
	float getDeviation(void) {return stdev;};
	virtual float next(void)	{return mean;};	//	Get the next value
};


class RWFrand_UniformRV:public RWFrand_RandomVariable {
private:

protected:

public:
	float next(void) {
		float x;
		x = 2.0*getDeviation()*(rwf_randRand()-0.5) + getMean();
				setSeed(rwf_randGetSeed());
		return x;
	};
};


class RWFrand_GaussianRV: public RWFrand_RandomVariable {
private:

protected:

public:
	float next(void) {
		float x;
		x = getDeviation()*rwf_randGaussian() + getMean();
				setSeed(rwf_randGetSeed());
		return x;
	};
};



#endif

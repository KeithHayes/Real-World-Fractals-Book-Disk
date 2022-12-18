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

#include "rwfrand.hpp"
#include "rwfmisc.hpp"

#include <stdlib.h>
#include <math.h>



#define RWFRAND_TABLE_SIZE 101
#define RWFRAND_INITIAL_SEED 3030303L
//	These are known to both RWFrand_srand() and RWFrand_rand()
static int rwfrand_first_time = 1;
static float rwfrand_ran_table[RWFRAND_TABLE_SIZE];
static float rwfrand_last_rand;
static int rwfrand_last_rand_int = RWFRAND_INITIAL_SEED;
static int need_newvars = 1;

/*
**	Utility Functions
*/
float rwf_randRand(void)
{
	int index;

	if (rwfrand_first_time) rwf_randSrand(RWFRAND_INITIAL_SEED);

	index = RWFMISC_NINT(RWFRAND_TABLE_SIZE * rwfrand_last_rand);
	rwfrand_last_rand = rwfrand_ran_table[index];
	rwfrand_ran_table[index] = (rwfrand_last_rand_int=rand()) /
		(RAND_MAX + 1.0);

	return rwfrand_last_rand;
}


void rwf_randSrand(unsigned long iseed)
{
	srand(iseed);
	for (int i=0; i<RWFRAND_TABLE_SIZE; i++) {
		rwfrand_ran_table[i] = (rwfrand_last_rand_int=rand()) /
		(RAND_MAX + 1.0);
	}
	rwfrand_last_rand = (rwfrand_last_rand_int=rand()) / (RAND_MAX + 1.0);
	rwfrand_first_time = 0;
	need_newvars = 1;
}


float rwf_randBiUniform(void)
{
	double x;

	x = rwf_randRand();
	if (rwf_randRand() < 0.5) x *= -1.0;

	return x;
}


float rwf_randExponential(void)
{
	double x;

	x = rwf_randRand();

	return (-log(x));
}


float rwf_randBiExponential(void)
{
	double x;

	x = rwf_randExponential();
	if (rwf_randRand() < 0.5) x *= -1.0;

	return x;
}

float rwf_randGaussian(void)
{
	static float factor,v1,v2;
	float result;

	if (need_newvars) {
		float r =1.1;
		while (r>=1.0 || r==0) { // Find radius < 1.0
			v1 = 2.0 * rwf_randRand() - 1.0;
			v2 = 2.0 * rwf_randRand() - 1.0;
			r = v1*v1 + v2*v2;
		}
		need_newvars = 0;
		factor = sqrt(-2.0 * log (r) / r);
		result = v1*factor;
	}	else {
		need_newvars = 1;
		result = v2*factor;
	}

	return result;
}


int rwf_randGetSeed(void)
{
	return rwfrand_last_rand_int;
}
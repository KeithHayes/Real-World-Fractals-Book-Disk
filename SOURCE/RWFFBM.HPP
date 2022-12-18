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

#ifndef RWFFBM_HPP
#define RWFFBM_HPP 1

#include "rwfrand.hpp"



void rwf_fbm1DGenerate(float,float,float (*random_func)(void),
											 float,float,int,float *);
void rwf_fbm2DGenerate(float corners[],float (*random_func)(void),
											 float,float,int,float **);
void rwf_fbm2DGenerate(float corners[],long seed_corners[],
											 float (*random_func)(void),
											 float,float,int,float **);
void rwf_fbm1DDiscreteGenerate(int,int,
															 float (*random_func)(void),int,int *);
void rwf_fbm2DDiscreteGenerate(int corners[],float (*random_func)(void),
															 int,int **);
void rwf_fbm2DDiscreteGenerate(int corners[],long seed_corners[],
															 float (*random_func)(void),int,int **);



#endif

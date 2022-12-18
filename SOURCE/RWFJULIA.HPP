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

#ifndef RWFJULIA_HPP
#define RWFJULIA_HPP 1

#include "rwfgraph.hpp"

#include <complex.h>

complex rwf_juliaZPowerTransform(complex &,int,void *);
int     rwf_juliaZPowerEscape(complex &,int,void *);
void    rwf_juliaDrawColor(int,int,int,int,void *);
complex rwf_juliaMandelbrotTransform(complex &,int,void *);


/*
** Class definitions
*/

class RWFJulia_Object:public RWFgraph_ComplexObject {
private:
	int max_level;
	void *escape_data;
	void *transform_data;
	void *action_data;

	int (*escape_func)(complex &,int,void *);
	complex (*transform_func)(complex &,int,void *);
	void (*action_func)(int,int,int,int,void *);

protected:

public:
	void setMaximumLevel(int max_levelIn) {
		max_level = max_levelIn;
		return;
	};
	void setEscapeFunction(int (*escape_funcIn)(complex &,int,void *),
		void *escape_dataIn) {
		escape_func = escape_funcIn;
		escape_data = escape_dataIn;
		return;
	};
	void setTransformation(complex (*transform_funcIn)(complex &,int,void *),
		void *transform_dataIn) {
		transform_func = transform_funcIn;
		transform_data = transform_dataIn;
		return;
	};
	void setActionFunction(void (*action_funcIn)(int,int,int,int,void *),
		void *action_dataIn) {
		action_func = action_funcIn;
		action_data = action_dataIn;
		return;
	};
	void process(void);
	void draw(void) {
		if(!action_func) {
			setActionFunction(rwf_juliaDrawColor, NULL);
		}
		if(!escape_func) {
			setEscapeFunction(rwf_juliaZPowerEscape, NULL);
    }
		process();};
};


typedef struct {
	int power;
	complex c;
	float cutoff;
} RWFJulia_zpowerdata;


#endif

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

#ifndef RWFVK_HPP
#define RWFVK_HPP 1

#include "rwfaf.hpp"

class RWFvk_Object:public RWFaf_Object {
private:
	void (*random_func)(RWFgraph_Polyline &basepoly, int level,
											void *userdata, RWFgraph_Polyline &outpoly);

	RWFgraph_Polyline basepoly_generator;
	RWFgraph_Polyline ranpoly_generator;
	void *rand_data;

protected:

public:
	RWFvk_Object(void);

	void setRandomFunction(
		void (*ranfuncIn)(RWFgraph_Polyline &basepoly, int level,
											void *userdata, RWFgraph_Polyline &outpoly),
		void *data)	{random_func = ranfuncIn; rand_data = data;};

	void setInitiator(RWFlist_ObjectPtrTable &otable)
		{RWFaf_Object::setInitiator(otable);};
	void setInitiator(RWFgraph_Line &line);
	void setInitiator(RWFgraph_LineTable &lt);
	void setInitiator(RWFgraph_Polyline &poly);

	void setGenerator(RWFgraph_LineTable &lt);
	void setGenerator(RWFgraph_Polyline &poly);
	void setGenerator(RWFmat_MatrixTable &mtable)
		{RWFaf_Object::setGenerator(mtable);};

	void process(void);
	void generate(RWFmat_Matrix &pmatrix, int level);

	void draw(void)
		{if(!action_func) setAction(rwf_afDraw); process();};
};

class RWFvk_HeightData {
public:
	long initial_seed;
	float mean, stdev;
};

void rwf_vkRandomizeHeight(RWFgraph_Polyline &basepoly, int level,
													 void *rand_data, RWFgraph_Polyline &outpoly);

#endif

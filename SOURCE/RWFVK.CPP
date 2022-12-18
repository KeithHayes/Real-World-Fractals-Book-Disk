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

#include "rwfvk.hpp"
#include <math.h>

// void constructor
RWFvk_Object::RWFvk_Object(void)
{
	random_func = NULL;
	rand_data = NULL;

	// Ok, now set up the initiator & generator
	RWFmat_MatrixTable mtable;

	RWFlist_ObjectPtrTable otable;
	RWFgraph_Line *unit =
		new RWFgraph_Line(-0.5, 0.0, 0.5, 0.0);

	otable[0] = unit;
	RWFaf_Object::setInitiator(otable);

	basepoly_generator[0].setCoord(0.0, 0.0);
	basepoly_generator[1].setCoord(1/3.0, 0.0);
	basepoly_generator[2].setCoord(1/2.0, (1/3.0)*sqrt(3.0/4.0));
	basepoly_generator[3].setCoord(2.0/3.0, 0.0);
	basepoly_generator[4].setCoord(1.0, 0.0);

	rwf_graphNormalize(basepoly_generator);
	rwf_matPolylineToMatrixTable(basepoly_generator, mtable);

	RWFaf_Object::setGenerator(mtable);

	return;
}

void
RWFvk_Object::setInitiator(RWFgraph_Line &line_seg)
{
	RWFaf_Object::setInitiator(&line_seg);
}

void
RWFvk_Object::setInitiator(RWFgraph_LineTable &lt)
{
	RWFlist_ObjectPtrTable otable;
	RWFgraph_Line *new_line, unit_line(-0.5, 0.0, 0.5, 0.0);

	// Loop through all segments
	for(int i=0; i < lt.size(); i++) {
		new_line = new RWFgraph_Line(-0.5, 0.0, 0.5, 0.0);
		new_line->setMatrix(rwf_graphLineToLineMatrix(unit_line, lt[i]));
		new_line->setColor(getColor());
		otable[i] = new_line;
	}

	RWFaf_Object::setInitiator(otable);
}

// To simplify debugging, we have used setInitiator() for a line
// segment table with the polygon version of setInitiator().  Note
// that all this does is create a line segment table and pass it
// to setInitiator() for a line segment table

void
RWFvk_Object::setInitiator(RWFgraph_Polyline &poly)
{
	RWFgraph_LineTable ltable;

	for(int i=1; i < poly.numPoints(); i++) {
		ltable[i-1].setEnd(poly[i-1]);
		ltable[i-1].setFirst(poly[i]);
	}

	setInitiator(ltable);
}

void RWFvk_Object::setGenerator(RWFgraph_LineTable &lt)
{
	RWFmat_MatrixTable mtable;

	RWFgraph_Line *unit =
		new RWFgraph_Line(-0.5, 0.0, 0.5, 0.0);

	rwf_graphNormalize(lt);
	for(int i=0; i < lt.size(); i++)
		mtable[i] = rwf_graphLineToLineMatrix(*unit, lt[i]);

	rwf_graphNormalize(basepoly_generator);
	rwf_matPolylineToMatrixTable(basepoly_generator, mtable);

	RWFaf_Object::setGenerator(mtable);
}

void RWFvk_Object::setGenerator(RWFgraph_Polyline &poly)
{
	RWFmat_MatrixTable mtable;

	basepoly_generator = poly;
	rwf_graphNormalize(basepoly_generator);
	rwf_matPolylineToMatrixTable(basepoly_generator, mtable);

	RWFaf_Object::setGenerator(mtable);
}

void RWFvk_Object::process(void)
{
	static RWFmat_Matrix save_matrix(2,2);
	//	base_object is an RWFgraph_Object pointer

	if (!action_func) return;	//	No actions are defined!

	for (int i=0; i<objectptrlist.size(); i++) {
		base_object = objectptrlist[i];
		cur_object_size = objectsizelist[i];
		save_matrix = base_object->getMatrix();
		base_object->setColor(getColor());
		generate(getMatrix() * save_matrix,0);
		base_object->setMatrix(save_matrix);
	}

	return;
}
void RWFvk_Object::generate(RWFmat_Matrix &pmatrix,int level)
{

	//	Reached max recursion level yet?
	if (level >= max_level) {
		base_object->setMatrix(pmatrix);
		(*action_func) (base_object,level);
		return;
	}

	//	Now, is the object small enough?
	float scale = rwf_matGetGlobalScalingFactor(pmatrix);
	if((scale*cur_object_size) <= min_object_size) {
		base_object->setMatrix(pmatrix);
		(*action_func) (base_object,level);
		return;
	}

	RWFmat_MatrixTable ran_transforms;
	if(random_func) {
		(*random_func) (basepoly_generator, level, rand_data,
										ranpoly_generator);
		rwf_matPolylineToMatrixTable(ranpoly_generator, ran_transforms);
	} else {
		ran_transforms = transforms;
	}

	rwf_miscCheckEscape();

	int next_level = level + 1;
	for (int i=0; i<ran_transforms.size(); i++) {
		//	OK, go to the next recursion level
		generate(pmatrix*ran_transforms[i],next_level);
	}

	return;
}

// Sample randomizing function
#include "rwfrand.hpp"

void
rwf_vkRandomizeHeight(RWFgraph_Polyline &basepoly, int level,
											void *rand_data, RWFgraph_Polyline &outpoly)
{
	float new_height;
	RWFvk_HeightData *hdata = (RWFvk_HeightData *)rand_data;
	long seed;

	// Temporary coordinate to hold the (x,y) position
	// of the center of the generator
	RWFgraph_Point temp_coord;
	float x, y;

	// Copy the base polyline to the output polyline
	outpoly = basepoly;

	// If no data, then don't randomize
	if(!hdata || basepoly.numPoints() != 5)
		return;

	// Use the starting seed if we are at the first level
	if(level == 0)
		rwf_randSrand(hdata->initial_seed);

	new_height = hdata->mean + rwf_randGaussian()*hdata->stdev;

	// Now replace the y coordinate with the new height
	outpoly[2].getCoord(x, y);
	outpoly[2].setCoord(x, new_height);

	return;
}


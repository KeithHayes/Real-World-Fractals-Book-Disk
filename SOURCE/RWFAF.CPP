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

#include "rwfaf.hpp"



static int rwfaf_fractalLevel = RWFAF_DEFAULT_FRACTAL_LEVEL;
static int rwfaf_lastFractalLevel = -1;

static RWFaf_UserDataTable dummy_userdatatable;


/*
**	Functions to support class RWFaf_Object
*/
void
RWFaf_Object::setColor(int color)
{
	if(color >= 0) {
		for(int i=0; i < objectptrlist.size(); i++)
			objectptrlist[i]->setColor(color);
	}

	setEraseColor(color);
	RWFgraph_Object::setColor(color);

	return;
}

void
RWFaf_Object::setEraseColor(int color)
{
	if(color >= 0) {
		for(int i=0; i < erase_object.size(); i++)
			erase_object[i]->setColor(color);
	}

	return;
}

void RWFaf_Object::setInitiator(RWFlist_ObjectPtrTable &objectTableIn) {
	int i;
	objectptrlist = objectTableIn;
	if(objectsizelist)
		delete objectsizelist;
	objectsizelist = new float [objectptrlist.size()];
	for (i=0; i<objectptrlist.size(); i++) {
		objectsizelist[i] = 1.0;
	}
	return;
}

void
RWFaf_Object::setInitiator(RWFgraph_Object *object)
{
	objectptrlist.reset();
	objectptrlist[0] = object;
	if(objectsizelist)
		delete objectsizelist;

	objectsizelist = new float [1];
	objectsizelist[0] = 1.0;

	return;
}

void
RWFaf_Object::setInitiator(RWFgraph_Line &line_seg)
{
	setInitiator(&line_seg);
}

void
RWFaf_Object::setInitiator(RWFgraph_LineTable &lt)
{
	RWFlist_ObjectPtrTable otable;

	for(int i=0; i < lt.size(); i++)
		otable[i] = &(lt[i]);

	setInitiator(otable);
}

void
RWFaf_Object::setInitiator(RWFgraph_Polyline &poly)
{
	RWFlist_ObjectPtrTable otable;
	RWFgraph_Line *line;

	for(int i=0; i < poly.numPoints()-1; i++) {
		line = new RWFgraph_Line;
		line->setFirst(poly[i]);
		line->setEnd(poly[i+1]);
		otable[i] = line;
	}
	line = new RWFgraph_Line;
	line->setFirst(poly[poly.numPoints()-1]);
	line->setEnd(poly[0]);
	otable[poly.numPoints()-1] = line;

	setInitiator(otable);
}

void RWFaf_Object::setEraseInitiator(RWFlist_ObjectPtrTable &objectTableIn) {
	erase_object = objectTableIn;
	return;
}

void
RWFaf_Object::setEraseInitiator(RWFgraph_Object *object)
{
	erase_object.reset();
	erase_object[0] = object;

	return;
}

void RWFaf_Object::process(void)
{
	RWFmat_Matrix save_matrix;
	int save_color;
	//	base_object is an RWFgraph_Object pointer

	if (!action_func) return;	//	No actions are defined!

	for (int i=0; i<objectptrlist.size(); i++) {
		base_object = objectptrlist[i];
		cur_object_size = objectsizelist[i];
		save_matrix = base_object->getMatrix();
		save_color = base_object->getColor();
		if(getColor() >= 0)
			base_object->setColor(getColor());
		generate(getMatrix() * save_matrix,0);
		base_object->setMatrix(save_matrix);
		base_object->setColor(save_color);
	}

	return;
}


void RWFaf_Object::generate(RWFmat_Matrix &pmatrix,int level)
{
	int valid = 1;
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

	// Let the user get away!
	rwf_miscCheckEscape();

	int next_level = level + 1;
	RWFmat_Matrix tmatrix;
	int ntrans = RWFMISC_MAX(transforms.size(), user_data.size());
	for (int i=0; i<ntrans; i++) {
		//	OK, go to the next recursion level
		if(matrix_func)
			valid = (*matrix_func)(user_data[i], level, tmatrix);
		else
			tmatrix = transforms[i];
		if(valid)
			generate(pmatrix*tmatrix,next_level);
	}

	return;
}


void RWFaf_Object::process_reverse(void)
{
	RWFmat_Matrix save_matrix;
	int save_color;
	//	base_object is an RWFgraph_Object pointer

	if (!action_func) return;	//	No actions are defined!

	for (int i=0; i<objectptrlist.size(); i++) {
		base_object = objectptrlist[i];
		cur_object_size = objectsizelist[i];
		save_matrix = base_object->getMatrix();
		//	Draw the initiator
		base_object->setMatrix(getMatrix() * save_matrix);
    save_color = base_object->getColor();
		if(getColor() >= 0)
			base_object->setColor(getColor());
		(*action_func)(base_object,0);
		generate_reverse(getMatrix() * save_matrix,0);
		base_object->setMatrix(save_matrix);
		base_object->setColor(save_color);
	}

	return;
}


void RWFaf_Object::generate_reverse(RWFmat_Matrix &pmatrix,int level)
{
	int valid = 1;
	//	Reached max recursion level yet?
	if (level >= max_level) return;

	//	Now, is the object small enough?
	float scale = rwf_matGetGlobalScalingFactor(pmatrix);
	if((scale*cur_object_size) <= min_object_size) return;

	int next_level = level + 1;
	//	OK, now erase the appropriate regions
	for (int i=0; i<erase_object.size(); i++) {
		RWFmat_Matrix save_matrix;
		save_matrix = erase_object[i]->getMatrix();
		erase_object[i]->setMatrix(pmatrix*save_matrix);
		(*action_func) (erase_object[i],next_level);
		erase_object[i]->setMatrix(save_matrix);
	}

	// Let the user get away!
	rwf_miscCheckEscape();

	RWFmat_Matrix tmatrix;
	int ntrans = RWFMISC_MAX(transforms.size(), user_data.size());
	for (i=0; i<ntrans; i++) {
		//	OK, go to the next recursion level
		if(matrix_func)
			valid = (*matrix_func)(user_data[i], level, tmatrix);
		else
			tmatrix = transforms[i];
		if(valid)
			generate_reverse(pmatrix*tmatrix,next_level);
	}

	return;
}

void RWFaf_3DObject::process(void)
{
	RWFmat_Matrix save_matrix;
	int save_color;
	//	base_object is an RWFgraph_Object pointer

	if (!action_3Dfunc) return;	//	No actions are defined!

	for (int i=0; i<objectptrlist.size(); i++) {
		base_object = objectptrlist[i];
		cur_object_size = objectsizelist[i];
		save_matrix = base_object->getMatrix();
		save_color = base_object->getColor();
		if(getColor() >= 0)
			base_object->setColor(getColor());
		generate(getMatrix() * save_matrix,0);
		base_object->setMatrix(save_matrix);
		base_object->setColor(save_color);
	}

	return;
}

void RWFaf_3DObject::generate(RWFmat_Matrix &pmatrix,int level)
{
	int valid = 1;
	//	Reached max recursion level yet?
	if (level >= max_level) {
		base_object->setMatrix(pmatrix);
		(*action_3Dfunc) (base_object,level, *save_vg);
		return;
	}

	//	Now, is the object small enough?
	float scale = rwf_matGetGlobalScalingFactor(pmatrix);
	if((scale*cur_object_size) <= min_object_size) {
		base_object->setMatrix(pmatrix);
		(*action_3Dfunc) (base_object,level,*save_vg);
		return;
	}

	// Let the user get out of this
	rwf_miscCheckEscape();

	int next_level = level + 1;
	RWFmat_Matrix tmatrix;
	int ntrans = RWFMISC_MAX(transforms.size(), user_data.size());
	RWFlist_SortingOrderTable order;

	// Sort the objects
	rwf_graphSortMatrices(*save_vg, pmatrix,
												base_object->getReferencePoint(),
												transforms, order);

	int index;
	for (int i=0; i<ntrans; i++) {
		//	OK, go to the next recursion level
		if(matrix_func) {
			valid = (*matrix_func)(user_data[i], level, tmatrix);
		} else {
			int index = order[i];
			tmatrix = transforms[index];
		}

		if(valid)
			generate(pmatrix*tmatrix,next_level);
	}

	return;
}

void RWFaf_3DObject::setEraseInitiator(RWFlist_ObjectPtrTable &objectTableIn)
{
	erase_object = objectTableIn;
	numBase = transforms.size();
	// Now add the transformations for the erase object
	for(int i=0; i < objectTableIn.size(); i++) {
		transforms[i + numBase] = objectTableIn[i]->getMatrix();
	}

	return;
}

void RWFaf_3DObject::process_reverse(void)
{
	RWFmat_Matrix save_matrix;
	int save_color;
	//	base_object is an RWFgraph_Object pointer

	if (!action_3Dfunc) return;	//	No actions are defined!

	for (int i=0; i<objectptrlist.size(); i++) {
		base_object = objectptrlist[i];
		cur_object_size = objectsizelist[i];
		save_color = base_object->getColor();
		if(getColor() >= 0)
			base_object->setColor(getColor());
		generate_reverse(getMatrix(),0);
		base_object->setColor(save_color);
	}

	return;
}

/*
	This version of generate_reverse() differs from the text because
	it takes into account the matrix generating functions.  The only
	difference is that if you are using a randomizing function, then
	a temporary matrix table is used to hold all of the randomized
	transformations, which can then be properly sorted.  Since there
	can be a variable number of random transformations, the parameter
	numEffectiveBase is used to keep track of how many are actually
	generated for a given level
*/

void
RWFaf_3DObject::generate_reverse(RWFmat_Matrix &pmatrix,int level)
{
	int valid = 1, numEffectiveBase = numBase;
	RWFmat_MatrixTable all_transforms;	// Temporary for sorting

	//	Reached max recursion level yet?
	if (level >= max_level)
		return;

	//	Now, is the object small enough?
	float scale = rwf_matGetGlobalScalingFactor(pmatrix);
	if((scale*cur_object_size) <= min_object_size)
		return;

	// Let the user get out of this
	rwf_miscCheckEscape();

	int next_level = level + 1;
	int index;
	RWFmat_Matrix tmatrix;
	int ntrans = RWFMISC_MAX(transforms.size(), user_data.size());
	RWFlist_SortingOrderTable order;

	// Sort the objects
	if(matrix_func && user_data.size() > 0) {
		// We handle randomized transformations using a
		// temporary matrix table
		// Add all of the base transformations
		for(int j=0; j < numBase; j++)
			all_transforms[j] = transforms[j];
		int index = numBase;

		// Now add the randomized transformations
		for(j=0; j < user_data.size(); j++) {
			valid = (*matrix_func)(user_data[j], next_level, tmatrix);
			if(valid)
				all_transforms[index++] = tmatrix;
		}

		numEffectiveBase = index;
		// Add the erase objects
		for(j=numBase; j < transforms.size(); j++)
			all_transforms[index++] = transforms[j];

		rwf_graphSortMatrices(*save_vg, pmatrix,
													base_object->getReferencePoint(),
													all_transforms, order);
		ntrans = all_transforms.size();
	} else {
		// Otherwise, we just sort as normal
		rwf_graphSortMatrices(*save_vg, pmatrix,
													base_object->getReferencePoint(),
													transforms, order);
	}

	for (int i=0; i<ntrans; i++) {
		//	OK, go to the next recursion level
		index = order[i];
		if(index < numEffectiveBase) {
			// This is a base generator transformation

			// A speed-up here, avoids an unnecessary call
			if(next_level == max_level)
				continue;

			if(matrix_func)
				tmatrix = all_transforms[index];
			else
				tmatrix = transforms[index];
			generate_reverse(pmatrix*tmatrix,next_level);
		} else {
			// This is an erase object to draw
			int j = index - numEffectiveBase;
			RWFmat_Matrix save_matrix;
			save_matrix = erase_object[j]->getMatrix();
			erase_object[j]->setMatrix(pmatrix * save_matrix);
			(*action_3Dfunc) (erase_object[j], next_level, *save_vg);
			erase_object[j]->setMatrix(save_matrix);
		}
	}

	return;
}

void RWFaf_Object::setMinimumSize(float sizeIn) {
	min_object_size = sizeIn;
	return;
}

static RWFlist_SortingOrderTable dummy_ordertable;

void
rwf_graphSortMatrices(RWFgraph_ViewingGeometry &vg,
											RWFmat_Matrix &global_matrix,
											RWFvec_Vector &ref_point,
											RWFmat_MatrixTable &mtable,
											RWFlist_SortingOrderTable &order)
{
	int n = mtable.size();
	// Create a table to hold the projected distances
	// No need to constantly create & destroy it, it can
	// remain static.
	static RWFlist_RTable rtable;
	static RWFvec_Vector tvec(3);

	for(int i=0; i < n; i++) {
		// Transform the reference point
		tvec = (global_matrix * mtable[i]) * ref_point;
		tvec = vg.getProjectedCoordinates(tvec);
		// Now get the distance values
		rtable[i] = tvec.length();
		order[i] = i;
	}

	// Sort the distance values using a bubble sort
	int sorted = 0;
	int limit = n;
	int index1, index2;
	while(!sorted) {
		sorted = 1;
		limit--;
		for(int i=0; i < limit; i++) {
			index1 = order[i];
			index2 = order[i+1];
			if(rtable[index1] < rtable[index2]) {
				sorted = 0;
				order.swap(i, i+1);
			}
		}
	}

	// The correct order for drawing objects is now stored
	// in the order array

	return;
}


/*
**	Utility Functions
*/
void rwf_afSetLevel(int level)
{
	if (level==-1) rwfaf_fractalLevel = RWFAF_DEFAULT_FRACTAL_LEVEL;
	else rwfaf_fractalLevel = level;
	return;
}


int rwf_afGetLevel(void)
{
	return rwfaf_fractalLevel;
}


void rwf_afSetLastLevel(int level)
{
	rwfaf_lastFractalLevel = level;
	return;
}


int rwf_afGetLastLevel(void)
{
	return rwfaf_lastFractalLevel;
}


void rwf_afDraw(RWFgraph_Object *object,int level)
{
	if (object) object->draw();

	return;
}


void rwf_afFill(RWFgraph_Object *object,int level)
{
	if (object) object->fill();

	return;
}


void rwf_afDrawColorCycle(RWFgraph_Object *object,int level)
{
	if (!object) return;
	int savecolor = object->getColor();
	object->setColor(((level+savecolor) % 15) + 1);
	object->draw();
	object->setColor(savecolor);

	return;
}

void rwf_afFillColorCycle(RWFgraph_Object *object, int level)
{
	if(!object) return;
	int savecolor = object->getColor();
	object->setColor(((level+savecolor) % 15) + 1);
	object->fill();
	object->setColor(savecolor);

	return;
}


void rwf_afDrawLastLevel(RWFgraph_Object *object,int level)
{
	int old_level = rwf_afGetLastLevel();
	if (!object || level <= old_level) return;
	object->fill();

	return;
}

void rwf_af3DDraw(RWFgraph_Object *object, int level,
									RWFgraph_ViewingGeometry &vg)
{
	if(object)
		object->draw(vg);

	return;
}

void rwf_af3DDrawColorCycle(RWFgraph_Object *object, int level,
														RWFgraph_ViewingGeometry &vg)
{
	if(!object)
		return;

	int savecolor = object->getColor();
	object->setColor(((savecolor + level) % 15) + 1);
	object->draw(vg);
	object->setColor(savecolor);

	return;
}

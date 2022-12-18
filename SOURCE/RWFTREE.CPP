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

#include "rwftree.hpp"
#include <math.h>

// Basic constructor

RWFaf_SimpleTree::RWFaf_SimpleTree(void)
{
	RWFmat_MatrixTable mtable;
	// First branch, 1/2 length, rotate 30.0
	mtable[0].scale(0.5, 0.5);
	mtable[0].rotate(30.0);
	mtable[0].translate(0.0, 0.3);

	// Second branch, 1/3 length, rotate -45.0
	mtable[1].scale(1.0/3.0, 1.0/3.0);
	mtable[1].rotate(-45.0);
	mtable[1].translate(0.0, 0.6);

	setGenerator(mtable);

	// Now set the initiator, a unit line segment
	RWFlist_ObjectPtrTable otable;
	RWFgraph_Polyline *unit_line = new RWFgraph_Polyline(2);

	(*unit_line)[0].setCoord(0.0, 0.0);
	(*unit_line)[1].setCoord(0.0, 1.0);

	otable[0] = unit_line;
	setInitiator(otable);

	// Now use the same one for the "erase" initiator
	RWFgraph_Polyline *unit2 = new RWFgraph_Polyline(2);

	*unit2 = *unit_line;
	otable[0] = unit2;
	setEraseInitiator(otable);
}

RWFaf_PolyTree::RWFaf_PolyTree(void)
{
	RWFmat_MatrixTable mtable;
	// First branch, 1/2 length, rotate 30.0
	mtable[0].scale(0.5, 0.5);
	mtable[0].rotate(30.0);
	mtable[0].translate(0.0, 0.3);

	// Second branch, 1/3 length, rotate -45.0
	mtable[1].scale(1.0/3.0, 1.0/3.0);
	mtable[1].rotate(-45.0);
	mtable[1].translate(0.0, 0.6);

	setGenerator(mtable);

	// Now set the initiator, a trapezoidal branch
	RWFlist_ObjectPtrTable otable;
	RWFgraph_Polygon *unit_branch = new RWFgraph_Polygon(5);

	const float bwidth = 0.03, twidth = bwidth / 4.0;
	(*unit_branch)[0].setCoord(-bwidth/2.0 , 0.0);
	(*unit_branch)[1].setCoord(-twidth/2.0, 1.0);
	(*unit_branch)[2].setCoord( twidth/2.0, 1.0);
	(*unit_branch)[3].setCoord( bwidth/2.0 , 0.0);

	otable[0] = unit_branch;
	setInitiator(otable);

	// Now use the same one for the "erase" initiator
	RWFgraph_Polygon *unit2 = new RWFgraph_Polygon(5);

	*unit2 = *unit_branch;
	otable[0] = unit2;
	setEraseInitiator(otable);
};

RWFtree_Object::RWFtree_Object(void)
{
	RWFtree_BranchData *btable = new RWFtree_BranchData [3];

	// Default first branch
	// Short branch near base
	btable[1].length.setMean(0.3);
	btable[1].length.setDeviation(0.1);
	btable[1].offset.setMean(0.1);
	btable[1].offset.setDeviation(0.05);

	// Longer branch near the top
	btable[2].length.setMean(0.4);
	btable[2].length.setDeviation(0.2);
	btable[2].offset.setMean(0.8);
	btable[2].offset.setDeviation(0.05);

	RWFaf_UserDataTable udtable;
	udtable[0] = &(btable[0]);
	udtable[1] = &(btable[1]);
	udtable[2] = &(btable[2]);
	setGenerator(udtable);
	setMatrixFunction(&rwf_treeFindBranchMatrix);

	// Now set the initiator, a trapezoidal branch
	RWFlist_ObjectPtrTable otable;
	RWFgraph_Polygon *unit_branch = new RWFgraph_Polygon(5);

	const float bwidth = 0.03, twidth = bwidth / 4.0;
	(*unit_branch)[0].setCoord(-bwidth/2.0 , 0.0);
	(*unit_branch)[1].setCoord(-twidth/2.0, 1.0);
	(*unit_branch)[2].setCoord( twidth/2.0, 1.0);
	(*unit_branch)[3].setCoord( bwidth/2.0 , 0.0);

	otable[0] = unit_branch;
	setInitiator(otable);

	// Now use the same one for the "erase" initiator
	RWFgraph_Polygon *unit2 = new RWFgraph_Polygon(5);

	*unit2 = *unit_branch;
	otable[0] = unit2;
	setEraseInitiator(otable);

	setAction(&rwf_treeLeafDraw);
}


static RWFgraph_Polygon *leaf = NULL;
static RWFgraph_Polygon *leaf3D = NULL;
static int leaf_color;

void
rwf_treeLeafDraw(RWFgraph_Object *object, int level)
{

	if(!object)
		return;

	if(level < rwf_afGetLastLevel()) {
		object->fill();
	} else {
		if(!leaf) {
			// Create the definition of the leaf
			leaf = new RWFgraph_Polygon(8);
			(*leaf)[0].setCoord( 0.0, 0.0);
			(*leaf)[1].setCoord(-0.1, 0.15);
			(*leaf)[2].setCoord(-0.075, 0.6);
			(*leaf)[3].setCoord(-0.025, 0.8);
			(*leaf)[4].setCoord( 0.0 , 1.0);
			(*leaf)[5].setCoord( 0.025, 0.8);
			(*leaf)[6].setCoord( 0.075, 0.6);
			(*leaf)[7].setCoord( 0.1 , 0.15);
		}
		leaf->setMatrix(object->getMatrix());
		leaf->setColor(leaf_color);
		leaf->fill();
	}

}

void
rwf_treeLeafDraw2(RWFgraph_Object *object, int level)
{

	if(!object)
		return;

	if(level < 1)
		return;

	if(level < rwf_afGetLastLevel()) {
		int savecolor = object->getColor();
		object->setColor(savecolor + level);
		object->fill();
		object->setColor(savecolor);
	} else {
		if(!leaf) {
			// Create the definition of the leaf
			leaf = new RWFgraph_Polygon(8);
			(*leaf)[0].setCoord( 0.0, 0.0);
			(*leaf)[1].setCoord(-0.1, 0.15);
			(*leaf)[2].setCoord(-0.075, 0.6);
			(*leaf)[3].setCoord(-0.025, 0.8);
			(*leaf)[4].setCoord( 0.0 , 1.0);
			(*leaf)[5].setCoord( 0.025, 0.8);
			(*leaf)[6].setCoord( 0.075, 0.6);
			(*leaf)[7].setCoord( 0.1 , 0.15);
		}
		leaf->setMatrix(object->getMatrix());
		int color = RWFMISC_NINT(rwf_randRand() * 15.0) + 16;
		leaf->setColor(color);
		leaf->fill();
	}

}

void
rwf_treeLeafDraw3(RWFgraph_Object *object, int level)
{

	if(!object)
		return;

	if(level < 1)
		return;

	if(level < rwf_afGetLastLevel()) {
		int savecolor = object->getColor();
		object->setColor(savecolor + level);
		object->fill();
		object->setColor(savecolor);
	} else {
		if(!leaf) {
			// Create the definition of the leaf
			leaf = new RWFgraph_Polygon(8);
			(*leaf)[0].setCoord( 0.0, 0.0);
			(*leaf)[1].setCoord(-0.1, 0.15);
			(*leaf)[2].setCoord(-0.075, 0.6);
			(*leaf)[3].setCoord(-0.025, 0.8);
			(*leaf)[4].setCoord( 0.0 , 1.0);
			(*leaf)[5].setCoord( 0.025, 0.8);
			(*leaf)[6].setCoord( 0.075, 0.6);
			(*leaf)[7].setCoord( 0.1 , 0.15);
		}
		leaf->setMatrix(object->getMatrix());
		int color = RWFMISC_NINT(rwf_randRand() * 15.0) + 32;
		leaf->setColor(color);
		leaf->fill();
	}

}

void
rwf_treeSetLeafColor(int color)
{
	if(leaf)
		leaf->setColor(color);
	if(leaf3D)
		leaf3D->setColor(color);
	leaf_color = color;
}

// Branching functions

RWFtree_BranchData::RWFtree_BranchData(void)
{
	id = 0;
	length.setMean(0.5);
	length.setDeviation(0.25);
	angle.setMean(30.0);
	angle.setDeviation(10.0);
	offset.setMean(0.75);
	offset.setDeviation(0.25);
	side.setMean(0.0);
	side.setDeviation(1.0);
}

int
rwf_treeFindBranchMatrix(void *data, int level, RWFmat_Matrix &bmatrix)
{
	RWFtree_BranchData *branch = (RWFtree_BranchData *) data;
	float factor, factor2;

	bmatrix.setIdentity();
	factor = branch->length.next();
	bmatrix.scale(factor, factor);
	factor = branch->angle.next();
	// Use the side paramter to decide whether angle is positive
	// (right side) or negative (left side)
	factor2 = branch->side.next();
	if(factor2 > 0.0)
		factor = -factor;

	bmatrix.rotate(factor);
	// Finally, translate the branch
	factor = RWFMISC_MAX(RWFMISC_MIN(branch->offset.next(), 1.0), 0.0);
	bmatrix.translate(0.0, factor);

	return 1;
}

// 3D tree functions

RWFtree_3DObject::RWFtree_3DObject(void)
{
	RWFtree_BranchData *btable = new RWFtree_BranchData [3];

	// Default first branch
	// Short branch near base
	btable[1].length.setMean(0.3);
	btable[1].length.setDeviation(0.1);
	btable[1].offset.setMean(0.1);
	btable[1].offset.setDeviation(0.05);

	// Longer branch near the top
	btable[2].length.setMean(0.4);
	btable[2].length.setDeviation(0.2);
	btable[2].offset.setMean(0.8);
	btable[2].offset.setDeviation(0.05);

	RWFaf_UserDataTable udtable;
	udtable[0] = &(btable[0]);
	udtable[1] = &(btable[1]);
	udtable[2] = &(btable[2]);
	setGenerator(udtable);
	setMatrixFunction(&rwf_tree3DFindBranchMatrix);

	// Now set the initiator, a trapezoidal branch
	RWFlist_ObjectPtrTable otable;
	RWFgraph_Cube *unit_branch = new RWFgraph_Cube;

	unit_branch->translate(0.0, 0.0, 0.5);
	unit_branch->scale(0.01, 0.01, 1.0);
	unit_branch->setColor(15);
	// Set all the polygon colors to 0
	for(int i=0; i < unit_branch->size(); i++)
		(*unit_branch)[i]->setColor(0);

	// Now set a better reference point
	RWFvec_Vector ref_point(3);
	ref_point[2] = 0.5;
	unit_branch->setReferencePoint(ref_point);

	otable[0] = unit_branch;
	setInitiator(otable);

	// Now use the same one for the "erase" initiator
	RWFgraph_Cube *unit2 = new RWFgraph_Cube;

	*unit2 = *unit_branch;

	otable[0] = unit2;
	setEraseInitiator(otable);

	make3D();
	set3DAction(&rwf_tree3DLeafDraw);
}

int
rwf_tree3DFindBranchMatrix(void *data, int level, RWFmat_Matrix &bmatrix)
{
	RWFtree_BranchData *branch = (RWFtree_BranchData *) data;
	float factor;

	bmatrix.identity();
	factor = branch->length.next();
	// Scale only in the z-direction
	bmatrix.scale(factor, factor, factor);

	// Add an initial orientation matrix
	factor = branch->side.next();
	bmatrix.rotate(0.0, 0.0, factor * 180.0);

	factor = branch->angle.next();
	// First, rotate about the y-axis
	bmatrix.rotate(0.0, factor, 0.0);

	// Use the side paramter to determine a rotation
	// angle about the z-axis
	// Now, rotate about the z-axis
	factor = branch->side.next();
	bmatrix.rotate(0.0, 0.0, factor * 180.0);

	// Finally, translate the branch along the z-axis
	factor = RWFMISC_MAX(RWFMISC_MIN(branch->offset.next(), 1.0), 0.0);

	bmatrix.translate(0.0, 0.0, factor);

	return 1;
}

void
rwf_tree3DLeafDraw(RWFgraph_Object *object, int level, RWFgraph_ViewingGeometry &vg)
{
	static RWFmat_Matrix leaf3D_matrix(3,3);

	if(!object)
		return;

	if(level < rwf_afGetLastLevel()) {
		int save_color = object->getColor();
		object->setColor(level + save_color);
		object->draw(vg);
		object->setColor(save_color);
	} else {
		if(!leaf3D) {
			// Create the definition of the leaf
			leaf3D = new RWFgraph_Polygon(8);
			leaf3D->make3D();
			(*leaf3D)[0].setCoord( 0.0, 0.0, 0.0);
			(*leaf3D)[1].setCoord(-0.1, 0.0, 0.15);
			(*leaf3D)[2].setCoord(-0.075, 0.0, 0.6);
			(*leaf3D)[3].setCoord(-0.025, 0.0, 0.8);
			(*leaf3D)[4].setCoord( 0.0  , 0.0, 1.0);
			(*leaf3D)[5].setCoord( 0.025, 0.0, 0.8);
			(*leaf3D)[6].setCoord( 0.075, 0.0, 0.6);
			(*leaf3D)[7].setCoord( 0.1  , 0.0, 0.15);
			leaf3D->setColor(2);
			// We must compensate for the fact that the branches
			// are scaled cubes.  This inverts the cube scaling
			leaf3D_matrix.scale(1.0/.02, 1.0/.02, 1.0);
			leaf3D_matrix.translate(0.0, 0.0, -0.5);
		}
		leaf3D->setMatrix(object->getMatrix() * leaf3D_matrix);
		leaf3D->setColor(leaf_color);
		leaf3D->draw(vg);
	}

}

/*
**	Crater functions
*/

RWFtree_CraterData::RWFtree_CraterData(void)
{
	prob_occuring.setMean(0.5);
	prob_occuring.setDeviation(0.5);

	distance.setMean(0.23);
	distance.setDeviation(0.15);

	xscale.setMean(0.25);
	xscale.setDeviation(0.15);
	yscale.setMean(0.25);
	yscale.setDeviation(0.15);

	angle.setMean(180.0);
	angle.setDeviation(180.0);
	orientation.setMean(180.0);
	orientation.setDeviation(180.0);
}

#define RWFTREE_CRATER_DEFAULT_CRATERS	20

RWFtree_CraterObject::RWFtree_CraterObject(void)
{
	RWFtree_CraterData *btable =
		new RWFtree_CraterData [RWFTREE_CRATER_DEFAULT_CRATERS];

	RWFaf_UserDataTable udtable;
	for(int i=0; i < RWFTREE_CRATER_DEFAULT_CRATERS; i++)
		udtable[i] = &(btable[i]);
	setGenerator(udtable);
	setMatrixFunction(&rwf_treeFindCraterMatrix);

	// Now set the initiator, a unit line segment
	RWFlist_ObjectPtrTable otable;
	RWFgraph_Circle *unit_circle = new RWFgraph_Circle(0.0, 0.0, 0.5);

	otable[0] = unit_circle;
	setInitiator(otable);

	// Now use the same one for the "erase" initiator
	RWFgraph_Circle *unit2 = new RWFgraph_Circle(0.0, 0.0, 0.5);

	unit2->setColor(RWFGRAPH_COLOR_LIGHT_GRAY);
	otable[0] = unit2;
	setEraseInitiator(otable);
}

int
rwf_treeFindCraterMatrix(void *data, int level, RWFmat_Matrix &bmatrix)
{
	RWFtree_CraterData *cluster = (RWFtree_CraterData *) data;
	float dist, angle, xs, ys, max_scale;

	if(cluster->prob_occuring.next() < 0.2)
		return 0;

	bmatrix.setIdentity();
	xs = -1.0; ys = -1.0;
	while(xs < 0.0)
		xs = cluster->xscale.next();
	while(ys < 0.0)
		ys = cluster->yscale.next();
	bmatrix.scale(xs, ys);
	bmatrix.rotate(cluster->orientation.next());
	//Finally, position it relative to the current center
	// The angle is uniformly distributed, but the distance
	// from the center is normally distributed
	angle = cluster->angle.next();
	max_scale = RWFMISC_MAX(xs, ys);
	dist = 1.0 + max_scale;
	while((dist + max_scale) > 0.5)
		dist = ABS(cluster->distance.next());
	float xtrans = cosD(angle)*dist;
	float ytrans = sinD(angle)*dist;
	bmatrix.translate(xtrans, ytrans);

	return 1;
}

void
rwf_treeCraterDraw(RWFgraph_Object *object, int level)
{
	if(!object)
		return;

	if(level == 1 && object->getColor() != RWFGRAPH_COLOR_WHITE)
		return;

	if(level == 0)
		object->setColor(RWFGRAPH_COLOR_WHITE);
	else if(level == 2)
		// Skip this level
		return;
	else
		object->setColor(LIGHTGRAY);

	object->fill();

	return;
}

// 3D Crater functions

RWFtree_3DCraterData::RWFtree_3DCraterData(void)
{
	prob_occuring.setMean(0.5);
	prob_occuring.setDeviation(0.5);

	distance.setMean(0.3);
	distance.setDeviation(0.1);

	xscale.setMean(0.7);
	xscale.setDeviation(0.25);
	yscale.setMean(0.7);
	yscale.setDeviation(0.25);
	zscale.setMean(0.3);
	zscale.setDeviation(0.1);

	xangle.setMean(0.0);
	xangle.setDeviation(180.0);
	yangle.setMean(0.0);
	yangle.setDeviation(180.0);
	zangle.setMean(0.0);
	zangle.setDeviation(20.0);

}

#define RWFTREE_3DCRATER_DEFAULT_CRATERS	7

RWFtree_3DCraterObject::RWFtree_3DCraterObject(void)
{
	RWFtree_3DCraterData *btable =
		new RWFtree_3DCraterData [RWFTREE_3DCRATER_DEFAULT_CRATERS];

	RWFaf_UserDataTable udtable;
	for(int i=0; i < RWFTREE_3DCRATER_DEFAULT_CRATERS; i++)
		udtable[i] = &(btable[i]);
	setGenerator(udtable);
	setMatrixFunction(&rwf_treeFind3DCraterMatrix);

	// Now set the initiator, a unit line segment
	RWFlist_ObjectPtrTable otable;
	RWFgraph_Circle *unit_circle = new RWFgraph_Circle(0.0, 0.0, 0.0, 0.5);

	otable[0] = unit_circle;
	setInitiator(otable);
	make3D();
	setColor(15);
}

int
rwf_treeFind3DCraterMatrix(void *data, int level, RWFmat_Matrix &bmatrix)
{
	RWFtree_3DCraterData *cluster = (RWFtree_3DCraterData *) data;
	float dist, xangle, zangle, xs, ys, zs;

	bmatrix.identity();
	xs = ABS(cluster->xscale.next());
	ys = ABS(cluster->yscale.next());
	zs = ABS(cluster->zscale.next());
	bmatrix.scale(xs, ys, zs);

	//Finally, position it relative to the current center
	// The angle is uniformly distributed, but the distance
	// from the center is normally distributed
	xangle = cluster->xangle.next();
	zangle = cluster->zangle.next();
	dist = cluster->distance.next();
	float xtrans = cosD(zangle)*cosD(xangle)*dist;
	float ytrans = cosD(zangle)*sinD(xangle)*dist;
	float ztrans = sinD(zangle)*dist;
	bmatrix.translate(xtrans, ytrans, ztrans);

	return 1;
}


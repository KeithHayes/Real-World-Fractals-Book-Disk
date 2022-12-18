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

#include "rwfsg.hpp"
#include "rwfgraph.hpp"
#include "rwfmat.hpp"

#include <math.h>



/*
**	Functions to support class RWFsg_Gasket
*/
//	Void constructor for the Sierpinski Gasket
//	This sets both the generator and the initiator
RWFsg_Gasket::RWFsg_Gasket(void)
{
	RWFmat_MatrixTable mtable;
	float height = sqrt(.75);

	//	Define the generator by a series of transformations
	//	operating on a centered triangle
	//	First, scale by 1/2
	mtable[0].scale(1/2.0,1/2.0);
	//	Now translate to the lower corner
	mtable[0].translate(-0.25,-height/4.0);
	//	Second triangle, scale and translate to other corner
	mtable[1].scale(1/2.0,1/2.0);
	mtable[1].translate(0.25,-height/4.0);
	//	Third triangle, scale and translate vertically
	mtable[2].scale(1/2.0,1/2.0);
	mtable[2].translate(0.0,height/4.0);

	setGenerator(mtable);

	//	Now set the initiator, a simple unit-sided triangle
	RWFlist_ObjectPtrTable otable;
	RWFgraph_Polygon *triangle = new RWFgraph_Polygon(3);

	//	Note that the initiator must be allocated by new
	//	so that the object pointer table can be appropriately
	//	destroyed when the object is destroyed

	(*triangle)[0].setCoord(-0.5,-height/2.0);
	(*triangle)[1].setCoord( 0.5,-height/2.0);
	(*triangle)[2].setCoord( 0.0, height/2.0);
	otable[0] = triangle;
	setInitiator(otable);
}


/*
**	Functions to support class RWFsg_Carpet
*/
//	Void constructor for the Sierpinski Carpet
//	This sets both the generator and the initiator
RWFsg_Carpet::RWFsg_Carpet(void)
{
	RWFmat_MatrixTable mtable;

	//	Define the generator by a series of transformations
	//	operating on a centered square
	mtable[0].scale(1/3.0,1/3.0);
	mtable[0].translate(-1/3.0,-1/3.0);
	mtable[1].scale(1/3.0,1/3.0);
	mtable[1].translate(0.0,-1/3.0);
	mtable[2].scale(1/3.0,1/3.0);
	mtable[2].translate(1/3.0,-1/3.0);

	mtable[3].scale(1/3.0,1/3.0);
	mtable[3].translate(-1/3.0,1/3.0);
	mtable[4].scale(1/3.0,1/3.0);
	mtable[4].translate(0.0,1/3.0);
	mtable[5].scale(1/3.0,1/3.0);
	mtable[5].translate(1/3.0,1/3.0);

	mtable[6].scale(1/3.0,1/3.0);
	mtable[6].translate(-1/3.0,0.0);
	mtable[7].scale(1/3.0,1/3.0);
	mtable[7].translate(1/3.0,0.0);

	setGenerator(mtable);

	//	Now set the initiator, a simple unit-sided square
	RWFlist_ObjectPtrTable otable;
	RWFgraph_Polygon *square = new RWFgraph_Polygon(4);

	//	Note that the initiator must be allocated by new
	//	so that the object pointer table can be appropriately
	//	destroyed when the object is destroyed

	(*square)[0].setCoord(-0.5,-0.5);
	(*square)[1].setCoord( 0.5,-0.5);
	(*square)[2].setCoord( 0.5, 0.5);
	(*square)[3].setCoord(-0.5, 0.5);
	otable[0] = square;
	setInitiator(otable);
}


/*
**	Functions to support class RWFsg_ReverseGasket
*/
//	Void constructor for the Sierpinski Gasket
//	This sets both the generator and the initiator
RWFsg_ReverseGasket::RWFsg_ReverseGasket(void)
{
	RWFmat_MatrixTable mtable;
	float height = sqrt(.75);

	//	Define the generator by a series of transformations
	//	operating on a centered triangle
	//	First, scale by 1/2
	mtable[0].scale(1/2.0,1/2.0);
	//	Now translate to the lower corner
	mtable[0].translate(-0.25,-height/4.0);
	//	Second triangle, scale and translate to other corner
	mtable[1].scale(1/2.0,1/2.0);
	mtable[1].translate(0.25,-height/4.0);
	//	Third triangle, scale and translate vertically
	mtable[2].scale(1/2.0,1/2.0);
	mtable[2].translate(0.0,height/4.0);

	setGenerator(mtable);

	//	Now set the initiator, a simple unit-sided triangle
	RWFlist_ObjectPtrTable otable;
	RWFgraph_Polygon *triangle = new RWFgraph_Polygon(3);

	//	Note that the initiator must be allocated by new
	//	so that the object pointer table can be appropriately
	//	destroyed when the object is destroyed

	(*triangle)[0].setCoord(-0.5,-height/2.0);
	(*triangle)[1].setCoord( 0.5,-height/2.0);
	(*triangle)[2].setCoord( 0.0, height/2.0);
	otable[0] = triangle;
	setInitiator(otable);

	//	Use the same triangle as the initiator
	RWFgraph_Polygon *copy_tri = new RWFgraph_Polygon(3);
	RWFlist_ObjectPtrTable etable;
	(*copy_tri) = (*triangle);
	copy_tri->rotate(180.0);	//	Rotate by 180 degrees
	copy_tri->scale(0.5,0.5);	//	Scale by 1/2
	copy_tri->translate(0.0,-height/4.0);
	copy_tri->setColor(RWFGRAPH_COLOR_BLACK);
	etable[0] = copy_tri;
	setEraseInitiator(etable);
}


/*
**	Functions to support class RWFsg_ReverseCarpet
*/
//	Void constructor for the Sierpinski Carpet
//	This sets both the generator and the initiator
RWFsg_ReverseCarpet::RWFsg_ReverseCarpet(void)
{
	RWFmat_MatrixTable mtable;

	//	Define the generator by a series of transformations
	//	operating on a centered square
	mtable[0].scale(1/3.0,1/3.0);
	mtable[0].translate(-1/3.0,-1/3.0);
	mtable[1].scale(1/3.0,1/3.0);
	mtable[1].translate(0.0,-1/3.0);
	mtable[2].scale(1/3.0,1/3.0);
	mtable[2].translate(1/3.0,-1/3.0);

	mtable[3].scale(1/3.0,1/3.0);
	mtable[3].translate(-1/3.0,1/3.0);
	mtable[4].scale(1/3.0,1/3.0);
	mtable[4].translate(0.0,1/3.0);
	mtable[5].scale(1/3.0,1/3.0);
	mtable[5].translate(1/3.0,1/3.0);

	mtable[6].scale(1/3.0,1/3.0);
	mtable[6].translate(-1/3.0,0.0);
	mtable[7].scale(1/3.0,1/3.0);
	mtable[7].translate(1/3.0,0.0);

	setGenerator(mtable);

	//	Now set the initiator, a simple unit-sided square
	RWFlist_ObjectPtrTable otable;
	RWFgraph_Polygon *square = new RWFgraph_Polygon(4);

	//	Note that the initiator must be allocated by new
	//	so that the object pointer table can be appropriately
	//	destroyed when the object is destroyed

	(*square)[0].setCoord(-0.5,-0.5);
	(*square)[1].setCoord( 0.5,-0.5);
	(*square)[2].setCoord( 0.5, 0.5);
	(*square)[3].setCoord(-0.5, 0.5);
	otable[0] = square;
	setInitiator(otable);

	//	Use the same square as the initiator
	RWFgraph_Polygon *copy_sq = new RWFgraph_Polygon(4);
	RWFlist_ObjectPtrTable etable;
	(*copy_sq) = (*square);
	copy_sq->scale(1/3.0,1/3.0);	//	Scale by 1/3
	copy_sq->setColor(RWFGRAPH_COLOR_BLACK);
	etable[0] = copy_sq;
	setEraseInitiator(etable);
}


/*
**	Functions to support class RWFsg_SpiralHexagon
*/
RWFsg_SpiralHexagon::RWFsg_SpiralHexagon(void)
{
	RWFmat_MatrixTable mtable;
	float x;

	x = sqrt(2.0);
	mtable[0].scale(1/x,1/x);
	mtable[0].rotate(30.0);
	x = 1.5;
	mtable[0].translate(-x,x);
	mtable[1].scale(1.0,1.0);

	setGenerator(mtable);

	//	Now set the initiator, an offset hexagon
	RWFlist_ObjectPtrTable otable;
	RWFgraph_Polygon *hex = new RWFgraph_Polygon(6);

	//	Note that the initiator must be allocated by new
	//	so that the object pointer table can be appropriately
	//	destroyed when the object is destroyed

	x = sqrt(3.0)/2.0;
	(*hex)[0].setCoord( 1.0,0.0);
	(*hex)[1].setCoord( 0.5,x);
	(*hex)[2].setCoord(-0.5,x);
	(*hex)[3].setCoord(-1.0,0.0);
	(*hex)[4].setCoord(-0.5,-x);
	(*hex)[5].setCoord( 0.5,-x);
	(*hex).scale(0.25,0.25);
	(*hex).translate(0.5,-0.25);
	otable[0] = hex;
	setInitiator(otable);
}

// Functions to support 3D gasket
RWFsg_3DGasket::RWFsg_3DGasket(void)
{
	RWFmat_MatrixTable mtable;
	RWFmat_Matrix tmat(3,3);
	const float height = sqrt(3.0)/2.0;
	const float zheight = sqrt(6.0)/3.0;

	//	Define the generator by a series of transformations
	//	operating on a centered tetrahedron
	//	First, scale by 1/2
	mtable[0] = tmat;
	mtable[0].scale(1/2.0,1/2.0,1/2.0);
	//	Now translate to the lower corner
	mtable[0].translate(-height/6.0, -0.25, -zheight/4.0);
	//	Second triangle, scale and translate to other corner
	mtable[1] = tmat;
	mtable[1].scale(1/2.0,1/2.0, 1/2.0);
	mtable[1].translate(-height/6.0, 0.25, -zheight/4.0);
	//	Third triangle, scale and translate vertically
	mtable[2] = tmat;
	mtable[2].scale(1/2.0,1/2.0, 1/2.0);
	mtable[2].translate(height/3.0, 0.0, -zheight/4.0);

	mtable[3] = tmat;
	mtable[3].scale(1/2.0, 1/2.0, 1/2.0);
	mtable[3].translate(0.0, 0.0, zheight/4.0);

	setGenerator(mtable);

	//	Now set the initiator, a tetrahedron
	RWFlist_ObjectPtrTable otable;
	RWFgraph_Tetrahedron *tetra = new RWFgraph_Tetrahedron;

	//	Note that the initiator must be allocated by new
	//	so that the object pointer table can be appropriately
	//	destroyed when the object is destroyed

	otable[0] = tetra;
	setInitiator(otable);

	make3D();

}

RWFsg_3DReverseGasket::RWFsg_3DReverseGasket(void)
{
	RWFmat_MatrixTable mtable;
	RWFmat_Matrix tmat(3,3);
	float height = sqrt(3.0)/2.0;
	const float zheight = sqrt(6.0)/3.0;

	//	Define the generator by a series of transformations
	//	operating on a body-centered tetrahedron
	//	First, scale by 1/2
	mtable[0] = tmat;
	mtable[0].scale(1/2.0,1/2.0,1/2.0);
	//	Now translate to the lower corner
	mtable[0].translate(-height/6.0, -0.25, -zheight/4.0);
	//	Second triangle, scale and translate to other corner
	mtable[1] = tmat;
	mtable[1].scale(1/2.0,1/2.0, 1/2.0);
	mtable[1].translate(-height/6.0, 0.25, -zheight/4.0);
	//	Third triangle, scale and translate vertically
	mtable[2] = tmat;
	mtable[2].scale(1/2.0,1/2.0, 1/2.0);
	mtable[2].translate(height/3.0, 0.0, -zheight/4.0);

	mtable[3] = tmat;
	mtable[3].scale(1/2.0, 1/2.0, 1/2.0);
	mtable[3].translate(0.0, 0.0, zheight/4.0);

	setGenerator(mtable);

	//	Now set the initiator, a tetrahedron
	RWFlist_ObjectPtrTable otable;
	RWFgraph_Tetrahedron *tetra = new RWFgraph_Tetrahedron;

	//	Note that the initiator must be allocated by new
	//	so that the object pointer table can be appropriately
	//	destroyed when the object is destroyed

	otable[0] = tetra;
	setInitiator(otable);

	tetra = new RWFgraph_Tetrahedron;
	tetra->rotate(180.0, 0.0, 0.0);
	tetra->scale(1/2.0, 1/2.0, 1/2.0);
  tetra->translate(0.0, 0.0, -zheight/4.0);
	otable[0] = tetra;
	setEraseInitiator(otable);

	make3D();

}

RWFsg_MengerSponge::RWFsg_MengerSponge(void)
{
	RWFmat_MatrixTable mtable;
	RWFmat_Matrix tmat(3,3);

	int n = 0;
	for(int i = -1; i <= 1; i++) {
		for(int j = -1; j <= 1; j++) {
			for(int k = -1; k <= 1; k++) {
				if(i == 0) {
					if(j == 0 || k == 0)
						continue;
				}
				if(j == 0 && k == 0)
					continue;

				mtable[n] = tmat;
				mtable[n].scale(1/3.0, 1/3.0, 1/3.0);
				mtable[n].translate(i/3.0, j/3.0, k/3.0);
				n++;
			}
		}
	}

	setGenerator(mtable);

	//	Now set the initiator, a tetrahedron
	RWFlist_ObjectPtrTable otable;
	RWFgraph_Cube *cube = new RWFgraph_Cube;

	//	Note that the initiator must be allocated by new
	//	so that the object pointer table can be appropriately
	//	destroyed when the object is destroyed

	otable[0] = cube;
	setInitiator(otable);

	make3D();
}

RWFsg_ReverseMengerSponge::RWFsg_ReverseMengerSponge(void)
{
	RWFmat_MatrixTable mtable;
	RWFmat_Matrix tmat(3,3);

	int n = 0;
	for(int i = -1; i <= 1; i++) {
		for(int j = -1; j <= 1; j++) {
			for(int k = -1; k <= 1; k++) {
				if(i == 0) {
					if(j == 0 || k == 0)
						continue;
				}
				if(j == 0 && k == 0)
					continue;

				mtable[n] = tmat;
				mtable[n].scale(1/3.0, 1/3.0, 1/3.0);
				mtable[n].translate(i/3.0, j/3.0, k/3.0);
				n++;
			}
		}
	}

	setGenerator(mtable);

	//	Now set the erase initiator, a tetrahedron
	RWFlist_ObjectPtrTable otable;

	RWFgraph_Cube *cube = new RWFgraph_Cube;

	//	Note that the initiator must be allocated by new
	//	so that the object pointer table can be appropriately
	//	destroyed when the object is destroyed

	otable[0] = cube;
	setInitiator(otable);

	// Fill in the holes
	cube = new RWFgraph_Cube;
	cube->scale(1/3.0, 1/3.0, 1/3.0);
	cube->translate(1/3.0, 0.0, 0.0);
	otable[0] = cube;
	cube = new RWFgraph_Cube;
	cube->scale(1/3.0, 1/3.0, 1/3.0);
	cube->translate(-1/3.0, 0.0, 0.0);
	otable[1] = cube;
	cube = new RWFgraph_Cube;
	cube->scale(1/3.0, 1/3.0, 1/3.0);
	cube->translate(0.0,  1/3.0, 0.0);
	otable[2] = cube;
	cube = new RWFgraph_Cube;
	cube->scale(1/3.0, 1/3.0, 1/3.0);
	cube->translate(0.0, -1/3.0, 0.0);
	otable[3] = cube;
	cube = new RWFgraph_Cube;
	cube->scale(1/3.0, 1/3.0, 1/3.0);
	cube->translate(0.0, 0.0, 1/3.0);
	otable[4] = cube;
	cube = new RWFgraph_Cube;
	cube->scale(1/3.0, 1/3.0, 1/3.0);
	cube->translate(0.0, 0.0, -1/3.0);
	otable[5] = cube;
	cube = new RWFgraph_Cube;

	setEraseInitiator(otable);

	make3D();
}

void rwf_sg3DDrawSpecial1(RWFgraph_Object *object, int level,
													RWFgraph_ViewingGeometry &vg)
{
	static RWFsg_3DGasket gasket;

	if(!object)
		return;

	if(level > 1) {
		object->setColor(level - 1);
		object->draw(vg);
	} else {
		gasket.setMaximumLevel(3);
		gasket.setMinimumSize(0.01);
		gasket.identity();
		gasket.scale(1.5, 1.5, 1.5);
		gasket.rotate(0.0, 0.0, -120.0);
		gasket.translate(0.0, 0.0, -sqrt(3)/6.0);
		gasket.setMatrix(object->getMatrix() * gasket.getMatrix());
		gasket.setColor(level-1);
		gasket.draw(vg);
	}

	return;
}

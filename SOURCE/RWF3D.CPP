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


#include "rwfgraph.hpp"

/*
**	Viewing Geometry functions
*/

RWFgraph_ViewingGeometry::RWFgraph_ViewingGeometry(void)
{
	static RWFvec_Vector tvec(3);
	static RWFmat_Matrix tmat(3,3);

	// Eye position at the origin
	eyepos = tvec;

	// Eye direction along +x axis
	eyedir = tvec;
	eyedir[0] = 1.0;

	xfov = 90.0;
	yfov = 80.0;

	forward_matrix = tmat;
	rotate_matrix  = tmat;
	coord_matrix   = tmat;

	light_direction = tvec;
	light_direction[0] = -1.0;

	ambient = 0.5;
	contrast = 0.5;
	orientation = 0.0;

	xul = yul = xlr = ylr = -1;

	isSetup = RWFMISC_FALSE;
}

void
RWFgraph_ViewingGeometry::setup(void)
{
	if(isSetup)
		return;

	isSetup = RWFMISC_TRUE;

	forward_matrix.identity();
	rotate_matrix.identity();

	eyedir.normalize();
	float sinp2, cosp2, sinh2, cosh2;

	sinp2 = eyedir[2];
	cosp2 = sqrt(1.0 - sinp2*sinp2);
	float rlength = sqrt(eyedir[0]*eyedir[0] + eyedir[1]*eyedir[1]);
	if(rlength > 0.0) {
		sinh2 =  eyedir[1]/rlength;
		cosh2 =  eyedir[0]/rlength;
	} else {
		sinh2 = 0.0;
		cosh2 = 1.0;
	}

	float zangle = atan2(sinh2, cosh2) * RWFMISC_RADIANS_TO_DEGREES;
	float yangle = atan2(sinp2, cosp2) * RWFMISC_RADIANS_TO_DEGREES;

	forward_matrix.translate(-eyepos[0], -eyepos[1], -eyepos[2]);
	rotate_matrix.rotate(-orientation, 0.0, 0.0);
	rotate_matrix.rotate(0.0, 0.0, -zangle);
	rotate_matrix.rotate(0.0, -yangle, 0.0);

	forward_matrix *= rotate_matrix;

	// Now create final screen projection matrix;

	coord_matrix.identity();

	if(xul < 0) {
		xul = yul = 0;
		xlr = rwf_graphGetMaxX() - 1;
		ylr = rwf_graphGetMaxY() - 1;
	}

	float xtan = -(float)(xlr - xul) / tan((xfov/2.0) * RWFMISC_DEGREES_TO_RADIANS);
	float ytan = -(float)(ylr - yul) / tan((yfov/2.0) * RWFMISC_DEGREES_TO_RADIANS);

	coord_matrix.scale(xtan, ytan, 1.0);
	float xmid = (xlr + xul)/2.0;
	float ymid = (ylr + yul)/2.0;
	coord_matrix.translate(xmid, ymid, 0.0);

	light_direction.normalize();

	return;
}

RWFvec_Vector
RWFgraph_ViewingGeometry::getProjectedCoordinates(RWFvec_Vector &vin)
{
	RWFvec_Vector tvec(3);
	float u, v, z;

	if(!isSetup)
		setup();

	tvec = forward_matrix * vin;

	// Ok, get coordinates in the right order
	u = tvec[1];
	v = tvec[2];
	z = tvec[0];
	tvec[0] = u;
	tvec[1] = v;
	tvec[2] = z;

	return tvec;
}

void
RWFgraph_ViewingGeometry::projectCoords(RWFvec_Vector &vin,
																				int &xscreen, int &yscreen)
{
	projectToScreen(getProjectedCoordinates(vin), xscreen, yscreen);
}

void
RWFgraph_ViewingGeometry::projectToScreen(RWFvec_Vector &uvz,
																					int &xscreen, int &yscreen)
{
	static RWFvec_Vector tvec(3);

	if(uvz[2] > 0.0) {
		float zinverse = 1.0 / uvz[2];
		tvec[0] = uvz[0] * zinverse;
		tvec[1] = uvz[1] * zinverse;
	} else {
		xscreen = xul;
		yscreen = yul;
		return;
	}

	tvec = coord_matrix * tvec;
	xscreen = RWFMISC_NINT(tvec[0]);
	yscreen = RWFMISC_NINT(tvec[1]);

	return;
}

int
RWFgraph_ViewingGeometry::computeIntensity(RWFvec_Vector &normal)
{
	float dot = (normal * light_direction) / normal.length();

	float intensity = RWFMISC_MAX(dot, 0.0)*contrast + ambient;

	return (RWFMISC_MAX(RWFMISC_MIN(intensity, 1.0), 0.0) * 255);
}

/*
**	Default 3D object constructors
*/

RWFgraph_Tetrahedron::RWFgraph_Tetrahedron(void)
{
	RWFgraph_Polygon triangle(3), *poly;

	// Unlike the text, this creates a tetrahedron that is centered
	// at the origin.  This makes it consistent with the the other
	// 3D objects which are also all body-centered.

	// Define the principal vertex coordinates

	const float xtri1 =  sqrt(3.0) / 3.0;
	const float xtri2 = -sqrt(3.0) / 6.0;
	const float ytri  =  0.5;
	const float z = sqrt(6.0)/6.0;

	// First, create the bottom piece
	triangle[0].setCoord(xtri1, 0, -z);
	triangle[1].setCoord(xtri2, -ytri, -z);
	triangle[2].setCoord(xtri2, ytri, -z);
	triangle.setColor(7);

	// Create a new polygon copy
	poly = new RWFgraph_Polygon(3);
	*poly = triangle;
	(*this)[0] = poly;

	// Now, create the 3 sides
	triangle[0].setCoord(0, 0, z);
	triangle[1].setCoord(xtri1, 0, -z);
	triangle[2].setCoord(xtri2, ytri, -z);
	triangle.setColor(4);

	poly = new RWFgraph_Polygon(3);
	*poly = triangle;
	(*this)[1] = poly;

	triangle[1].setCoord(xtri2, ytri, -z);
	triangle[2].setCoord(xtri2, -ytri, -z);
	triangle.setColor(1);

	poly = new RWFgraph_Polygon(3);
	*poly = triangle;
	(*this)[2] = poly;

	triangle[1].setCoord(xtri2, -ytri, -z);
	triangle[2].setCoord(xtri1, 0, -z);
	triangle.setColor(14);
	poly = new RWFgraph_Polygon(3);
	*poly = triangle;
	(*this)[3] = poly;

	make3D();

	return;
}

RWFgraph_Cube::RWFgraph_Cube(void)
{
	RWFgraph_Polygon square(4), *poly;

	make3D();

	// Make the bottom
	square[0].setCoord( 0.5,  0.5, -0.5);
	square[1].setCoord( 0.5, -0.5, -0.5);
	square[2].setCoord(-0.5, -0.5, -0.5);
	square[3].setCoord(-0.5,  0.5, -0.5);
	poly = new RWFgraph_Polygon(4);
	*poly = square;
	poly->setColor(15);
	(*this)[0] = poly;

	// Make the top
	square[0].setCoord( 0.5,  0.5, 0.5);
	square[1].setCoord(-0.5,  0.5, 0.5);
	square[2].setCoord(-0.5, -0.5, 0.5);
	square[3].setCoord( 0.5, -0.5, 0.5);
	poly = new RWFgraph_Polygon(4);
	*poly = square;
	poly->setColor(1);
	(*this)[1] = poly;

	// Make the sides
	square[0].setCoord( 0.5,  0.5,  0.5);
	square[1].setCoord( 0.5,  0.5, -0.5);
	square[2].setCoord(-0.5,  0.5, -0.5);
	square[3].setCoord(-0.5,  0.5,  0.5);
	poly = new RWFgraph_Polygon(4);
	*poly = square;
	poly->setColor(2);
	(*this)[2] = poly;

	square[0].setCoord( 0.5, -0.5,  0.5);
	square[1].setCoord(-0.5, -0.5,  0.5);
	square[2].setCoord(-0.5, -0.5, -0.5);
	square[3].setCoord( 0.5, -0.5, -0.5);
	poly = new RWFgraph_Polygon(4);
	*poly = square;
	poly->setColor(4);
	(*this)[3] = poly;

	square[0].setCoord( 0.5,  0.5,  0.5);
	square[1].setCoord( 0.5, -0.5,  0.5);
	square[2].setCoord( 0.5, -0.5, -0.5);
	square[3].setCoord( 0.5,  0.5, -0.5);
	poly = new RWFgraph_Polygon(4);
	*poly = square;
	poly->setColor(14);
	(*this)[4] = poly;

	square[0].setCoord(-0.5,  0.5,  0.5);
	square[1].setCoord(-0.5,  0.5, -0.5);
	square[2].setCoord(-0.5, -0.5, -0.5);
	square[3].setCoord(-0.5, -0.5,  0.5);
	poly = new RWFgraph_Polygon(4);
	*poly = square;
	poly->setColor(7);
	(*this)[5] = poly;

	return;
}


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

#include <math.h>

static RWFmat_MatrixTable dummy_table;

/*
**	Utility Functions
*/

float
rwf_matGetGlobalScalingFactor(RWFmat_Matrix &matrix)
{
	float det = ABS(matrix.determinant());
	return pow(det, 1.0 / (float)matrix.ncols());
}

// Finds the transformation from the source line segment
// to the destination line segment
RWFmat_Matrix
rwf_graphLineToLineMatrix(RWFgraph_Line &lSrc, RWFgraph_Line &lDest)
{
	RWFmat_Matrix temp_matrix(2,2), rot_matrix(2,2);
	float x1, y1, x2, y2, xd1, xd2, yd1, yd2;
	float dist1, dist2, sinang, cosang;

	// First, translate the source line segment to the
	// origin
	lSrc.getFirst(x1, y1);
	temp_matrix.translate(-x1, -y1);

	// Now make it the same length as the destination line segment
	lSrc.getEnd(x2, y2);
	dist1 = lSrc.length();
	dist2 = lDest.length();
	if(dist1 == 0.0)
		return temp_matrix;
	temp_matrix.scale(dist2/dist1, dist2/dist1);

	// Now, rotate to line up with the other line segment
	// First, find the sin/cos of the angles
	lDest.getFirst(xd1, yd1);
	lDest.getEnd(xd2, yd2);
	// cosine is dot product, sine is cross-product
	cosang = (x2 - x1)*(xd2 - xd1) + (y2 - y1)*(yd2 - yd1);
	sinang = (x2 - x1)*(yd2 - yd1) - (xd2 - xd1)*(y2 - y1);
	rot_matrix[0][0] = cosang / (dist1*dist2);
	rot_matrix[1][1] = cosang / (dist1*dist2);
	rot_matrix[0][1] = -sinang / (dist1*dist2);
	rot_matrix[1][0] = sinang / (dist1*dist2);
	temp_matrix = rot_matrix * temp_matrix;

	// Finally, translate to the destination vector
	temp_matrix.translate(xd1, yd1);

	return temp_matrix;
}

void
rwf_matPolylineToMatrixTable(RWFgraph_Polyline &poly,
														 RWFmat_MatrixTable &mtable)
{
	static RWFgraph_Line l1, l2;

	if(poly.numPoints() < 2)
		return;

	l1.setFirst(poly[0]);
	l1.setEnd(poly[poly.numPoints() - 1]);
	for(int i=0; i < poly.numPoints() - 1; i++) {
		l2.setFirst(poly[i]);
		l2.setEnd(poly[i+1]);
		mtable[i] = rwf_graphLineToLineMatrix(l1, l2);
	}

	return;
}

float
RWFmat_Matrix::determinant(void)
{
	float x = 0.0;
	if (numRows == 2) {;
		x = elements[0][0]*elements[1][1] - elements[1][0]*elements[0][1];
	} else if (numRows == 3) {
		float t;
		t = elements[0][0];
		t *= elements[1][1]*elements[2][2] - elements[2][1]*elements[1][2];
		x += t;
		t = -elements[0][1];
		t *= elements[1][0]*elements[2][2] - elements[2][0]*elements[1][2];
		x += t;
		t = elements[0][2];
		t *= elements[0][0]*elements[1][1] - elements[1][0]*elements[0][1];
		x += t;
	}

	return x;
}

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

#ifndef RWFGRID_HPP
#define RWFGRID_HPP 1

#include "rwfgraph.hpp"

// Terrain Grid classes
class RWFgraph_GridElement {
	public:
	int class_value;	// Class value for this pixel
	float h;		// Height above sea level

	RWFgraph_GridElement(void)
		{class_value = 0; h = 0.0;};
};

class RWFfbm_ClassTableElement {
	public:

	int red, green, blue;	// Color of the class
	int color;						// Color index for this class
	float stdev;					// Initial standard deviation
	float level_scale;    // (0.5)^H

	// Initial constructor
	RWFfbm_ClassTableElement(void)
		{red = green = blue = 255; stdev = 1.0; color = 15;
		 level_scale = sqrt(0.5);};

	void setColor(int r, int g, int b)
		{red = r; green = g; blue = b;};
	void setColor(int c)
		{color = c;};

	void setStandardDeviation(float new_stdev)
		{stdev = new_stdev;};

	void setFractalDimension(float H)
		{level_scale = pow(0.5, H);};
};

typedef RWFlist_SimpleTable<RWFfbm_ClassTableElement> RWFlist_ClassTable;

class RWFgraph_Grid : public RWFgraph_Object {
	private:

	int numCols, numRows;
	RWFgraph_GridElement **data;
	float **zfbm;
	int   **class_fbm;
	float x_cell_size, y_cell_size;
	int max_recursion;
	int nlevels;

	RWFlist_ClassTable classes;

	protected:
	public:

	RWFgraph_Grid(void);
	RWFgraph_Grid(int nx, int ny);
	~RWFgraph_Grid(void);

	int ncols(void)	{return numCols;};
	int nrows(void)	{return numRows;};

	RWFgraph_GridElement * & operator [] (unsigned int i)
		{return data[i];};

	void setPixelSize(float xsizeIn, float ysizeIn)
		{x_cell_size = xsizeIn; y_cell_size = ysizeIn;};

	void findScanningOrder(RWFgraph_ViewingGeometry &, int &, int &, int &);
	void setMaximumRecursion(int max_levelIn)
		{max_recursion = max_levelIn;};
	void setRecursionLevel(RWFgraph_ViewingGeometry &);

	void setClassTable(RWFlist_ClassTable &ctable)
		{classes = ctable;};

	void draw(RWFgraph_ViewingGeometry &);
};

#endif

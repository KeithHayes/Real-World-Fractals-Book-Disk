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

#ifndef RWFIMAGE_HPP
#define RWFIMAGE_HPP 1

#include "rwfgraph.hpp"
#include "rwfmisc.hpp"

#include <stdio.h>
#include <alloc.h>

typedef unsigned char RWFpixel;

class RWFpcx_PaletteEntry {
	public:
	unsigned char red, green, blue;
	unsigned char intensity(void)
		{return (((int)red + (int)green + (int)blue)/3);};
};

class RWFpcx_File {
	private:

	RWFmisc_Boolean opened;
	FILE *file;
	long numCols, numRows;

	long curLine;
	RWFpixel *line_buffer;
	long *filepos;

	RWFpcx_PaletteEntry palette[256];

	protected:
	public:

	RWFpcx_File(void) {
		opened = RWFMISC_FALSE;
		file = NULL;
		numCols = numRows = 0;
		curLine = -1;
		line_buffer = NULL;
	};

	~RWFpcx_File(void);
	void close(void) {RWFpcx_File::~RWFpcx_File();};

	RWFmisc_Boolean isOpen(void)	{return opened;};
	void getSize(int &nx, int &ny) {nx = numCols; ny = numRows;};
	void readBlock(class RWFgraph_PixelBlock &pblock, int x, int y);
	int  readNextLine(RWFpixel *buffer, int start_x, int nread);
	int  readLine(RWFpixel *buffer, int iLine, int start_x, int nread);
	void reset(void);

	friend RWFpcx_File *rwf_pcxOpen(char *filename);

};

class RWFgraph_PixelBlock : public RWFgraph_Object {
	private:

	long numCols, numRows;
	long max_numCols, max_numRows;

	RWFpixel **lbuffer;

	int scan_order;
	int xoffset, yoffset, xmult, ymult;

	void setup(void);

	protected:
	public:

	RWFgraph_PixelBlock(void) {
		numCols = numRows = 0;
		max_numCols = max_numRows = 0;
		lbuffer = NULL;
		scan_order = 0;
	};

	RWFgraph_PixelBlock(long nx, long ny);
	~RWFgraph_PixelBlock(void);
	void setSize(long xsize, long ysize);
	void getSize(int &xsize, int &ysize) {
		xsize = numCols;
		ysize = numRows;
	};

	void setScanningOrder(int order) {
		scan_order = order;
		setup();
	}

	void setScanningOrder(int leftright, int bottomtop,
												int rowcolumn) {
		scan_order = 0;
		if(leftright)
			scan_order = 1;
		if(bottomtop)
			scan_order = scan_order | 2;
		if(rowcolumn)
			scan_order = scan_order | 4;
		setup();
	}

	int getScanningOrder(void)	{return scan_order;};
	void getScanningOrder(int &leftright, int &bottomtop,
												int &rowcolumn) {
		leftright = scan_order & 1;
		bottomtop = (scan_order & 2) > 1;
		rowcolumn = (scan_order & 4) > 1;
	}

	void setPixel(int x, int y, RWFpixel p) {
		if(lbuffer) {
			if(scan_order < 4)
				lbuffer[yoffset + y*ymult][xoffset + x*xmult] = p;
			else
				lbuffer[xoffset + x*xmult][yoffset + y*ymult] = p;
		}
	};

	RWFpixel getPixel(int x, int y) {
		if(lbuffer) {
			if(scan_order < 4)
				return lbuffer[yoffset + y*ymult][xoffset + x*xmult];
			else
				return lbuffer[xoffset + x*xmult][yoffset + y*ymult];
		}
		else
			return 0;
	};

	void getFromScreen(int x, int y);
	void getFromFile(RWFpcx_File *pfile, int x, int y);
	void getFromBlock(RWFgraph_PixelBlock &inblock, int x, int y);
	void copyToBlock(RWFgraph_PixelBlock &outblock, int x, int y);

	void draw(int x, int y);
	void draw(void)	{draw(0, 0);};
	void draw(RWFgraph_ViewingGeometry &)	{};

	void remapIntensity(float contrast, float offset);
	float compare(RWFgraph_PixelBlock &pblock, float &c, float &b,
								int &scan);

	friend class RWFpcx_File;
};

class RWFimage_RegionMap {
	public:
	// Define the input region
	int x_region, y_region;
	int x_size, y_size;

	// Maps to a larger region
	int x_output, y_output;
	int out_factor;					// Output region is always an integer
													// multiple of input
	int scan_order; // Scanning order for the best match
	int b;
	float c;
	float error;	// Error term between the regions
};

typedef RWFlist_SimpleTable<RWFimage_RegionMap> RWFlist_RegionMapTable;

void rwf_imageEncode8x8(RWFgraph_PixelBlock &image,
												RWFlist_RegionMapTable &rtable);
void rwf_imageEncodeQuadTree(RWFgraph_PixelBlock &image,
														 RWFlist_RegionMapTable &rtable,
														 float max_error, int min_size);

void rwf_imageFindBestMatch(RWFgraph_PixelBlock &image,
														RWFimage_RegionMap &rmap);
void rwf_PixelBlockResample(RWFgraph_PixelBlock &inp, RWFgraph_PixelBlock &outp);

void rwf_imageDecode(RWFlist_RegionMapTable &rtable,
										 RWFgraph_PixelBlock &screen_image, int npasses);

void rwf_imageDecodeRegionTable(RWFlist_RegionMapTable &rtable,
																RWFgraph_PixelBlock &screen_image);

#endif

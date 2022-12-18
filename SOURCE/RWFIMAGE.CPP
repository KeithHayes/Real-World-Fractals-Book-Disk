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

/*
** All routines for PCX image I/O and manipulating image blocks
*/

#include "rwfimage.hpp"
#include "rwfrand.hpp"

#include <dos.h>

// Definition of PCX header

class RWFpcx_Header {
	public:
	unsigned char header;
	unsigned char version;
	unsigned char encode;
	unsigned char bitsPerPixel;
	unsigned short xMinBound;
	unsigned short yMinBound;
	unsigned short xMaxBound;
	unsigned short yMaxBound;
	unsigned short xRes;
	unsigned short yRes;
	RWFpcx_PaletteEntry palette[16];
	unsigned char vmode;
	unsigned char numOfPlanes;
	unsigned short bytesPerLine;
	unsigned short numImages;
	RWFpixel *paletteOffset;
	unsigned char unused[60 - sizeof(RWFpixel *)-sizeof(unsigned short)];
};

// Open a PCX file

static RWFlist_RegionMapTable dummy_regions;

#define RWFPCX_START_OF_DATA	128

RWFpcx_File *
rwf_pcxOpen(char *filename)
{
	RWFpcx_File *pcx;
	RWFpcx_Header pcxhead;

	if(!filename)
		return NULL;

	pcx = new RWFpcx_File;

	pcx->file = fopen(filename, "r+b");

	if(!pcx->file) {
		printf("Unable to open file %s\n", filename);
		delete pcx;
		return NULL;
	}

	// Read in the PCX header
	fseek(pcx->file, 0, SEEK_SET);
	fread(&pcxhead, sizeof(RWFpcx_Header), 1, pcx->file);
	pcx->numCols = pcxhead.xMaxBound - pcxhead.xMinBound + 1;
	pcx->numRows = pcxhead.yMaxBound - pcxhead.yMinBound + 1;

	pcx->line_buffer = new RWFpixel [pcx->numCols];

	pcx->opened = RWFMISC_TRUE;
	pcx->filepos = new long [pcx->numRows];

	for(int i=0; i < pcx->numRows; i++)
		pcx->filepos[i] = 0;

	pcx->reset();
	pcx->readLine(NULL, pcx->numRows-1, 0, 1);

	// Now read the color palette
	RWFpcx_PaletteEntry rgb;
	int status = 1, numPalette=0;
	unsigned short numEntries;
	status = fread(&numEntries, sizeof(unsigned char), 1, pcx->file);
	int intensity, maxintensity = 0;
	while(status && numPalette < 256) {
		status = fread(&rgb, sizeof(RWFpcx_PaletteEntry), 1, pcx->file);
		pcx->palette[numPalette++] = rgb;
		intensity = rgb.red + rgb.green + rgb.blue;
		maxintensity = RWFMISC_MAX(intensity, maxintensity);
	}

	if(numPalette < 256) {
		rgb.red = rgb.green = rgb.blue = 0;
		for(int i=numPalette; i < 256; i++)
			pcx->palette[i] = rgb;
	}

	maxintensity = maxintensity / 3;
	if(maxintensity < 255) {
		float scale = 255.0 / (float)maxintensity;
		for(int i=0; i < 256; i++) {
			rgb = pcx->palette[i];
			rgb.red   = RWFMISC_MIN(rgb.red*scale, 255);
			rgb.green = RWFMISC_MIN(rgb.green*scale, 255);
			rgb.blue  = RWFMISC_MIN(rgb.blue*scale, 255);
			pcx->palette[i] = rgb;
		}
	}

	pcx->reset();

	return pcx;
}

void RWFpcx_File::reset(void)
{
	if(isOpen())
		fseek(file, RWFPCX_START_OF_DATA, SEEK_SET);

	curLine = -1;
}

RWFpcx_File::~RWFpcx_File(void)
{
	if(file)
		fclose(file);

	if(line_buffer)
		delete line_buffer;

	opened = RWFMISC_FALSE;
	file = NULL;
	numCols = numRows = 0;
	curLine = -1;
	line_buffer = NULL;
}

int
RWFpcx_File::readNextLine(RWFpixel *buffer, int start_x, int nread)
{
	int i;

	// Set the buffer to 0
	if(buffer) {
		for(i=0; i < nread; i++)
			buffer[i] = 0;
	}

	curLine++;
	if(curLine > numRows-1) {
		curLine = numRows - 1;
		return 1;
	}


	filepos[curLine] = ftell(file);

	// Fill the line buffer
	int numpixels = numCols, count;
	RWFpixel pixel;
	RWFpixel *pbuf = line_buffer;

	while(numpixels > 0) {
		fread(&pixel, 1, 1, file);
		if( (pixel & 0xc0) == 0xc0) {
			count = pixel & (~0xc0);
			fread(&pixel, 1, 1, file);
		} else if (pixel == 0xc0) {
			count = 1;
			printf("Bad pixel = 0xc0\n");
		} else {
			count = 1;
		}
		count = RWFMISC_MIN(count, numpixels);
		for(i=0; i < count; i++)
			*pbuf++ = pixel;

		numpixels -= count;
	}

	if(buffer) {
		int first = RWFMISC_MAX(0, -start_x);
		int last  = nread - RWFMISC_MAX(0, start_x + nread - numCols);
		for(i=first; i < last; i++)
			buffer[i] = line_buffer[i + start_x];
	}

	if(numpixels < 0) {
		printf("Bad line count\n");
		return 1;
	}

	return 0;
}

int
RWFpcx_File::readLine(RWFpixel *buffer, int iLine, int start_x, int nread)
{
	int i, status;

	if(iLine < 0 || iLine > numRows-1) {
		if(buffer) {
			for(i=0; i < nread; i++)
				buffer[i] = 0;
		}
		return 0;
	}

	if(filepos[iLine] != 0) {
		curLine = iLine-1;
		fseek(file, filepos[iLine], SEEK_SET);
		status = readNextLine(buffer, start_x, nread);
	} else if(iLine < curLine) {
		reset();
		for(i=0; i <= iLine; i++)
			status = readNextLine(buffer, start_x, nread);
	} else if (iLine > curLine) {
		for(i=curLine; i < iLine; i++)
			status = readNextLine(buffer, start_x, nread);
	} else {
		if(buffer) {
			for(i = 0; i < nread; i++)
				buffer[i] = 0;
			int first = RWFMISC_MAX(0, -start_x);
			int last  = nread - RWFMISC_MAX(0, start_x + nread - numCols);
			for(i=first; i < last; i++)
				buffer[i] = line_buffer[i + start_x];
			status = 0;
		}
	}

	return status;
}

void
RWFpcx_File::readBlock(RWFgraph_PixelBlock &pblock, int x, int y)
{
	int ix, iy, ncols, nlines;
	RWFpixel *pbuffer;
	RWFpixel *lbuffer;

	pblock.getSize(ncols, nlines);

	readLine(NULL, y, x, 0);
	for(iy = 0; iy < nlines; iy++) {
		pbuffer = pblock.lbuffer[iy];
		lbuffer = &(line_buffer[x]);
		if(iy + y < numRows) {
			for(ix = 0; ix < ncols; ix++) {
				if((x + ix) >= numCols)
					*pbuffer++ = 0;
				else {
					RWFpcx_PaletteEntry *rgb;
					rgb = &(palette[*lbuffer++]);
					*pbuffer++ = rgb->intensity();
				}
			}
			readNextLine(NULL, x, 0);
		} else {
			for(ix = 0; ix < ncols; ix++)
				*pbuffer++ = 0;
		}
	}

	return;
}

// Pixel Block functions

RWFgraph_PixelBlock::RWFgraph_PixelBlock(long nx, long ny)
{
	numCols = nx;
	numRows = ny;
	lbuffer = new far RWFpixel * [ny];
	for(int i=0; i < numRows; i++)
		lbuffer[i] = new far RWFpixel [nx];
	scan_order = 0;
	max_numCols = nx;
	max_numRows = ny;
	setup();
};

RWFgraph_PixelBlock::~RWFgraph_PixelBlock(void)
{
	if(lbuffer) {
		for(int i=0; i < max_numRows; i++)
			delete lbuffer[i];
		delete lbuffer;
	}
	numCols = 0;
	numRows = 0;
	max_numCols = 0;
	max_numRows = 0;
}

void RWFgraph_PixelBlock::setSize(long xsize, long ysize)
{
	if(xsize > max_numCols || ysize > max_numRows) {
		RWFgraph_PixelBlock::~RWFgraph_PixelBlock();
		numCols = xsize;
		numRows = ysize;
		lbuffer = new far RWFpixel * [ysize];
		for(int i=0; i < numRows; i++)
			lbuffer[i] = new far RWFpixel [xsize];
	} else {
		numCols = xsize;
		numRows = ysize;
	}

	scan_order = 0;
	setup();
	max_numCols = RWFMISC_MAX(numCols, max_numCols);
	max_numRows = RWFMISC_MAX(numRows, max_numRows);

};


void
RWFgraph_PixelBlock::draw(int x, int y)
{
	for(int j=0; j < numRows; j++) {
		for(int i=0; i < numCols; i++)
			rwf_graphPlot(i + x, j + y, getPixel(i, j));
	}
}

void
RWFgraph_PixelBlock::getFromScreen(int x, int y)
{
	for(int j=0; j < numRows; j++) {
		for(int i=0; i < numCols; i++) {
			setPixel(i, j, rwf_graphGetPixel(i+x, j+y));
		}
	}

	return;
}

void
RWFgraph_PixelBlock::getFromFile(RWFpcx_File *pfile, int x, int y)
{
	if(pfile)
		pfile->readBlock(*this, x, y);

	return;
}

void
RWFgraph_PixelBlock::setup(void)
{
	switch(scan_order) {
		case 0: // +x, +y, top
		xoffset = 0;
		yoffset = 0;
		xmult  = 1;
		ymult  = 1;
		break;
		case 1: // -x, +y, top
		xoffset = numCols-1;
		yoffset = 0;
		xmult  = -1;
		ymult  = 1;
		break;
		case 2: // +x, +y, bottom
		xoffset = 0;
		yoffset = numRows-1;
		xmult  = 1;
		ymult  = -1;
		break;
		case 3:	// -x, +y, bottom
		xoffset = numCols-1;
		yoffset = numRows-1;
		xmult  = -1;
		ymult  = -1;
		break;
		case 4: // +x, +y, top
		xoffset = 0;
		yoffset = 0;
		xmult  = 1;
		ymult  = 1;
		break;
		case 5: // -x, +y, top
		xoffset = numRows-1;
		yoffset = 0;
		xmult  = -1;
		ymult  = 1;
		break;
		case 6: // +x, +y, bottom
		xoffset = 0;
		yoffset = numCols-1;
		xmult  = 1;
		ymult  = -1;
		break;
		case 7:	// -x, +y, bottom
		xoffset = numRows-1;
		yoffset = numCols-1;
		xmult  = -1;
		ymult  = -1;
		break;
		default:
		xoffset = 0;
		yoffset = 0;
		xmult  = 1;
		ymult  = 1;
		break;
	}

	return;
}

void
RWFgraph_PixelBlock::getFromBlock(RWFgraph_PixelBlock &inblock, int x, int y)
{
	RWFpixel pixel;

	for(int j=0; j < numRows; j++) {
		if((j + y) < inblock.numRows) {
			for(int i=0; i < numCols; i++) {
				if((i + x) < inblock.numCols)
					pixel = inblock.getPixel(i + x, j + y);
				else
					pixel = 0;
				setPixel(i, j, pixel);
			}
		} else {
			for(int i=0; i < numCols; i++)
				setPixel(i, j, 0);
		}
	}
}

void
RWFgraph_PixelBlock::copyToBlock(RWFgraph_PixelBlock &outblock, int x, int y)
{
	RWFpixel pixel;

	for(int j=0; j < numRows; j++) {
		if((j + y) < outblock.numRows) {
			for(int i=0; i < numCols; i++) {
				pixel = getPixel(i, j);
				if((i + x) < outblock.numCols) {
					outblock.setPixel(i + x, j + y, pixel);
				} else {
					break;
				}
			}
		} else {
			break;
		}
	}
}

void
RWFgraph_PixelBlock::remapIntensity(float contrast, float offset)
{
	if(numRows > 0) {
		for(int j=0; j < numRows; j++) {
			for(int i=0; i < numCols; i++) {
				float newval = (getPixel(i,j)*contrast + offset);
				int pixel = RWFMISC_MIN(RWFMISC_MAX(newval, 0), 255);
				setPixel(i, j, pixel);
			}
		}
	}

	return;
}

static float error_threshold = 150.0;

void rwf_ImageSetErrorThreshold(float e)
{
	error_threshold = e*e;
}

float getErrorThreshold(void)
{
	return error_threshold;
}

static RWFgraph_PixelBlock temp_block(128, 128);

float
RWFgraph_PixelBlock::compare(RWFgraph_PixelBlock &pblock,
														 float &best_c, float &best_b, int &best_scan)
{
	float min_error, error;
	float c;
	long b;
	long sum1, sum2, sum12, square_sum;

	temp_block.setSize(numCols, numRows);
	rwf_PixelBlockResample(pblock, temp_block);

	min_error = 100000;
	best_scan = 0;
	best_c = 1.0;
	best_b = 0;

	for(int scan = 0; scan < 8; scan++) {

		temp_block.setScanningOrder(scan);

		if(scan == 0)
			sum1 = sum2 = square_sum = 0;

		sum12 = 0;

		int pixel1, pixel2;

		for(int j=0; j < numRows; j++) {
			for(int i=0; i < numCols; i++) {
				pixel1 = getPixel(i, j);
				pixel2 = temp_block.getPixel(i, j);
				if(scan == 0) {
					sum1 += pixel1;
					square_sum += pixel2*pixel2;
					sum2 += pixel2;
				}
				sum12 += pixel1*pixel2;
			}
		}

		long total_pixels = numRows * numCols;
		double one_over_total = 1.0 / (double)total_pixels;
		double avg1, avg2, avg12, avgsq, dnum, denom;

		avg1 = (double)sum1*one_over_total;
		avg2 = (double)sum2*one_over_total;
		avg12 = (double)sum12 * one_over_total;
		avgsq = (double)square_sum * one_over_total;

		dnum = avg12 - avg1*avg2;
		denom = avgsq - (avg2*avg2);
		if(ABS(denom) < 0.001)
			c = 255.0;
		else
			c = dnum / denom;

		if(ABS(c) > 1.0)
			continue;

		b = ((float)avg1 - c*(float)avg2);

		error = 0.0;
		long err;
		for(j=0; j < numRows; j++) {
			for(int i=0; i < numCols; i++) {
				pixel1 = getPixel(i, j);
				pixel2 = temp_block.getPixel(i, j);
				err    = RWFMISC_NINT(c*pixel2) + b - pixel1;
				error += err*err;
			}
			if(error*one_over_total > min_error)
				break;
		}

		error *= one_over_total;
		if(error < min_error) {
			min_error = error;
			best_scan = scan;
			best_c = c;
			best_b = b;
			if(min_error < getErrorThreshold())
				return min_error;
		}

	}

	return min_error;
}



void
rwf_PixelBlockResample(RWFgraph_PixelBlock &inp, RWFgraph_PixelBlock &outp)
{
	int xin, yin;
	int xout, yout;
	float x, y, xfactor, yfactor;
	int ix, iy;
	RWFpixel pixel;

	// Get the input and output sizes
	// Make sure the output scans in normal order

	inp.getSize(xin, yin);
	outp.getSize(xout, yout);
	outp.setScanningOrder(0);

	// These represent the increment in input coordinates
	xfactor = (float)xin / (float)xout;
	yfactor = (float)yin / (float)yout;

	// x and y are the input coordinates to get the pixel from
	// RWFMISC_NINT rounds a float to an int
	y = 0.0;
	for(int j=0; j < yout; j++) {
		x = 0.0;
		iy = RWFMISC_NINT(y);
		iy = RWFMISC_MIN(RWFMISC_MAX(iy, 0), yin-1);
		for(int i=0; i < xout; i++) {
			ix = RWFMISC_NINT(x);
			ix = RWFMISC_MIN(RWFMISC_MAX(ix, 0), xin-1);
			pixel = inp.getPixel(ix, iy);
			outp.setPixel(i, j, pixel);
			x += xfactor;
		} // End x loop
		y += yfactor;
	} // End y loop
}

void
rwf_imageEncode8x8(RWFgraph_PixelBlock &image,
									 RWFlist_RegionMapTable &rtable)
{
	int xsize, ysize;
	int ntable;
	RWFimage_RegionMap rmap;

	image.getSize(xsize, ysize);
	rmap.out_factor = 2;
	rtable.reset(); // Reset the table to 0 size
	ntable = 0;

	for(int in_y=0; in_y < ysize-1; in_y += 8) {
		for(int in_x=0; in_x < xsize-1; in_x += 8) {
			rmap.x_region = in_x;
			rmap.y_region = in_y;
			rmap.x_size = RWFMISC_MIN(8, xsize - in_x);
			rmap.y_size = RWFMISC_MIN(8, ysize - in_y);
			rwf_imageFindBestMatch(image, rmap);
			// Now save the settings in the dictionary for this region
			rtable[ntable++] = rmap;
		} // End of x region loop
	} // End of y region loop

	return;
}

// Temporaries for use in the comparison tests

static RWFgraph_PixelBlock in_region(32, 32), out_region(64, 64);

void
rwf_imageFindBestMatch(RWFgraph_PixelBlock &image,
											 RWFimage_RegionMap &rmap)
{
	int xsize, ysize;
	int in_xsize, in_ysize;

	float min_e;
	float b, c, e;
	int xstep, ystep, scan_order, xadd, yadd;

	image.getSize(xsize, ysize);
	image.setScanningOrder(0);
	in_region.setSize(rmap.x_size, rmap.y_size);
	in_region.getFromBlock(image, rmap.x_region, rmap.y_region);

	// Mark the area in white
	setfillstyle(SOLID_FILL, 255);
	bar(rmap.x_region, rmap.y_region,
			rmap.x_region + rmap.x_size - 1, rmap.y_region + rmap.y_size - 1);

	xstep = rmap.out_factor * rmap.x_size;
	ystep = rmap.out_factor * rmap.y_size;
	xadd = RWFMISC_MIN(xstep/2 + 1, 10);
	yadd = RWFMISC_MIN(ystep/2 + 1, 10);
	out_region.setSize(xstep, ystep);
	out_region.setScanningOrder(0);
	min_e = 1000000;

	rmap.error = min_e;
	rmap.x_output = 0;
	rmap.y_output = 0;
	rmap.b = 0;
	rmap.c = 1.0;
	rmap.scan_order = 0;

	// Now scan every region in the image
	for(int y=0; y <= ysize - ystep; y += yadd) {
		int xstart = RWFMISC_MIN(y % xadd, xsize-xstep);
		for(int x=xstart; x <= xsize - xstep; x += xadd) {
			out_region.getFromBlock(image, x, y);
			e = in_region.compare(out_region, c, b, scan_order);
			out_region.draw(rmap.x_region*2 + xsize + 8,
											rmap.y_region*2);
			if(e < min_e) {
				rmap.error = e;
				rmap.b = RWFMISC_NINT(b);
				rmap.c = c;
				rmap.scan_order = scan_order;
				rmap.x_output = x;
				rmap.y_output = y;
				min_e = e;
				if(min_e < getErrorThreshold()) {
					out_region.setScanningOrder(scan_order);
					out_region.remapIntensity(rmap.c, rmap.b);
					out_region.draw(rmap.x_region*2 + xsize + 8,
													rmap.y_region*2);
					in_region.draw(rmap.x_region, rmap.y_region);
					return;
				}
			}
			rwf_miscCheckEscape();
		} // End of scan in x
	} // End of scan in y

	rwf_miscCheckEscape();
	out_region.getFromBlock(image, rmap.x_output, rmap.y_output);
	out_region.setScanningOrder(rmap.scan_order);
	out_region.remapIntensity(rmap.c, rmap.b);
	out_region.draw(rmap.x_region*2 + xsize + 8,
									rmap.y_region*2);
	in_region.draw(rmap.x_region, rmap.y_region);

	return;
} // Finished, best match is in rmap

void
rwf_imageEncodeQuadSub(RWFgraph_PixelBlock &out_image,
											 RWFlist_RegionMapTable &rtable,
											 RWFimage_RegionMap &rmap, int min_size);
void
rwf_imageEncodeQuadTree(RWFgraph_PixelBlock &out_image,
												RWFlist_RegionMapTable &rtable,
												float max_error, int min_size)
{
	int xsize, ysize;
	RWFimage_RegionMap rmap;

	rwf_ImageSetErrorThreshold(max_error);

	out_image.getSize(xsize, ysize);
	rmap.out_factor = 2;
	rtable.reset(); // Reset the table to 0 size

	for(int in_y=0; in_y < ysize-1; in_y += 32) {
		for(int in_x=0; in_x < xsize-1; in_x += 32) {
			rmap.x_region = in_x;
			rmap.y_region = in_y;
			rmap.x_size = RWFMISC_MIN(32, xsize - in_x);
			rmap.y_size = RWFMISC_MIN(32, ysize - in_y);
			rwf_imageEncodeQuadSub(out_image, rtable, rmap, min_size);
		} // End of x region loop
	} // End of y region loop

	return;
}

// Main working routine for quad encoding

void
rwf_imageEncodeQuadSub(RWFgraph_PixelBlock &out_image,
											 RWFlist_RegionMapTable &rtable,
											 RWFimage_RegionMap &rmap, int min_size)
{
	rwf_imageFindBestMatch(out_image, rmap);

	// Ok, are we as small as we can get or did we get a good match
	if(rmap.error < getErrorThreshold() ||
		 rmap.x_size <= min_size) {
		 int ntable = rtable.size();
		 rtable[ntable] = rmap;
		 return;
	}

	// No, divide the region into 4 smaller regions
	RWFimage_RegionMap new_rmap;

	new_rmap.out_factor = 2;
	new_rmap.x_region = rmap.x_region;
	new_rmap.y_region = rmap.y_region;
	new_rmap.x_size = rmap.x_size/2;
	new_rmap.y_size = rmap.y_size/2;
	rwf_imageEncodeQuadSub(out_image, rtable, new_rmap, min_size);

	new_rmap.x_region += new_rmap.x_size;
	rwf_imageEncodeQuadSub(out_image, rtable, new_rmap, min_size);

	new_rmap.x_region -= new_rmap.x_size;
	new_rmap.y_region += new_rmap.y_size;
	rwf_imageEncodeQuadSub(out_image, rtable, new_rmap, min_size);

	new_rmap.x_region += new_rmap.x_size;
	rwf_imageEncodeQuadSub(out_image, rtable, new_rmap, min_size);

	return;
}


void
rwf_imageDecode(RWFlist_RegionMapTable &rtable,
								RWFgraph_PixelBlock &out_image, int npasses)
{
	int xsize, ysize;
	int i,j;

	out_image.getSize(xsize, ysize);
  out_image.setScanningOrder(0);

	// Initialize out_image to a checker board
	for(j=0; j < ysize; j++) {
		if( (j >> 1) % 2 == 0) {
			for(i=0; i < xsize; i++)
				out_image.setPixel(i, j, 192);
		} else {
			for(i=0; i < xsize; i++)
				out_image.setPixel(i, j, ((i >> 1)%2)*128 + 64);
		}
	}

	// Okay, decode the image
	for(int pass=0; pass < npasses; pass++) {
		rwf_miscCheckEscape();
		rwf_imageDecodeRegionTable(rtable, out_image);
	}
}

void
rwf_imageDecodeRegionTable(RWFlist_RegionMapTable &rtable,
													 RWFgraph_PixelBlock &out_image)
{
	RWFimage_RegionMap rmap;

	int out_xsize, out_ysize;
	int x_screensize, y_screensize;
	int xoffset, yoffset;

	out_image.getSize(x_screensize, y_screensize);
	temp_block.setSize(x_screensize, y_screensize);

	// Randomize the resampling to get slightly better results
	xoffset = ((int)(rwf_randRand()*10)) & 1;
	yoffset = ((int)(rwf_randRand()*10)) & 1;
	for(int i=0; i < rtable.size(); i++) {
		rmap = rtable[i];
		in_region.setSize(rmap.x_size, rmap.y_size);
		in_region.setScanningOrder(0);
		// Now get the region from the screen
		out_xsize = rmap.x_size * rmap.out_factor;
		out_ysize = rmap.y_size * rmap.out_factor;
		out_region.setSize(out_xsize, out_ysize);
		out_region.getFromBlock(out_image, rmap.x_output + xoffset, rmap.y_output + yoffset);
		rwf_PixelBlockResample(out_region, in_region);
		// Rescale the intensities
		in_region.remapIntensity(rmap.c, rmap.b);
		// Ok, now copy the remapped region into the temporary
		in_region.setScanningOrder(rmap.scan_order);
		in_region.copyToBlock(temp_block, rmap.x_region, rmap.y_region);
	} // End of loop on region map table;

	out_image.getFromBlock(temp_block, 0, 0);

	return;
}


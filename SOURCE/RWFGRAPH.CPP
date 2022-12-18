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
#include "rwfrand.hpp"
#include "rwfmisc.hpp"
#include "svga256.h"

#include <dos.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>

/*
**	Functions to support class RWFgraph_GraphicsState
*/

class RWFgraph_GraphicsState{
private:
	int minx;
	int maxx;
	int npixx;
	int miny;
	int maxy;
	int npixy;
	float aspectratio;
	float xmin;
	float xmax;
	float ymin;
	float ymax;
	int force_wireframe;

protected:

public:
	RWFgraph_GraphicsState(void)	{is_Initialized = RWFMISC_FALSE;};
	RWFmisc_Boolean is_Initialized;
	float xscale;
	float yscale;
	float ave_scale;
	float xoffset;
	float yoffset;
	void initialize(int mode);
	void setScreenWindow(int iMinIn,int jMinIn,int iMaxIn,
		int jMaxIn) {
		minx = iMinIn;
		miny = jMinIn;
		maxx = iMaxIn;
		maxy = jMaxIn;
		npixx = maxx - minx + 1;
		npixy = maxy - miny + 1;
		xmin = -1.0 * aspectratio * RWFGRAPH_GLOBAL_SCALE *
			(((float) npixx) / npixy);
		xmax =  1.0 * aspectratio * RWFGRAPH_GLOBAL_SCALE *
			(((float) npixx) / npixy);
		ymin = -1.0 * RWFGRAPH_GLOBAL_SCALE;
		ymax =  1.0 * RWFGRAPH_GLOBAL_SCALE;
		setWindow(xmin,ymin,xmax,ymax);
	}
	void setWindow(float xMinIn,float yMinIn,float xMaxIn,
		float yMaxIn) {
		xmin = xMinIn;
		xmax = xMaxIn;
		ymin = yMinIn;
		ymax = yMaxIn;
		xscale = (maxx - minx) / (xmax - xmin);
		yscale = -(maxy - miny) / (ymax - ymin);
		ave_scale = sqrt((xscale*xscale + yscale*yscale)/2.0);
		xoffset = ((minx + maxx) / 2.0) - (xscale * (xmin + xmax)/2.0);
		yoffset = ((miny + maxy) / 2.0) - (yscale * (ymin + ymax)/2.0);
	};
	int getIMin(void) {return minx;};
	int getIMax(void) {return maxx;};
	int getJMin(void) {return miny;};
	int getJMax(void) {return maxy;};
	float getXMin(void) {return xmin;};
	float getXMax(void) {return xmax;};
	float getYMin(void) {return ymin;};
	float getYMax(void) {return ymax;};
	int getWireframeMode(void)	{return force_wireframe;};
	void setWireframeMode(int flag) {force_wireframe = flag;};
};

static RWFgraph_GraphicsState rwfgraph_graphicsstate;

void
RWFgraph_GraphicsState::initialize(int mode)
	{
		int iint,jint;
		int gdriver, gmode = SVGA800x600x256, errorcode;
		char *overmode = getenv("RWF_DISPLAY");


		if(mode)
			gmode = mode;
		else if (overmode)
			sscanf(overmode,"%d",&gmode);

		gdriver = installuserdriver("Svga256", NULL);

		// Find best graphics mode
		do {
			initgraph(&gdriver,&gmode,"C:\\TC\\BGI");
			errorcode = graphresult();
			gmode--;
		} while (errorcode != grOk && gmode >= 0);

		if (errorcode != grOk) {
			printf("***Error reported by function initgraph***\n***%s***\n",
				grapherrormsg(errorcode));
			printf(">>>Press any key to abort<<<\n");
			getch();
			exit(EXIT_FAILURE);
		}

		setcolor(RWFGRAPH_COLOR_WHITE);
		if(!is_Initialized) {
			minx = 0;
			miny = 0;
			maxx = getmaxx();
			maxy = getmaxy();
			npixx = maxx - minx + 1;
			npixy = maxy - miny + 1;
			getaspectratio((int far *) &iint,(int far *) &jint);
			aspectratio = 10000.0 / iint;
			xmin = -1.0 * aspectratio * RWFGRAPH_GLOBAL_SCALE *
				(((float) npixx) / npixy);
			xmax =  1.0 * aspectratio * RWFGRAPH_GLOBAL_SCALE *
				(((float) npixx) / npixy);
			ymin = -1.0 * RWFGRAPH_GLOBAL_SCALE;
			ymax =  1.0 * RWFGRAPH_GLOBAL_SCALE;
			setWindow(xmin,ymin,xmax,ymax);

			// Set default palette
			rwf_graphSetPaletteIndex(0 ,   0,   0,   0);
			rwf_graphSetPaletteIndex(1 ,   0,   0, 168);
			rwf_graphSetPaletteIndex(2 ,   0, 168,   0);
			rwf_graphSetPaletteIndex(3 ,   0, 168, 168);
			rwf_graphSetPaletteIndex(4 , 168,   0,   0);
			rwf_graphSetPaletteIndex(5 , 168,   0, 168);
			rwf_graphSetPaletteIndex(6 , 168,  84,   0);
			rwf_graphSetPaletteIndex(7 , 168, 168, 168);
			rwf_graphSetPaletteIndex(8 ,  84,  84,  84);
			rwf_graphSetPaletteIndex(9 ,  84,  84, 255);
			rwf_graphSetPaletteIndex(10,  84, 255,  84);
			rwf_graphSetPaletteIndex(11,  84, 255, 255);
			rwf_graphSetPaletteIndex(12, 255,  84,  84);
			rwf_graphSetPaletteIndex(13, 255,  84, 255);
			rwf_graphSetPaletteIndex(14, 255, 255,  84);
			rwf_graphSetPaletteIndex(15, 255, 255, 255);
			rwf_graphSetColorScale();
		}
		force_wireframe = 0;
		is_Initialized = RWFMISC_TRUE;
		rwf_graphSetPaletteIndex(255, 255, 255, 255);
	};

int rwf_graphGetMaxX(void) {return getmaxx();}
int rwf_graphGetMaxY(void) {return getmaxy();}
int rwf_graphIMin(void) {return rwfgraph_graphicsstate.getIMin();}
int rwf_graphIMax(void) {return rwfgraph_graphicsstate.getIMax();}
int rwf_graphJMin(void) {return rwfgraph_graphicsstate.getJMin();}
int rwf_graphJMax(void) {return rwfgraph_graphicsstate.getJMax();}
float rwf_graphXMin(void) {return rwfgraph_graphicsstate.getXMin();}
float rwf_graphXMax(void) {return rwfgraph_graphicsstate.getXMax();}
float rwf_graphYMin(void) {return rwfgraph_graphicsstate.getYMin();}
float rwf_graphYMax(void) {return rwfgraph_graphicsstate.getYMax();}
float rwf_graphXScale(void) {return rwfgraph_graphicsstate.xscale;}
float rwf_graphYScale(void) {return rwfgraph_graphicsstate.yscale;}
float rwf_graphAveScale(void) {return rwfgraph_graphicsstate.ave_scale;}
float rwf_graphXOffset(void) {return rwfgraph_graphicsstate.xoffset;}
float rwf_graphYOffset(void) {return rwfgraph_graphicsstate.yoffset;}
int rwf_graphForceWireframe(void) {return rwfgraph_graphicsstate.getWireframeMode();}
void rwf_graphSetWireframeMode(int flag) {rwfgraph_graphicsstate.setWireframeMode(flag);}

void rwf_graphSetWindow(float xMinIn,float yMinIn,float xMaxIn,
	float yMaxIn) {
	rwfgraph_graphicsstate.setWindow(xMinIn,yMinIn,xMaxIn,yMaxIn);
	return;
}

void rwf_graphSetScreenWindow(int xMinIn,int yMinIn,int xMaxIn,
	int yMaxIn) {
	rwfgraph_graphicsstate.setScreenWindow(xMinIn,yMinIn,xMaxIn,yMaxIn);
	return;
}

void rwf_graphInitialize(int mode) {
	rwfgraph_graphicsstate.initialize(mode);
	return;
}

RWFmisc_Boolean rwf_graphIsInitialized(void) {
	return rwfgraph_graphicsstate.is_Initialized;
}

/*
**	Functions to support class RWFgraph_Object
*/

/*
** Functions to support class RWFgraph_Line
*/

void RWFgraph_Line::draw(void) {
	int i0,j0,i1,j1;
	int oldcolor;
	float newx, newy;
	float x, y;
	static RWFvec_Vector tvec(2);

	first.getCoord(tvec);
	tvec = getMatrix() * tvec;
	i0 = RWFGRAPH_XSCREENCOORD(tvec[0]);
	j0 = RWFGRAPH_YSCREENCOORD(tvec[1]);

	end.getCoord(tvec);
	tvec = getMatrix() * tvec;
	i1 = RWFGRAPH_XSCREENCOORD(tvec[0]);
	j1 = RWFGRAPH_YSCREENCOORD(tvec[1]);

	oldcolor = getcolor();
	setcolor(getColor());
	line(i0,j0,i1,j1);
	setcolor(oldcolor);

	return;
}


RWFgraph_Line
operator * (RWFmat_Matrix &mat, RWFgraph_Line &lIn)
{
	float x, y;
	RWFgraph_Line ltemp;
	static RWFvec_Vector tvec(2);

	lIn.getFirst(x, y);
	tvec[0] = x;
	tvec[1] = y;
	tvec = mat*tvec;
	ltemp.setFirst(tvec[0], tvec[1]);
	lIn.getEnd(x, y);
	tvec[0] = x;
	tvec[1] = y;
	tvec = mat*tvec;
	ltemp.setEnd(tvec[0], tvec[1]);
	ltemp.setColor(lIn.getColor());

	return ltemp;
}


/*
**	Functions to support class RWFgraph_Circle
*/
void RWFgraph_Circle::draw(void) {
	int savecolor;
	float x0,y0,r;
	static RWFvec_Vector center(2);

	center[0] = xCenter;
	center[1] = yCenter;

	center = getMatrix() * center;

	x0 = center[0];
	y0 = center[1];
	r = radius * rwf_matGetGlobalScalingFactor(getMatrix());

	savecolor = getcolor();
	setcolor(getColor());
	rwf_graphCircleDraw(x0,y0,r);

	setcolor(savecolor);
	return;
}


void RWFgraph_Circle::fill(void) {
	int savecolor;
	static fillsettingstype oldfillinfo;
	float x0,y0,r;

	static RWFvec_Vector center(2);

	center[0] = xCenter;
	center[1] = yCenter;

	center = getMatrix() * center;

	x0 = center[0];
	y0 = center[1];
	r = radius * rwf_matGetGlobalScalingFactor(getMatrix());

	getfillsettings(&oldfillinfo);
	setfillstyle(oldfillinfo.pattern,getColor());
	savecolor = getcolor();
	setcolor(getColor());
	rwf_graphCircleFill(x0,y0,r);
	setfillstyle(oldfillinfo.pattern,oldfillinfo.color);
	setcolor(savecolor);
	return;
}

void RWFgraph_Circle::draw(RWFgraph_ViewingGeometry &vg) {
	int savecolor;
	int x0,y0,x1,y1,rx, ry;
	static RWFvec_Vector center(3), center2(3), normal(3);
	float radius_eff;
	static fillsettingstype oldfillinfo;

	if(!is3D())
		return;

	center[0] = xCenter;
	center[1] = yCenter;
	center[2] = zCenter;
	center2 = center;
	center2[0] += radius;

	center  = getMatrix() * center;
	center2 = getMatrix() * center2;
	radius_eff = (center - center2).length();

	// A sort of dummy normal
	normal = vg.getEyepoint() - center;
	// Make sure z points up
	normal[2] = ABS(normal[2]);

	center = vg.getProjectedCoordinates(center);
	vg.projectToScreen(center, x0, y0);
	center[0] += radius_eff;
	vg.projectToScreen(center, x1, y1);
	float xdiff = x1 - x0;
	float ydiff = y1 - y0;
	rx = RWFMISC_NINT(sqrt(xdiff*xdiff + ydiff*ydiff));
	if(rx < 0 || rx > 1000)
		return;

	int intensity = vg.computeIntensity(normal);
	intensity *= 1.0 + ((rwf_randRand() - 0.5) * 0.05);
	intensity = RWFMISC_MIN(intensity, 255);
	int drawcolor = (getColor() << 4) + (intensity >> 4);

	getfillsettings(&oldfillinfo);
	setfillstyle(oldfillinfo.pattern,drawcolor);
	savecolor = getcolor();
	setcolor(drawcolor);
	fillellipse(x0,y0,rx,rx);
	setfillstyle(oldfillinfo.pattern,oldfillinfo.color);
	setcolor(savecolor);

	return;
}



/*
**	Functions to support class RWFgraph_2DPlotObject
*/
void RWFgraph_2DPlotObject::plot(void)
{
	int oldcolor, color;
	long i;
	float ulast,vlast;
	RWFvec_Vector xy(2);

	oldcolor = getcolor();
	color = getColor();
	setcolor(color);
	xy[1] = xy[0];
	xy[0] = 0.0;
	for (i=firstIteration; i<maxIterations; i++) {
		ulast = xy[0];
		vlast = xy[1];

		(*plotFunc)(position,xy,userData);

		if(getColor() == 0 && maxIterations < 256) {
			color = i%15 + 1;
			setcolor(i%15 + 1);
		}

		if (connect==RWFMISC_TRUE) {
			if (i>firstIteration) rwf_graphLineDraw(ulast,vlast,xy[0],xy[1]);
		}
		else {
			rwf_graphPointDraw(xy[0],xy[1], color);
		}
	}

	setcolor(oldcolor);
	return;
}



/*
**	Functions to support class RWFGraph_PolyLine
*/

static RWFgraph_Polyline dummy_polyline;

void RWFgraph_Polyline::draw(void) {
	int i,savecolor;
	int lastx, lasty, ix, iy;
	float x,y,tx,ty;
	static RWFvec_Vector tvec(2);

	savecolor = getcolor();
	setcolor(getColor());
	points[0].getCoord(tvec);
	tvec = getMatrix() * tvec;
	lastx = RWFGRAPH_XSCREENCOORD(tvec[0]);
	lasty = RWFGRAPH_YSCREENCOORD(tvec[1]);
	for (i=1; i<numPoints(); i++) {
		points[i].getCoord(tvec);
		tvec = getMatrix() * tvec;
		ix = RWFGRAPH_XSCREENCOORD(tvec[0]);
		iy = RWFGRAPH_YSCREENCOORD(tvec[1]);
		rwf_graphLine(lastx, lasty, ix, iy);
		lastx = ix;
		lasty = iy;
	}

	if(closed) {
		points[0].getCoord(tvec);
		tvec = getMatrix() * tvec;
		ix = RWFGRAPH_XSCREENCOORD(tvec[0]);
		iy = RWFGRAPH_YSCREENCOORD(tvec[1]);
		rwf_graphLine(lastx, lasty, ix, iy);
	}

	setcolor(savecolor);
	return;
}

/*
**	Functions to support class RWFgraph_Polygon
*/

void RWFgraph_Polygon::fill(void) {
	int i,savecolor;
	int *intpoints;
	fillsettingstype oldfillinfo;
	float x,y;
	RWFvec_Vector tvec(2);

	intpoints = rwf_miscNew(2*numPoints(),int);
	for (i=0; i<numPoints(); i++) {
		points[i].getCoord(tvec);
		tvec = getMatrix() * tvec;
		intpoints[2*i]   = RWFGRAPH_XSCREENCOORD(tvec[0]);
		intpoints[2*i+1] = RWFGRAPH_YSCREENCOORD(tvec[1]);
	}

	getfillsettings(&oldfillinfo);
	setfillstyle(oldfillinfo.pattern,getColor());
	savecolor = getcolor();
	setcolor(getColor());
	fillpoly(numPoints(),intpoints);
	setfillstyle(oldfillinfo.pattern,oldfillinfo.color);
	setcolor(savecolor);
	rwf_miscFree(intpoints);
	return;
}

/*
**	3D Drawing Functions
*/

void
RWFgraph_Object::make3D(void)
{
	if(is3D())
		return;

	static RWFmat_Matrix mat3D(3,3);

	setMatrix(mat3D);
}

/*
**	Object list support functions
*/
static RWFgraph_ObjectList dummy_objectlist;

void
RWFgraph_ObjectList::draw(void)
{
	RWFmat_Matrix temp_matrix;
	RWFgraph_Object *optr;

	for(int i=0; i < size(); i++) {
		// Get the pointer to the ith object
		// Because this is an RWFlist_SimpleTable, you can
		// use [] to access each element
		optr = (*this)[i];
		// Composite each object's matrix with the matrix for
		// the entire list
		// Save the original matrix
		temp_matrix = optr->getMatrix();
		optr->setMatrix(getMatrix() * temp_matrix);

		int save_color = optr->getColor();

		if(getColor() == 255)
			optr->setColor(255);
		else if (save_color < 16 && getColor() > 0)
			optr->setColor(((save_color + getColor()) % 15) + 1);
		else
			optr->setColor(save_color + getColor());

		optr->draw();
		// Restore the object's original matrix
		optr->setMatrix(temp_matrix);
		optr->setColor(save_color);
	}
}

void
RWFgraph_ObjectList::fill(void)
{
	RWFmat_Matrix temp_matrix;
	RWFgraph_Object *optr;

	for(int i=0; i < size(); i++) {
		// Get the pointer to the ith object
		// Because this is an RWFlist_SimpleTable, you can
		// use [] to access each element
		optr = (*this)[i];
		// Composite each object's matrix with the matrix for
		// the entire list
		// Save the original matrix
		temp_matrix = optr->getMatrix();
		optr->setMatrix(getMatrix() * temp_matrix);

		int save_color = optr->getColor();

		if(getColor() == 255)
			optr->setColor(255);
		else if (save_color < 16 && getColor() > 0)
			optr->setColor(((save_color + getColor()) % 15) + 1);
		else
			optr->setColor(save_color + getColor());

		optr->fill();
		// Restore the object's original matrix
		optr->setMatrix(temp_matrix);
		optr->setColor(save_color);
	}
}

void
RWFgraph_ObjectList::draw(RWFgraph_ViewingGeometry &vg)
{
	RWFgraph_Object *optr;
	RWFmat_Matrix temp_matrix;

	for(int i=0; i < size(); i++) {
		optr = (*this)[i];
		temp_matrix = optr->getMatrix();
		if(optr->is3D()) {
			optr->setMatrix(getMatrix() * temp_matrix);
		} else {
			optr->setMatrix(getMatrix());
		}
		int save_color = optr->getColor();

		if(getColor() == 255)
			optr->setColor(255);
		else if (save_color < 16 && getColor() > 0)
			optr->setColor(((save_color + getColor()) % 15) + 1);
		else
			optr->setColor(save_color + getColor());

		optr->draw(vg);
		optr->setMatrix(temp_matrix);
		optr->setColor(save_color);
	}

	return;
}

RWFvec_Vector
RWFgraph_Polygon::getNormal(void)
{
	static RWFvec_Vector v0(3), v1(3), v2(3);

	if(points.size() < 3) {
		RWFvec_Vector normal(3);
		normal[0] = 0.0;
		normal[1] = 0.0;
		normal[2] = 1.0;
		return normal;
	}

	v0 = getMatrix()*points[0];
	v1 = getMatrix()*points[1] - v0;
	v2 = getMatrix()*points[2] - v0;

	return (v1.cross(v2));
}

void
RWFgraph_Polygon::draw(RWFgraph_ViewingGeometry &vg)
{
	int i,savecolor;
	int *intpoints;
	fillsettingstype oldfillinfo;
	float x,y;
	int ix, iy;
	static RWFvec_Vector tvec(3), normal(3);

	if(!is3D())
		return;

	normal = getNormal();
	// Now do the backfacing test
	points[0].getCoord(tvec);
	tvec = getMatrix() * tvec;
	if(!vg.isFacing(tvec, normal))
		return;

	intpoints = new int [2*numPoints() + 2];

	for (i=0; i<numPoints(); i++) {
		points[i].getCoord(tvec);
		tvec = getMatrix() * tvec;
		vg.projectCoords(tvec, ix, iy);
		intpoints[2*i]   = ix;
		intpoints[2*i+1] = iy;
	}

	getfillsettings(&oldfillinfo);
	savecolor = getcolor();
	if(getColor() == 255) {
		// Use intensity
		int drawcolor = vg.computeIntensity(normal);
		setfillstyle(oldfillinfo.pattern,drawcolor);
		setcolor(drawcolor);
	} else if(getColor() < 16 && getColor() > 0) {
		int drawcolor = (getColor() << 4) + (vg.computeIntensity(normal) >> 4);
		setfillstyle(oldfillinfo.pattern,drawcolor);
		setcolor(drawcolor);
	} else {
		setfillstyle(oldfillinfo.pattern,getColor());
		setcolor(getColor());
	}

	if(rwf_graphForceWireframe()) {
		intpoints[numPoints()*2] = intpoints[0];
		intpoints[numPoints()*2 + 1] = intpoints[1];
		drawpoly(numPoints() + 1, intpoints);
	} else {
		fillpoly(numPoints(),intpoints);
	}

	setfillstyle(oldfillinfo.pattern,oldfillinfo.color);
	setcolor(savecolor);

	delete intpoints;
	return;
}

/*
**	Utility Functions
*/

void rwf_graphEraseScreen(void)
{
	cleardevice();
	return;
}


void rwf_graphClose(void) {
	closegraph();
	return;
}


void rwf_graphPlot(int i,int j,int color)
{
	putpixel(i,j,color);
	return;
}

unsigned int
rwf_graphGetPixel(int i, int j)
{
	return getpixel(i,j);
}

void rwf_graphLine(int i0,int j0,int i1,int j1)
{
	line(i0,j0,i1,j1);
	return;
}


int rwf_graphMaxColor(void)
{
	return getmaxcolor();
}

// Palette functions

void rwf_graphGetPalette(int start_index, RWFgraph_Palette &pal)
{
	struct REGPACK reg;
	DacPalette256 PalBuf;
	int red, green, blue;

	reg.r_ax = 0x1017;
	reg.r_bx = start_index;
	reg.r_cx = pal.size();
	reg.r_es = FP_SEG(&PalBuf);
	reg.r_dx = FP_OFF(&PalBuf);
	intr(0x10,&reg);

	for(int i=0; i < pal.size(); i++) {
		red   = (PalBuf[i][0] << 2) + 3;
		green = (PalBuf[i][1] << 2) + 3;
		blue  = (PalBuf[i][2] << 2) + 3;
		pal[i].setRGB(red, green, blue);
	}

}


void rwf_graphSetPalette(int start_index, RWFgraph_Palette &pal)
{
	struct REGPACK reg;
	DacPalette256 PalBuf;
	int red, green, blue;

	for(int i=0; i < pal.size(); i++) {
		pal[i].getRGB(red, green, blue);
		PalBuf[i][0] = red >> 2;
		PalBuf[i][1] = green >> 2;
		PalBuf[i][2] = blue >> 2;
	}

	reg.r_ax = 0x1012;
	reg.r_bx = start_index;
	reg.r_cx = pal.size();
	reg.r_es = FP_SEG(&PalBuf);
	reg.r_dx = FP_OFF(&PalBuf);
	intr(0x10,&reg);

}

void rwf_graphSetPaletteIndex(int index, int red, int green, int blue)
{
	struct REGPACK reg;
	unsigned char rgb[3];

	rgb[0] = red >> 2;
	rgb[1] = green >> 2;
	rgb[2] = blue >> 2;

	reg.r_ax = 0x1012;
	reg.r_bx = index;
	reg.r_cx = 1;
	reg.r_es = FP_SEG(&rgb);
	reg.r_dx = FP_OFF(&rgb);
	intr(0x10,&reg);
}

void rwf_graphGetPaletteIndex(int index, int &red, int &green, int &blue)
{
	struct REGPACK reg;
	unsigned char rgb[3];

	reg.r_ax = 0x1017;
	reg.r_bx = index;
	reg.r_cx = 1;
	reg.r_es = FP_SEG(&rgb);
	reg.r_dx = FP_OFF(&rgb);
	intr(0x10,&reg);

	red   = (rgb[0] << 2) | 3;
	green = (rgb[1] << 2) | 3;
	blue  = (rgb[2] << 2) | 3;
}

void rwf_graphSetGrayScale(int maxValue)
{
	RWFgraph_Palette gray(256);

	float scale = (float)255.0 / (float)maxValue;
	for(int i=0; i < 256; i++) {
		int value = RWFMISC_NINT(i*scale);
		value = RWFMISC_MIN(value, 255);
		gray[i].setRGB(value, value, value);
	}

	rwf_graphSetPalette(0, gray);
}

void rwf_graphSetColorScale(void)
{
	RWFgraph_Palette cscale(256);
	int red, green, blue;
	int rcolor, gcolor, bcolor;

	// Define black as color 0
	rwf_graphGetPaletteIndex(0, red, green, blue);
	cscale[0].setRGB(red, green, blue);
	// Now load up the default color scheme
	for(int i=1; i < 16; i++) {
		rwf_graphGetPaletteIndex(i, red, green, blue);
		int index = i*16;
		cscale[i].setRGB(red, green, blue);
		rcolor = red;
		gcolor = green;
		bcolor = blue;
		for(int level=0; level < 16; level++) {
			cscale[index].setRGB(rcolor >> 4, gcolor >> 4, bcolor >> 4);
			rcolor += red;
			gcolor += green;
			bcolor += blue;
			index++;
		}
	}

	rwf_graphSetPalette(0, cscale);
}

/*
**	Utility functions
*/

void rwf_graphLineDraw(float x0,float y0,float x1,float y1)
{
	int i0,j0,i1,j1;
	float xmin,ymin,xmax,ymax;
	float a,b,c;

	xmin = rwf_graphXMin();
	xmax = rwf_graphXMax();
	ymin = rwf_graphYMin();
	ymax = rwf_graphYMax();

	if ((x0<xmin) || (x0>xmax)) return;
	if ((y0<ymin) || (y0>ymax)) return;

	a = y1-y0;
	b = x0-x1;
	c = -(a*x0 + b*y0);
	if (x1<xmin) {
		x1 = xmin;
		y1 = -(a*x1+c)/b;
		if (y1<ymin) {
			y1 = ymin;
			x1 = -(b*y1+c)/a;
		}
		if (y1>ymax) {
			y1 = ymax;
			x1 = -(b*y1+c)/a;
		}
	}
	else if (x1>xmax) {
		x1 = xmax;
		y1 = -(a*x1+c)/b;
		if (y1<ymin) {
			y1 = ymin;
			x1 = -(b*y1+c)/a;
		}
		if (y1>ymax) {
			y1 = ymax;
			x1 = -(b*y1+c)/a;
		}
	}
	else if (y1<ymin) {
		y1 = ymin;
		x1 = -(b*y1+c)/a;
	}
	else if (y1>ymax) {
		y1 = ymax;
		x1 = -(b*y1+c)/a;
	}

	i0 = RWFGRAPH_XSCREENCOORD(x0);
	j0 = RWFGRAPH_YSCREENCOORD(y0);
	i1 = RWFGRAPH_XSCREENCOORD(x1);
	j1 = RWFGRAPH_YSCREENCOORD(y1);
	line(i0,j0,i1,j1);

	return;
}


void rwf_graphPointDraw(float x,float y,int color)
{
	int i,j;

	if ((x<rwf_graphXMin()) || (x>rwf_graphXMax())) return;
	if ((y<rwf_graphYMin()) || (y>rwf_graphYMax())) return;
	i = RWFGRAPH_XSCREENCOORD(x);
	if ((i<rwf_graphIMin()) || (i>rwf_graphIMax())) return;
	j = RWFGRAPH_YSCREENCOORD(y);
	if ((j<rwf_graphJMin()) || (j>rwf_graphJMax())) return;
	putpixel(i,j,color);

	return;
}


void rwf_graphCircleDraw(float x0,float y0,float r)
{
	int i0,j0,rint;

	i0 = (int) RWFGRAPH_XSCREENCOORD(x0);
	j0 = (int) RWFGRAPH_YSCREENCOORD(y0);
	rint = RWFMISC_NINT(r * rwf_graphAveScale());
	circle(i0,j0,rint);

	return;
}


void rwf_graphCircleFill(float x0,float y0,float r)
{
	int i0,j0,rint;

	i0 = (int) RWFGRAPH_XSCREENCOORD(x0);
	j0 = (int) RWFGRAPH_YSCREENCOORD(y0);
	rint = RWFMISC_NINT(r * rwf_graphAveScale());
	fillellipse(i0,j0,rint,rint);

	return;
}


void rwf_graphBarDraw(float x0,float y0,float x1,float y1)
{
	int i0,j0,i1,j1;

	i0 = (int) RWFGRAPH_XSCREENCOORD(x0);
	j0 = (int) RWFGRAPH_YSCREENCOORD(y0);
	i1 = (int) RWFGRAPH_XSCREENCOORD(x1);
	j1 = (int) RWFGRAPH_YSCREENCOORD(y1);
	bar(i0,j0,i1,j1);

	return;
}

// Normalization functions

void
rwf_graphNormalize(RWFgraph_LineSegmentTable &lt)
{
	if(lt.size() <= 1)
		return;

	int last_seg = lt.size() - 1;
	RWFgraph_Point fpoint, lpoint;
	RWFvec_Vector coord1, coord2, delta_coord;
	RWFgraph_Line end_to_end;
	RWFgraph_Line unit(-0.5, 0.0, 0.5, 0.0);

	// Get the first and last points
	fpoint = lt[0].getFirst();
	lpoint = lt[last_seg].getEnd();

	// Set the normalization vector
	end_to_end.setFirst(fpoint);
	end_to_end.setEnd(lpoint);

	RWFmat_Matrix tmatrix;
	tmatrix = rwf_graphLineToLineMatrix(end_to_end, unit);

	// Now scale everything with the overloaded * operator
	for(int i=0; i < lt.size(); i++)
		lt[i] = tmatrix * lt[i];

	return;
}

void
rwf_graphNormalize(RWFgraph_Polyline &poly)
{
	RWFgraph_LineSegmentTable ltable;

	for(int i=0; i < poly.numPoints() - 1; i++) {
		ltable[i].setFirst(poly[i]);
		ltable[i].setEnd(poly[i+1]);
	}

	rwf_graphNormalize(ltable);

	for(i=0; i < poly.numPoints(); i++) {
		float x, y;
		if(i < poly.numPoints() - 1)
			ltable[i].getFirst(x, y);
		else
			ltable[i-1].getEnd(x, y);
		poly[i].setCoord(x, y);
	}

	return;
}



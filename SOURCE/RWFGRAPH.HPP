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

#ifndef RWFGRAPH_HPP
#define RWFGRAPH_HPP 1

#include "rwflist.hpp"
#include "rwfvec.hpp"
#include "rwfmat.hpp"
#include "rwfmisc.hpp"

#include <math.h>
#include <complex.h>
#include <graphics.h>


#define RWFGRAPH_GLOBAL_SCALE 1.0
#define RWFGRAPH_BEVEL_FRAC 0.1

#define RWFGRAPH_COLOR_WHITE WHITE
#define RWFGRAPH_COLOR_BLACK BLACK
#define RWFGRAPH_COLOR_GREEN GREEN
#define RWFGRAPH_COLOR_LIGHT_GRAY LIGHTGRAY

// Utility macros for 2D drawing

int rwf_graphGetMaxX(void);
int rwf_graphGetMaxY(void);
int rwf_graphIMin(void);
int rwf_graphIMax(void);
int rwf_graphJMin(void);
int rwf_graphJMax(void);
float rwf_graphXMin(void);
float rwf_graphXMax(void);
float rwf_graphYMin(void);
float rwf_graphYMax(void);
float rwf_graphXScale(void);
float rwf_graphYScale(void);
float rwf_graphAveScale(void);
float rwf_graphXOffset(void);
float rwf_graphYOffset(void);
void rwf_graphSetWindow(float,float,float,float);
void rwf_graphSetScreenWindow(int,int,int,int);
void rwf_graphInitialize(int);
void rwf_graphSetWireframeMode(int);

RWFmisc_Boolean rwf_graphIsInitialized(void);

// Same as macros, but lets compiler do some checking for us

inline int RWFGRAPH_XSCREENCOORD(float X) {
	return RWFMISC_NINT((X) * rwf_graphXScale() + rwf_graphXOffset());
}
inline int RWFGRAPH_YSCREENCOORD(float Y) {
	return RWFMISC_NINT((Y) * rwf_graphYScale() + rwf_graphYOffset());
}


typedef enum {
	RWFGRAPH_UNKNOWN_OBJECT,
	RWFGRAPH_POLYLINE_OBJECT,
	RWFGRAPH_CIRCLE_OBJECT,
	RWFGRAPH_LIST_OBJECT
} RWFgraph_ObjectType;



class RWFgraph_Color{
private:
	unsigned char red;
	unsigned char green;
	unsigned char blue;

protected:

public:
	RWFgraph_Color(void) {red = 255; green = 255; blue = 255;}
	void setColor(RWFgraph_Color *colorIn) {
		red = colorIn->red;
		green = colorIn->green;
		blue = colorIn->blue;
		return;
	};
	void setRGB(int redIn, int greenIn, int blueIn) {
		red = redIn; green = greenIn; blue = blueIn;};

	void getRGB(int &redOut, int &greenOut, int &blueOut) {
		redOut = red; greenOut = green; blueOut = blue;};

};

typedef RWFlist_SimpleTable<RWFgraph_Color> RWFgraph_Palette;

class RWFgraph_Object : public RWFmat_Matrix {
private:

	RWFgraph_ObjectType type;  // What type am I?
	int color;                 // Drawing color
	RWFvec_Vector ref_point;   // Reference point for 3D objects

protected:
public:

	RWFgraph_Object(void) : RWFmat_MatrixTemplate<float> (2,2) {
		type = RWFGRAPH_UNKNOWN_OBJECT;
		color = RWFGRAPH_COLOR_WHITE;
		RWFvec_Vector v(3);
		ref_point = v;
	};
	void setType(RWFgraph_ObjectType typeIn) {
		type = typeIn;
		return;
	};
	void setColor(int colorIn) {color = colorIn;};
	void setMatrix(RWFmat_Matrix &matrixIn) {
		*((RWFmat_Matrix *)this) = matrixIn;
	};
	RWFmat_Matrix &getMatrix() {
		return *this;
	};

	void move(float xoff, float yoff)
		{translate(xoff, yoff);};
	void move(float xoff, float yoff, float zoff)
		{translate(xoff, yoff, zoff);};

	RWFgraph_ObjectType getType(void) {return type;};
	int getColor(void) {return color;};

	virtual void draw(void) {};
	virtual void fill(void) {};
	virtual void draw(class RWFgraph_ViewingGeometry &) {};

	RWFmisc_Boolean is3D(void)	{
		if(ncols() == 3)
			return RWFMISC_TRUE;
		else
			return RWFMISC_FALSE;
	};
	RWFmisc_Boolean is2D(void) {
		if(ncols() == 2)
			return RWFMISC_TRUE;
		else
			return RWFMISC_FALSE;
	};

	void make3D(void);	// Forces the object to 3D

	void setReferencePoint(RWFvec_Vector &new_p)
		{ref_point[0] = new_p[0]; ref_point[1] = new_p[1];
		 if(new_p.size() > 2) ref_point[2] = new_p[2];};

	RWFvec_Vector getReferencePoint(void)
		{return ref_point;};

};

// Define table of object pointers

typedef RWFlist_SimpleTable<class RWFgraph_Object *> RWFlist_ObjectPtrTable;

// Graphics object classes

/*
**	A single point
*/

class RWFgraph_Point : public RWFvec_Vector {
	private:

	protected:

	public:

	RWFgraph_Point(void) : RWFvec_VectorTemplate<float>(3) {};

	RWFgraph_Point(float xCoordIn, float yCoordIn) {
		(*this)[0] = xCoordIn;
		(*this)[1] = yCoordIn;
	}

	RWFgraph_Point(float xCoordIn, float yCoordIn, float zCoordIn) {
		(*this)[0] = xCoordIn;
		(*this)[1] = yCoordIn;
		(*this)[2] = zCoordIn;
	}

	void setCoord(float xCoordIn,float yCoordIn) {
		(*this)[0] = xCoordIn;
		(*this)[1] = yCoordIn;
		return;
	}

	void setCoord(RWFgraph_Point &pIn) {
		*this = pIn;
	};

	void setCoord(RWFvec_Vector &v) {
		(*this)[0] = v[0];
		(*this)[1] = v[1];
		if(v.size() > 2)
			(*this)[2] = v[2];
	}

	void getCoord(float &xCoordIn,float &yCoordIn) {
		xCoordIn = (*this)[0];
		yCoordIn = (*this)[1];
		return;
	}

	void getCoord(RWFvec_Vector &v) {
		v[0] = (*this)[0];
		v[1] = (*this)[1];
		if(v.size() > 2)
			v[2] = (*this)[2];
	}

	void setCoord(float x, float y, float z) {
		(*this)[0] = x;
		(*this)[1] = y;
		(*this)[2] = z;
	}

	void getCoord(float &x, float &y, float &z) {
		x = (*this)[0];
		y = (*this)[1];
		z = (*this)[2];
	}

};

class RWFgraph_Line:public RWFgraph_Object {
private:
	RWFgraph_Point first, end;

protected:

public:
	RWFgraph_Line(void) {
	};
	RWFgraph_Line(float xFirstIn,float yFirstIn,
		float xEndIn,float yEndIn) {
		first.setCoord(xFirstIn, yFirstIn);
		end.setCoord(xEndIn, yEndIn);
		//return;
	};
	void setFirst(float xIn,float yIn) {
		first.setCoord(xIn, yIn);
		return;
	};
	void setEnd(float xIn,float yIn) {
		end.setCoord(xIn, yIn);
		return;
	};
	void setFirst(RWFgraph_Point &pIn) {
		first.setCoord(pIn);
		return;
	}
	void setEnd(RWFgraph_Point &pIn) {
		end.setCoord(pIn);
		return;
	}
	void getFirst(float &xIn, float &yIn) {
		first.getCoord(xIn, yIn);
		return;
	}
	void getEnd(float &xIn, float &yIn) {
		end.getCoord(xIn, yIn);
		return;
	}
	RWFgraph_Point getFirst(void) {
		return first;
	}
	RWFgraph_Point getEnd(void) {
		return end;
	}
	float length(void) {
		float xFirst, yFirst, xEnd, yEnd;
		first.getCoord(xFirst, yFirst);
		end.getCoord(xEnd, yEnd);
		return sqrt((xEnd - xFirst) * (xEnd - xFirst) +
								(yEnd - yFirst) * (yEnd - yFirst));
	}
	void draw(void);
	void fill(void)	{draw();};
};

RWFgraph_Line operator * (RWFmat_Matrix &mat, RWFgraph_Line &lIn);

typedef RWFlist_SimpleTable<class RWFgraph_Line> RWFgraph_LineSegmentTable;
typedef RWFgraph_LineSegmentTable RWFgraph_LineTable;
typedef RWFgraph_Line	RWFgraph_LineSegment;

class RWFgraph_Polyline:public RWFgraph_Object {
private:
protected:
	RWFlist_SimpleTable<RWFgraph_Point> points;
	RWFmisc_Boolean closed;

public:
	RWFgraph_Polyline(void)
		{setType(RWFGRAPH_POLYLINE_OBJECT); closed = RWFMISC_FALSE;};

	int numPoints(void)	{return points.size();};
	void reset(void)
		{points.reset();};
	RWFgraph_Polyline(int np)
		{setType(RWFGRAPH_POLYLINE_OBJECT); closed = RWFMISC_FALSE; points[np-1];};
	RWFgraph_Point& operator[](unsigned int index)
		{return points[index];};
	void draw();

};


class RWFgraph_Polygon:public RWFgraph_Polyline {
private:
protected:
public:
	RWFgraph_Polygon(void)
		{closed = RWFMISC_TRUE;};
	RWFgraph_Polygon(int np) : RWFgraph_Polyline(np)
		{closed = RWFMISC_TRUE;};
	void draw(void)	{RWFgraph_Polyline::draw();};
	void fill(void);

	void draw(class RWFgraph_ViewingGeometry &);
	RWFvec_Vector getNormal(void);
};

class RWFgraph_Circle:public RWFgraph_Object {
private:
	float xCenter;
	float yCenter;
	float zCenter;
	float radius;

protected:

public:
	RWFgraph_Circle(float xCenterIn,float yCenterIn,float radiusIn) {
		xCenter = xCenterIn;
		yCenter = yCenterIn;
		zCenter = 0.0;
		radius = radiusIn;
	}
	RWFgraph_Circle(float xCenterIn, float yCenterIn, float zCenterIn,
									float radiusIn) {
		xCenter = xCenterIn;
		yCenter = yCenterIn;
		zCenter = zCenterIn;
		radius  = radiusIn;
		make3D();
	}
	void setRadius(float radiusIn) {radius = radiusIn; return;}
	void setCenter(float xCenterIn,float yCenterIn) {
		xCenter = xCenterIn;
		yCenter = yCenterIn;
		return;
	}
	void setCenter(float xCenterIn,float yCenterIn, float zCenterIn) {
		xCenter = xCenterIn;
		yCenter = yCenterIn;
		zCenter = zCenterIn;
		make3D();
		return;
	}
	float getRadius(void) {return radius;}
	void getCenter(float &xCenterIn,float &yCenterIn) {
		xCenterIn = xCenter;
		yCenterIn = yCenter;
		return;
	}
	void getCenter(float &xCenterIn,float &yCenterIn, float &zCenterIn) {
		xCenterIn = xCenter;
		yCenterIn = yCenter;
		zCenterIn = zCenter;
		return;
	}
	void draw(void);
	void fill(void);
	void draw(RWFgraph_ViewingGeometry &);
};


class RWFgraph_ComplexObject:public RWFgraph_Object {
private:
	void calcAdjWindow(float xOrigin,float yOrigin,
		double scale) {
		float xMinAdj,xMaxAdj,yMinAdj,yMaxAdj;
		xMinAdj = xOrigin - (rwf_graphXMax()-rwf_graphXMin())/(2.0*scale);
		xMaxAdj = xOrigin + (rwf_graphXMax()-rwf_graphXMin())/(2.0*scale);
		yMinAdj = yOrigin - (rwf_graphYMax()-rwf_graphYMin())/(2.0*scale);
		yMaxAdj = yOrigin + (rwf_graphYMax()-rwf_graphYMin())/(2.0*scale);
		rwf_graphSetWindow(xMinAdj,yMinAdj,xMaxAdj,yMaxAdj);
		return;
	}

protected:

public:
	int getMinX(void) {return rwf_graphIMin();}
	int getMaxX(void) {return rwf_graphIMax();}
	int getMinY(void) {return rwf_graphJMin();}
	int getMaxY(void) {return rwf_graphJMax();}
	void setScreenWindow(int iMinIn,int jMinIn,int iMaxIn,
		int jMaxIn) {
		rwf_graphSetScreenWindow(iMinIn,jMinIn,iMaxIn,jMaxIn);
		return;
	};
	void getScreenWindow(int &iMinIn,int &jMinIn,int &iMaxIn,
		int &jMaxIn) {
		iMinIn = rwf_graphIMin();
		iMaxIn = rwf_graphIMax();
		jMinIn = rwf_graphJMin();
		jMaxIn = rwf_graphJMax();
		return;
	};
	void setWindow(float xMinIn,float yMinIn,float xMaxIn,
		float yMaxIn) {
		float xOrigin,yOrigin,scale;
		rwf_graphSetWindow(xMinIn,yMinIn,xMaxIn,yMaxIn);
		xOrigin = (xMinIn + xMaxIn)/2.0;
		yOrigin = (yMinIn + yMaxIn)/2.0;
		scale = 1.0;
		calcAdjWindow(xOrigin,yOrigin,scale);
		return;
	};
	void getWindow(float &xMinIn,float &yMinIn,float &xMaxIn,
		float &yMaxIn) {
		xMinIn = rwf_graphXMin();
		xMaxIn = rwf_graphXMax();
		yMinIn = rwf_graphYMin();
		yMaxIn = rwf_graphYMax();
		return;
	};
	void zoomWindow(float xOriginIn,float yOriginIn,
		double scaleIn) {
		calcAdjWindow(xOriginIn,yOriginIn,scaleIn);
		return;
	};
	complex getComplex(int iIn,int jIn) {
		double x,y;
		complex value;

		x = (iIn-rwf_graphIMin()+0.5)*(rwf_graphXMax()-rwf_graphXMin())/
			(rwf_graphIMax()+1.0-rwf_graphIMin()) + rwf_graphXMin();
		y = (rwf_graphJMax()-jIn+0.5)*(rwf_graphYMax()-rwf_graphYMin())/
			(rwf_graphJMax()+1.0-rwf_graphJMin()) + rwf_graphYMin();
		value = complex(x,y);

		return value;
	}
};


class RWFgraph_2DPlotObject:public RWFgraph_Object {
private:
	int iMin;
	int iMax;
	int jMin;
	int jMax;
	float xMin;
	float xMax;
	float yMin;
	float yMax;
	float xOrigin;
	float yOrigin;
	double scale;
	float xMinAdj;
	float xMaxAdj;
	float yMinAdj;
	float yMaxAdj;
	int (*plotFunc)(RWFvec_Vector &,RWFvec_Vector &,void *);
	RWFvec_Vector position;
	void *userData;
	RWFmisc_Boolean connect;
	long maxIterations;
	long firstIteration;
	void calcAdjWindow(void) {
		xMinAdj = xOrigin - (xMax-xMin)/(2.0*scale);
		xMaxAdj = xOrigin + (xMax-xMin)/(2.0*scale);
		yMinAdj = yOrigin - (yMax-yMin)/(2.0*scale);
		yMaxAdj = yOrigin + (yMax-yMin)/(2.0*scale);
		rwf_graphSetWindow(xMinAdj,yMinAdj,xMaxAdj,yMaxAdj);
		return;
	}
	void calcDefault(void) {
		int i, j;
		float aspectratio;
		iMin = 0;
		iMax = getmaxx();
		jMin = 0;
		jMax = getmaxy();
		getaspectratio((int far *) &i,(int far *) &j);
		aspectratio = 10000.0 / i;
		xMin = -1.0 * aspectratio *
			(((float) (iMax-iMin)) / (jMax-jMin));
		xMax =  1.0 * aspectratio *
			(((float) (iMax-iMin)) / (jMax-jMin));
		yMin = -1.0;
		yMax = 1.0;
	}


protected:

public:
	RWFgraph_2DPlotObject(void) {
		iMin = -1;
		iMax = -1;
		jMin = -1;
		jMax = -1;
		xMin = -1.0;
		xMax =  1.0;
		yMin = -1.0;
		yMax = 1.0;
		if(rwf_graphIsInitialized())
			calcDefault();
		xOrigin = 0.0;
		yOrigin = 0.0;
		scale = 1.0;
		calcAdjWindow();
		plotFunc = NULL;
		position = RWFvec_Vector(3);
		userData = NULL;
		connect = RWFMISC_TRUE;
		maxIterations = 10000;
		firstIteration = 0;
	}
	int getMinX(void) {return iMin;};
	int getMaxX(void) {return iMax;};
	int getMinY(void) {return jMin;};
	int getMaxY(void) {return jMax;};
	void setScreenWindow(int iMinIn,int jMinIn,int iMaxIn,
		int jMaxIn) {
		iMin = iMinIn;
		iMax = iMaxIn;
		jMin = jMinIn;
		jMax = jMaxIn;
		return;
	};
	void getScreenWindow(int &iMinIn,int &jMinIn,int &iMaxIn,
		int &jMaxIn) {
		iMinIn = iMin;
		iMaxIn = iMax;
		jMinIn = jMin;
		jMaxIn = jMax;
		return;
	};
	void setWindow(float xMinIn,float yMinIn,float xMaxIn,
		float yMaxIn) {
		xMin = xMinIn;
		xMax = xMaxIn;
		yMin = yMinIn;
		yMax = yMaxIn;
		xOrigin = (xMin + xMax)/2.0;
		yOrigin = (yMin + yMax)/2.0;
		scale = 1.0;
		calcAdjWindow();
		return;
	};
	void getWindow(float &xMinIn,float &yMinIn,float &xMaxIn,
		float &yMaxIn) {
		xMinIn = xMin;
		xMaxIn = xMax;
		yMinIn = yMin;
		yMaxIn = yMax;
		return;
	};
	void zoomWindow(float xOriginIn,float yOriginIn,
		double scaleIn) {
		xOrigin = xOriginIn;
		yOrigin = yOriginIn;
		scale = scaleIn;
		calcAdjWindow();
		return;
	};
	void setPlotFunction(int (*plotFuncIn)(RWFvec_Vector &,
		RWFvec_Vector &,void *)) {
		plotFunc = plotFuncIn;
		return;
	};
	void setInitialPoint(RWFvec_Vector &initPosIn,void *userDataIn,
		int connectIn) {
		position = initPosIn;
		userData = userDataIn;
		if (connectIn==0) connect = RWFMISC_FALSE;
		else connect = RWFMISC_TRUE;
		return;
	}
	void setNumIterations(long maxIterationsIn) {
		maxIterations = maxIterationsIn;
		return;
	};
	void setFirstIteration(long firstIterationIn) {
		firstIteration = firstIterationIn;
		return;
	};
	void plot(void);
	void draw(void) {plot();};
};


// 3D support classes

class RWFgraph_ObjectList : public RWFgraph_Object, public RWFlist_ObjectPtrTable {
	private:
	protected:
	public:

	RWFgraph_ObjectList(void)
		{setType(RWFGRAPH_LIST_OBJECT); setColor(0);};
	RWFgraph_ObjectList(int nobjects)
		{setType(RWFGRAPH_LIST_OBJECT); setColor(0); (*this)[nobjects-1];};

	// We want the [] operator to return the nth object
	RWFgraph_Object * & operator [] (unsigned int i)
		{return ((*(RWFlist_ObjectPtrTable *)this)[i]);};

	void draw(void);
	void fill(void);
	void draw(class RWFgraph_ViewingGeometry &);
};

class RWFgraph_ViewingGeometry {
	private:

	RWFmisc_Boolean isSetup;
	RWFvec_Vector eyepos, eyedir;
	float xfov, yfov, orientation;
	RWFmat_Matrix forward_matrix, rotate_matrix;
	RWFmat_Matrix coord_matrix;

	RWFvec_Vector light_direction;
	float ambient, contrast;

	int xul, yul, xlr, ylr;

	void setup(void);

	protected:
	public:

	RWFgraph_ViewingGeometry(void);

	void setEyepoint(float xeye, float yeye, float zeye)
		{eyepos[0] = xeye; eyepos[1] = yeye; eyepos[2] = zeye;
		 isSetup = RWFMISC_FALSE; return;};

	void setEyepoint(RWFvec_Vector &eye)
		{eyepos = eye; isSetup = RWFMISC_FALSE; return;};

	RWFvec_Vector getEyepoint(void)	{return eyepos;};

	void setEyeDirection(RWFvec_Vector &dirvec)
		{eyedir = dirvec; isSetup = RWFMISC_FALSE; return;};

	RWFvec_Vector getEyeDirection(void)	{return eyedir;};

	void setFOV(float xfovIn, float yfovIn)
		{xfov = xfovIn; yfov = yfovIn; isSetup = RWFMISC_FALSE;};

	void setOrientation(float angle)
		{orientation = angle; isSetup = RWFMISC_FALSE;};
	float getOrientation(void)
		{return orientation;};

	void setScreen(int x1, int y1, int x2, int y2)
		{xul = x1; yul = y1; xlr = x2; ylr = y2; isSetup = RWFMISC_FALSE;};
	void getScreen(int &x1, int &y1, int &x2, int &y2)
		{x1 = xul; y1 = yul; x2 = xlr; y2 = ylr; return;};

	RWFvec_Vector getProjectedCoordinates(RWFvec_Vector &v);
	void projectCoords(RWFvec_Vector &vin, int &xscreen, int &yscreen);
	void projectToScreen(RWFvec_Vector &uvz, int &xscreen, int &yscreen);

	void setLight(RWFvec_Vector &dirvec)
		{light_direction = dirvec; isSetup = RWFMISC_FALSE;};

	void setLightAttributes(float ambIn, float conIn)
		{ambient = ambIn; contrast = conIn; isSetup = RWFMISC_FALSE; return;};

	RWFmisc_Boolean isFacing(RWFvec_Vector &ref, RWFvec_Vector &normal)
		{if( (normal * (ref - eyepos)) < 0.0 ) return RWFMISC_TRUE;
		 else                                  return RWFMISC_FALSE;};

	int computeIntensity(RWFvec_Vector &normal);
};

class RWFgraph_Tetrahedron : public RWFgraph_ObjectList
{
	public:

	RWFgraph_Tetrahedron(void);
};

class RWFgraph_Cube : public RWFgraph_ObjectList
{
	public:

	RWFgraph_Cube(void);
};

// Utility functions

void rwf_graphSetPalette(int, RWFgraph_Palette &);
void rwf_graphGetPalette(int, RWFgraph_Palette &);
void rwf_graphSetGrayScale(int);
void rwf_graphSetColorScale(void);
void rwf_graphSetPaletteIndex(int, int, int, int);
void rwf_graphGetPaletteIndex(int, int &, int &, int &);
void rwf_graphEraseScreen(void);
void rwf_graphClose(void);
void rwf_graphPlot(int,int,int);
unsigned int  rwf_graphGetPixel(int, int);
void rwf_graphLine(int,int,int,int);
int rwf_graphMaxColor(void);
void rwf_graphLineDraw(float,float,float,float);
void rwf_graphPointDraw(float,float,int);
void rwf_graphCircleDraw(float,float,float);
void rwf_graphCircleFill(float,float,float);
void rwf_graphBarDraw(float,float,float,float);

void rwf_graphNormalize(RWFgraph_Polyline &poly);
void rwf_graphNormalize(RWFgraph_LineSegmentTable &lt);

RWFmat_Matrix rwf_graphLineToLineMatrix(RWFgraph_Line &lSrc,
																				RWFgraph_Line &lDest);

#endif

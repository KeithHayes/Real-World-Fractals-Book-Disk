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


#include "rwfjulia.hpp"
#include "rwfgraph.hpp"
#include "rwfrand.hpp"
#include "svga256.h"

#include <string.h>
#include <conio.h>
#include <dos.h>

static void draw_cursor(int, int);

int main(int argc,char *argv[])
{
	int i,command,numiterations=64,power=2,xcursor,ycursor;
	int xmid, ymid;
	float xcenter=0.0, ycenter=0.0;
	char string[100], string2[100];
	RWFgraph_Palette pal(16), pal2(16);
  int mode = SVGA640x480x256;

	float x,y;
	double scale=0.5;

	complex tempc;
	complex (*trans_func)(complex &,int,void *) = rwf_juliaZPowerTransform;

	RWFJulia_Object jobject;
	RWFJulia_zpowerdata zdata,zdatasave;

	//	Create the data for the transformation
	zdata.power = power;
	zdata.c = complex(-0.5, 0.5);
	//	Compute the cutoff threshold
	zdata.cutoff = RWFMISC_MAX(2.0,abs(zdata.c));

/*
**	Read command line options, if any
*/
	i = 1;
	while (i<argc) {
		if (strcmp("-center",argv[i])==0) {
			i++;
			sscanf(argv[i],"%f",&xcenter);
			i++;
			sscanf(argv[i],"%f",&ycenter);
		}
		else if (strcmp("-Julia",argv[i])==0) {
			trans_func = rwf_juliaZPowerTransform;
		}
		else if (strcmp("-Mandelbrot",argv[i])==0) {
			trans_func = rwf_juliaMandelbrotTransform;
		}
		else if (strcmp("-numiterations",argv[i])==0) {
			i++;
			sscanf(argv[i],"%d",&numiterations);
		}
		else if (strcmp("-offset",argv[i])==0) {
			i++;
			sscanf(argv[i],"%f",&x);
			i++;
			sscanf(argv[i],"%f",&y);
			zdata.c = complex(x,y);
			zdata.cutoff = RWFMISC_MAX(2.0,abs(zdata.c));
		}
		else if (strcmp("-power",argv[i])==0) {
			i++;
			sscanf(argv[i],"%d",&power);
			zdata.power = power;
		}
		else if (strcmp("-scale",argv[i])==0) {
			i++;
			sscanf(argv[i],"%f",&scale);
		}
		else if (strcmp("-?",argv[i])==0) {
			printf("Usage:  julia [-?] [-center x y] [-Julia]\n");
			printf("    [-Mandelbrot] [-numiterations count]\n");
			printf("    [-offset c_real c_imag] [-power exponent]\n");
			printf("    [-scale zoom]\n");
			printf("    -center        :  Set display center\n");
			printf("    -Julia         :  Display Julia set\n");
			printf("    -Mandelbrot    :  Display Mandelbrot set\n");
			printf("    -numiterations :  Set number of iterations\n");
			printf("    -offset        :  Set function offset\n");
			printf("    -power         :  Set function exponent\n");
			printf("    -scale         :  Set display scale\n");
			printf("    -?             :  Display usage information (i.e., this message)\n");
			return 0;
		}
		else {
			printf("***Unrecognized command-line argument \"%s\"\n",argv[(int) i]);
			return 1;
		}
		i++;
	}

	rwf_miscTitle("Julia", 8);

	rwf_graphInitialize(0);
	string[0] = 0;
	string2[0] = 0;

	// Setup color cycling palette
	rwf_graphGetPalette(0, pal);
	rwf_graphGetPalette(0, pal2);

/*
**	OK, now clear the screen & draw it
*/
drawobject:

	rwf_graphEraseScreen();

	xmid = rwf_graphGetMaxX()/2;
	ymid = rwf_graphGetMaxY()/2;

	jobject.setTransformation(trans_func, &zdata);
	jobject.setEscapeFunction(&rwf_juliaZPowerEscape,&zdata);
	jobject.setActionFunction(&rwf_juliaDrawColor,&zdata);
	jobject.zoomWindow(xcenter,ycenter,scale);
	jobject.setMaximumLevel(numiterations);
	zdatasave = zdata;
	jobject.process();
	zdata = zdatasave;

	xcursor = xmid;
	ycursor = ymid;
	setwritemode(XOR_PUT);
	draw_cursor(xcursor, ycursor);
	setwritemode(COPY_PUT);

/*
**	Return
*/
getcommand:

	// Update status
	setcolor(0);
	outtextxy(0, 0, string);
	outtextxy(0, textheight(string2)+1, string2);
	sprintf(string,"Evaluating z = z**%1d + c = (%5.2f + %5.2fi), Cursor at (%6.3f + %6.3fi)",
								 power, real(zdata.c), imag(zdata.c), xcenter, ycenter);
	setcolor(15);
	outtextxy(0, 0, string);
	sprintf(string2,"Using %d iterations",numiterations);
	outtextxy(0, textheight(string2)+1, string2);

	scale = 1.0;

	// Get a command
	command = rwf_miscPressAnyKey("Press command key (arrow,+,-,m,p,r,z,c,J,M), q to quit...", 1);
	switch (command) {
	case 0:
		setwritemode(XOR_PUT);
		draw_cursor(xcursor, ycursor);
		setwritemode(COPY_PUT);

		int key = getch();
		switch(key) {
			case 72: ycursor -= 5; break;
			case 77: xcursor += 5; break;
			case 80: ycursor += 5; break;
			case 75: xcursor -= 5; break;
			case 71:
				xcursor = xmid;
				ycursor = ymid;
				break;
		}

		tempc = jobject.getComplex(xcursor,ycursor);
		xcenter = real(tempc);
		ycenter = imag(tempc);

		setwritemode(XOR_PUT);
		draw_cursor(xcursor, ycursor);
		setwritemode(COPY_PUT);
		goto getcommand;
	case '+':
		scale = 5.0;
		goto drawobject;
	case '-':
		scale = 0.2;
		goto drawobject;
	case 'm':
		rwf_graphClose();
		printf("Enter maximum recursion level:  ");
		gets(string);
		sscanf(string,"%d",&numiterations);
		string[0] = 0;
		rwf_graphInitialize(0);
		goto drawobject;
	case 'p':
		rwf_graphClose();
		printf("Enter new exponent (currently %d) :  ", power);
		gets(string);
		if(strlen(string) > 0) {
			sscanf(string,"%d",&power);
			zdata.power = power;
		}
		printf("Enter real      portion of c (currently %f): ",real(zdata.c));
		gets(string);
		if(strlen(string) > 0)
			sscanf(string,"%f", &x);
		else
			x = real(zdata.c);

		printf("Enter imaginary portion of c (currently %f): ",imag(zdata.c));
		gets(string);
		if(strlen(string) > 0)
			sscanf(string,"%f", &y);
		else
			y = imag(zdata.c);
		zdata.c = complex(x, y);
		zdata.cutoff = RWFMISC_MAX((float)power,abs(zdata.c));
		string[0] = 0;
		rwf_graphInitialize(0);
		goto drawobject;
	case 'q':
		rwf_graphClose();
		return 0;
	case 'r':
		while (!kbhit()) {
			rwf_graphEraseScreen();
			x = rwf_randRand();
			y = RWFMISC_TWOPI*rwf_randRand();
			xcenter = x*cos(y);
			ycenter = x*sin(y);
			if(trans_func == rwf_juliaZPowerTransform) {
				zdata.c = complex(xcenter, ycenter);
			} else {
				jobject.zoomWindow(2.0*xcenter,2.0*ycenter,1.1);
			}
			jobject.process();
			int index = 1;
			for(int loop=0; loop < 60; loop++) {
				for(i=1; i < 16; i++)
					pal2[i] = pal[(i+index)%15 + 1];
				rwf_graphSetPalette(0, pal2);
				delay(100);
				index = (index+1) % 16;
			}
			rwf_graphSetPalette(0, pal);
		}
		xcenter = 0.0;
		ycenter = 0.0;
		goto drawobject;
	case 'z':
		rwf_graphClose();
		printf("Enter zoom factor:  ");
		gets(string);
		if(!sscanf(string,"%lg",&scale)) {
			sscanf(string,"%d",&i);
			scale = i;
		}
		string[0] = 0;
		rwf_graphInitialize(0);
		goto drawobject;
	case 'c':
		int index = 1;
		while(!kbhit()) {
			for(i=1; i < 16; i++)
				pal2[i] = pal[(i+index)%15 + 1];
			rwf_graphSetPalette(0, pal2);
			delay(100);
			index = (index+1) % 16;
		}
		rwf_graphSetPalette(0, pal);
		goto getcommand;
	case 'J':
		trans_func = rwf_juliaZPowerTransform;
		// Reset the scale
		jobject.setWindow(-2.0*1.333,-2.0,2.0*1.333,2.0);
		// Use the cursor as the value of c for the Julia set
		zdata.c = complex(xcenter, ycenter);
		xcenter = 0.0;
		ycenter = 0.0;
		goto drawobject;
	case 'M':
		trans_func = rwf_juliaMandelbrotTransform;
		// Reset the scale
		jobject.setWindow(-2.0*1.333,-2.0,2.0*1.333,2.0);
		xcenter = 0.0;
		ycenter = 0.0;
		goto drawobject;
	default:
		goto getcommand;
	}
}

static void
draw_cursor(int xc, int yc)
{
	line(xc,0,xc, rwf_graphGetMaxY());
	line(0,yc,rwf_graphGetMaxX(),yc);
}


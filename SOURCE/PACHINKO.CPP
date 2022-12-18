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


#include "rwfaf.hpp"
#include "rwfgraph.hpp"
#include "rwfrand.hpp"

#include <string.h>
#include <conio.h>
#include <dos.h>



/*
**	Define RWFpach_Peg class as a set of circles
*/
class RWFpach_Peg:public RWFgraph_ObjectList {
private:
protected:
public:
	RWFpach_Peg(void) {
		(*this)[0] = new RWFgraph_Circle(0.0,0.0,1.0);
		(*this)[1] = new RWFgraph_Circle(0.0,0.0,0.75);
		(*this)[2] = new RWFgraph_Circle(0.0,0.0,0.50);
		(*this)[3] = new RWFgraph_Circle(0.0,0.0,0.25);

		(*this)[0]->setColor(CYAN);
		(*this)[1]->setColor(LIGHTGREEN);
		(*this)[2]->setColor(LIGHTBLUE);
		(*this)[3]->setColor(LIGHTRED);
	};
};

/*
**	Main program
*/
int main(int argc,char *argv[])
{
	int i,j,k,l,m,numrows=16,numballs=175;
	int ballnum,ballcount,maxactiveballs,numactiveballs,offsetactiveballs;
	int *ballstep,*xchangedir,*ballbin;
	int phase,numphases,midphasecount,curstep;
	int numsteps,numsteps1,numsteps2,numsteps3;
	int savecolor;
	RWFmisc_Boolean use_sound = RWFMISC_TRUE;
	long countssincenewball;
	float x,y,z,pegscale=.02,ballscale=.005;
	float xpegmin,xpegmax,ypegmin,ypegmax;
	float ybinmin,ybinmax;
	float *xphase,*yphase,*xprofile,*yprofile;
	float *ballx,*bally,*balldir,*binheight;
	RWFpach_Peg peg;
	RWFgraph_Circle ball(0.0, 0.0, 1.0);

/*
**	Read command line options
*/
	i = 1;
	while (i<argc) {
		if (strcmp("-balls",argv[i])==0) {
			i++;
			sscanf(argv[i],"%d",&numballs);
		}
		else if (strcmp("-rows",argv[i])==0) {
			i++;
			sscanf(argv[i],"%d",&numrows);
		}
		else if (strcmp("-nosound", argv[i])==0) {
			use_sound = RWFMISC_FALSE;
		}
		else if (strcmp("-?",argv[i])==0) {
			printf("Usage:  pachinko [-?] [-balls numberBalls]\n");
			printf("                 [-rows numberRows]\n");
			printf("    -balls   :  Set number of balls\n");
			printf("    -rows    :  Set number of rows\n");
			printf("    -nosound :  No sound while running\n");
			printf("    -?       :  Display usage information (i.e., this message)\n");
			return 0;
		}
		else {
			printf("***Unrecognized command-line argument \"%s\"***\n",argv[(int) i]);
			return 1;
		}
		i++;
	}

	rwf_miscTitle("Pachinko", 4);

/*
**	Initialize graphics display
*/
	rwf_graphInitialize(0);
	rwf_graphEraseScreen();
	rwf_randSrand(1010101L);

/*
**	Draw rows of pegs
*/
	xpegmin = -0.8;
	xpegmax = -xpegmin;
	ypegmin = -0.40;
	ypegmax =  0.70;

	for (j=1; j<=numrows; j++) {
		if (numrows==1) {
			x = (xpegmax+xpegmin)/2.0;
			y = (ypegmax+ypegmin)/2.0;
		}
		else {
			x = -(j-1.0)*(xpegmax-xpegmin)/(numrows-1.0)/2.0;
			y = (j-1.0)*(ypegmin-ypegmax)/(numrows-1.0) + ypegmax;
		}
		for (i=1; i<=j; i++) {
			peg.setIdentity();
			peg.scale(pegscale,pegscale);
			peg.translate(x,y);
			peg.fill();
			x += (xpegmax-xpegmin)/(numrows-1.0);
		}
	}

/*
**	Draw bin boundaries
*/
	ybinmin = -0.95;
	ybinmax = ypegmin - 0.10;
	setfillstyle(SOLID_FILL,MAGENTA);
	if (numrows==1) {
		x = (xpegmax+xpegmin)/2.0;
	}
	else {
		x = -(j-1.0)*(xpegmax-xpegmin)/(numrows-1.0)/2.0;
	}
	x -= (xpegmax-xpegmin)/(numrows-1.0)/2.0;
	for (i=0; i<=numrows+1; i++) {
		rwf_graphBarDraw(x-.005,ybinmin,x+.005,ybinmax);
		x += (xpegmax-xpegmin)/(numrows-1.0);
	}

/*
**	Calculate intermediate positions of ball
*/
	numphases = 7;
	midphasecount = 3;
	xphase = new float [numphases+1];
	yphase = new float [numphases+1];

	xphase[0] = 0.0;
	yphase[0] = (pegscale+ballscale);
	xphase[midphasecount] = (xpegmax-xpegmin)/(numrows-1.0)/2.0;
	yphase[midphasecount] = yphase[0] - 0.25*(ypegmin-ypegmax)/(numrows-1.0)/2.0;
	xphase[numphases] = (xpegmax-xpegmin)/(numrows-1.0)/2.0;
	yphase[numphases] = yphase[0] + (ypegmin-ypegmax)/(numrows-1.0);

	for (i=1; i<midphasecount; i++) {
		xphase[i] = i*(xphase[midphasecount]-xphase[0])/(midphasecount) +
			xphase[0];
		yphase[i] = i*(yphase[midphasecount]-yphase[0])/(midphasecount) +
			yphase[0];
	}
	for (i=midphasecount+1; i<numphases; i++) {
		xphase[i] = (i-midphasecount)*(xphase[numphases]-xphase[midphasecount])/
			(numphases-midphasecount) + xphase[midphasecount];
		yphase[i] = (i-midphasecount)*(yphase[numphases]-yphase[midphasecount])/
			(numphases-midphasecount) + yphase[midphasecount];
	}

	numsteps1 = RWFMISC_NINT((1.0-ypegmax)/
		(yphase[numphases-1]-yphase[numphases]));
	numsteps2 = numrows*numphases;
	numsteps3 = RWFMISC_NINT((ypegmin-(-1.0))/
		(yphase[numphases-1]-yphase[numphases]));
	numsteps = numsteps1 + numsteps2 + numsteps3;
	xprofile = new float [numsteps];
	yprofile = new float [numsteps];
	xchangedir = new int [numsteps];
	binheight = new float [numrows+1];
	for (i=0; i<=numrows; i++) {
		binheight[i] = ybinmin;
	}

/*
**	Calculate vertical profile used by every ball
*/
	k = numsteps1;
	y = ypegmax;
	for (j=0; j<numrows; j++) {
		for (i=0; i<numphases; i++) {
			yprofile[k] = y + yphase[i];
			if (i==0) {
				xchangedir[k] = 1;
				xprofile[k] = 0.0;
			}
			else {
				xchangedir[k] = 0;
				xprofile[k] = xphase[i] - xphase[i-1];
			}
			k++;
		}
		y += (ypegmin-ypegmax)/(numrows-1.0);
	}

	k = numsteps1 + numsteps2;
	y = yprofile[k-1];
	for (i=0; i<numsteps3; i++) {
		y += yphase[numphases]-yphase[numphases-1];
		xprofile[k] = 0.0;
		yprofile[k] = y;
		xchangedir[k] = 0;
		k++;
	}

	k = numsteps1;
	y = yprofile[k];
	for (i=0; i<numsteps1; i++) {
		k--;
		y -= yphase[numphases]-yphase[numphases-1];
		xprofile[k] = 0.0;
		yprofile[k] = y;
		xchangedir[k] = 0;
	}

/*
**	Set up storage for active balls
*/
	maxactiveballs = numrows + 1;
	ballx = new float [maxactiveballs];
	bally = new float [maxactiveballs];
	balldir = new float [maxactiveballs];
	ballstep = new int [maxactiveballs];
	ballbin = new int [maxactiveballs];
	numactiveballs = 0;
	offsetactiveballs = 0;
	countssincenewball = 0;
	int release_count = (numsteps / maxactiveballs) + 1;
	int doswap = 0;

/*
**	Simulate balls falling through array of pins
*/
	while (numballs>0) {
		l = numactiveballs;
		m = offsetactiveballs;
		for (ballcount=0; ballcount<l; ballcount++) {
			ballnum = (ballcount + m) % maxactiveballs;

/*
**			Erase last rendering of this ball
*/
			ball.setIdentity();
			ball.scale(ballscale,ballscale);
			ball.translate(ballx[ballnum],bally[ballnum]);
			savecolor = ball.getColor();
			ball.setColor(RWFGRAPH_COLOR_BLACK);
			ball.fill();
			ball.setColor(savecolor);

			ballstep[ballnum]++;
			curstep = ballstep[ballnum];
			if(curstep >= numsteps) {
				ball.fill();
				continue;
			}

			if ((curstep>0) && (xchangedir[curstep-1]==1)) {
				peg.setIdentity();
				peg.scale(pegscale,pegscale);
				peg.translate(ballx[ballnum],bally[ballnum]-yphase[0]);
				peg.setColor(255); // Forces all colors to white in an object list
				peg.fill();
				if(use_sound) {
					sound(100 + ballnum*100 + rwf_randRand()*50);
					delay(1);
				} else {
					delay(5);
				}
				peg.setColor(0);
				peg.fill();
			}
/*
**			Draw ball at new position
*/
			if (xchangedir[curstep]==1) {
				if (rwf_randRand()>=0.5) {
					balldir[ballnum] = 1.0;
					ballbin[ballnum]++;
				}
				else {
					balldir[ballnum] = -1.0;
					ballbin[ballnum]--;
				}
			}
			ballx[ballnum] += xprofile[curstep] * balldir[ballnum];

			doswap = 0;
			k = ballbin[ballnum];
			k += numrows;
			k /= 2;
			if (yprofile[curstep]<=(binheight[k]+ballscale)) {
				bally[ballnum] = binheight[k]+ballscale;
				binheight[k] += 2.0*ballscale;
				ballstep[ballnum] = numsteps;
				numballs--;
				numactiveballs--;
				// If this is the first in the list, then we can just
				// shift the list around
				if(ballcount == 0)
					offsetactiveballs = (ballnum+1) % maxactiveballs;
				else
					// Otherwise, we must actually swap this ball out
					doswap = 1;
			}
			else {
				bally[ballnum] = yprofile[curstep];
			}

			ball.setIdentity();
			ball.scale(ballscale,ballscale);
			ball.translate(ballx[ballnum],bally[ballnum]);
			ball.fill();

			if(doswap) {
				int lastnum = (offsetactiveballs + numactiveballs) % maxactiveballs;
				// Swap the two
				float temp;
				int   itemp;
				temp = ballx[ballnum];
				ballx[ballnum] = ballx[lastnum];
				ballx[lastnum] = temp;
				temp = bally[ballnum];
				bally[ballnum] = bally[lastnum];
				bally[lastnum] = temp;
				temp = balldir[ballnum];
				balldir[ballnum] = balldir[lastnum];
				balldir[lastnum] = temp;
				itemp = ballstep[ballnum];
				ballstep[ballnum] = ballstep[lastnum];
				ballstep[lastnum] = itemp;
				itemp = ballbin[ballnum];
				ballbin[ballnum] = ballbin[lastnum];
				ballbin[lastnum] = itemp;
			}

			rwf_miscCheckEscape();
		}

/*
**		See if a new ball should be dropped
*/
		countssincenewball++;
		if ((numactiveballs<maxactiveballs) && (numactiveballs<numballs) &&
			(countssincenewball>release_count)) {
			k = (offsetactiveballs+numactiveballs) % maxactiveballs;
			ballx[k] = 0.001;
			bally[k] = 1.5;
			balldir[k] = 1.0;
			ballstep[k] = -1;
			ballbin[k] = 0;
			numactiveballs++;
			countssincenewball = 0;
		}
	}

/*
**	Terminate program
*/
	nosound();
	rwf_miscPressAnyKey();
	rwf_graphClose();
	return 0;
}
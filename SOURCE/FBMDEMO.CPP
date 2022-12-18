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


#include "rwffbm.hpp"
#include "rwfgraph.hpp"

#include <string.h>
#include <conio.h>
#include <graphics.h>



int main(int argc,char *argv[])
{
	int i,j,k,l;
	float stdev = 0.5;
	float scale = 1.0 / sqrt(2.0);
	int nlevels = 8,npoints;
	float values[257];
	int intvalues[257];
	float corners[4] = {0.0,0.0,0.0,0.0};
	int intcorners[4] = {WHITE,RED,GREEN,BLUE};
	int nlevels2D = 5;
	int nvalues2D;
	float x,y,xinc;
	float **values2D;
	int **intvalues2D;
	float xmin,xmax,ymin,ymax;
	float (*random_func)(void);
	char outstring[100],*randfuncname,hstring[30];
	RWFmisc_Boolean use2D=RWFMISC_FALSE,usediscrete=RWFMISC_FALSE;
	RWFmisc_Boolean stdevset=RWFMISC_FALSE,scaleset=RWFMISC_FALSE;
	RWFgraph_Line seg;
	RWFvec_Vector spacevec(3),screenvec(3);
	RWFgraph_ViewingGeometry vg;

	rwf_miscTitle("fBMdemo",9);

	//	Define virtual window parameters
	random_func = &rwf_randGaussian;
	xmin = -1.25;
	xmax = 1.25;
	ymin = -1.0;
	ymax = 1.0;
	vg.setEyepoint(-2.0,-2.0,1.0);
	spacevec[0] = 1.0;
	spacevec[1] = 1.0;
	spacevec[2] = -0.5;
	vg.setEyeDirection(spacevec);

/*
**	If there are no command-line arguments,
**	execute default demonstartion sequence
*/
	if (argc==1) {
		rwf_graphInitialize(0);
		rwf_randSrand(1010101);

/*
**		Continuous 1-D demonstrations
*/
		npoints = 1 << nlevels;
		for (i=0; i<3; i++) {
			if (i==0) {
				random_func = rwf_randGaussian;
				randfuncname = "Gaussian";
			}
			else if (i==1) {
				random_func = rwf_randBiUniform;
				randfuncname = "Uniform";
			}
			else if (i==2) {
				random_func = rwf_randBiExponential;
				randfuncname = "Exponential";
			}
			for (j=0; j<4; j++) {
				if (j==0) scale  = 0.1;
				else if (j==1) scale = 0.25;
				else if (j==2) scale = 0.5;
				else if (j==3) scale = 0.75;

				rwf_fbm1DGenerate(0.0,0.0,random_func,stdev,
					scale,nlevels,values);

/*
**				Now plot the results
*/
				rwf_graphEraseScreen();
				x = xmin;
				xinc = (xmax - xmin) / (float) npoints;
				for (k=0; k<npoints; k++) {
					seg.setFirst(x,values[k]);
					seg.setEnd(x+xinc,values[k+1]);
					seg.draw();
					x += xinc;
				}
				strcpy(outstring,randfuncname);
				strcat(outstring," distribution, H = ");
				x = -log(scale)/log(2.0);
				sprintf(hstring,"%f",x);
				strcat(outstring,hstring);
				outtextxy(0,rwf_graphGetMaxY()-2*textheight("Press")-1,
					outstring);
				rwf_miscPressAnyKey();
			}
		}

/*
**		Continuous 2-D demonstrations
*/
		stdev = 0.25;
		nvalues2D = 1<< nlevels2D;
		values2D = rwf_miscNew(nvalues2D+1,float *);
		for (i=0; i<nvalues2D+1; i++) {
			values2D[i] = rwf_miscNew(nvalues2D+1,float);
		}
		for (i=0; i<3; i++) {
			if (i==0) {
				random_func = rwf_randGaussian;
				randfuncname = "Gaussian";
			}
			else if (i==1) {
				random_func = rwf_randBiUniform;
				randfuncname = "Uniform";
			}
			else if (i==2) {
				random_func = rwf_randBiExponential;
				randfuncname = "Exponential";
			}
			for (j=0; j<4; j++) {
				if (j==0) scale  = 0.05;
				else if (j==1) scale = 0.1;
				else if (j==2) scale = 0.25;
				else if (j==3) scale = 0.5;

				scale = sqrt(scale);
				rwf_fbm2DGenerate(corners,random_func,stdev,scale,nlevels2D,
					values2D);

/*
**				Now plot the results
*/
				rwf_graphEraseScreen();
				x = xmin;
				xinc = (xmax - xmin) / (float) npoints;
				for (k=0; k<nvalues2D+1; k++) {
					for (l=0; l<nvalues2D; l++) {
						spacevec[0] = 2.0*(k/(float) nvalues2D)-1.0;
						spacevec[1] = 2.0*(l/(float) nvalues2D)-1.0;
						spacevec[2] = values2D[k][l];
						screenvec = vg.getProjectedCoordinates(spacevec);
						seg.setFirst(screenvec[0],screenvec[1]);

						spacevec[0] = 2.0*(k/(float) nvalues2D)-1.0;
						spacevec[1] = 2.0*((l+1)/(float) nvalues2D)-1.0;
						spacevec[2] = values2D[k][l+1];
						screenvec = vg.getProjectedCoordinates(spacevec);
						seg.setEnd(screenvec[0],screenvec[1]);

						seg.draw();
					}
				}
				for (l=0; l<nvalues2D+1; l++) {
					for (k=0; k<nvalues2D; k++) {
						spacevec[0] = 2.0*(k/(float) nvalues2D)-1.0;
						spacevec[1] = 2.0*(l/(float) nvalues2D)-1.0;
						spacevec[2] = values2D[k][l];
						screenvec = vg.getProjectedCoordinates(spacevec);
						seg.setFirst(screenvec[0],screenvec[1]);

						spacevec[0] = 2.0*((k+1)/(float) nvalues2D)-1.0;
						spacevec[1] = 2.0*(l/(float) nvalues2D)-1.0;
						spacevec[2] = values2D[k+1][l];
						screenvec = vg.getProjectedCoordinates(spacevec);
						seg.setEnd(screenvec[0],screenvec[1]);

						seg.draw();
					}
				}

				strcpy(outstring,randfuncname);
				strcat(outstring," distribution, H = ");
				x = -log(scale)/log(2.0);
				sprintf(hstring,"%f",x);
				strcat(outstring,hstring);
				outtextxy(0,rwf_graphGetMaxY()-2*textheight("Press")-1,
					outstring);
				rwf_miscPressAnyKey();
			}
		}

/*
**		Discrete 2-D demonstration
*/
		intvalues2D = rwf_miscNew(nvalues2D+1,int *);
		for (i=0; i<nvalues2D+1; i++) {
			intvalues2D[i] = rwf_miscNew(nvalues2D+1,int);
		}

		rwf_fbm2DDiscreteGenerate(intcorners,rwf_randGaussian,nlevels2D,
			intvalues2D);

		rwf_graphEraseScreen();
		for (i=0; i<nvalues2D+1; i++) {
			for (j=0; j<nvalues2D+1; j++) {
				setfillstyle(SOLID_FILL,intvalues2D[i][j]);
				rwf_graphBarDraw(1.8*(i/(float) (nvalues2D+1))-0.9,
					1.8*(j/(float) (nvalues2D+1))-0.9,
					1.8*((i+1)/(float) (nvalues2D+1))-0.9,
					1.8*((j+1)/(float) (nvalues2D+1))-0.9);
			}
		}

		outtextxy(0,rwf_graphGetMaxY()-2*textheight("Press")-1,
			"Two-dimensional discrete fBM");
		rwf_miscPressAnyKey();

/*
**		Combination Continuous and Discrete 2-D demonstration
*/
		scale = 0.20;
		scale = sqrt(scale);
		rwf_fbm2DGenerate(corners,random_func,stdev,scale,nlevels2D,values2D);

		rwf_graphEraseScreen();
		for (i=0; i<nvalues2D+1; i++) {
			for (j=0; j<nvalues2D; j++) {
				spacevec[0] = 2.0*(i/(float) nvalues2D)-1.0;
				spacevec[1] = 2.0*(j/(float) nvalues2D)-1.0;
				spacevec[2] = values2D[i][j];
				screenvec = vg.getProjectedCoordinates(spacevec);
				seg.setFirst(screenvec[0],screenvec[1]);

				spacevec[0] = 2.0*(i/(float) nvalues2D)-1.0;
				spacevec[1] = 2.0*((j+1)/(float) nvalues2D)-1.0;
				spacevec[2] = values2D[i][j+1];
				screenvec = vg.getProjectedCoordinates(spacevec);
				seg.setEnd(screenvec[0],screenvec[1]);

				seg.setColor(intvalues2D[i][j]);
				seg.draw();
			}
		}
		for (j=0; j<nvalues2D+1; j++) {
			for (i=0; i<nvalues2D; i++) {
				spacevec[0] = 2.0*(i/(float) nvalues2D)-1.0;
				spacevec[1] = 2.0*(j/(float) nvalues2D)-1.0;
				spacevec[2] = values2D[i][j];
				screenvec = vg.getProjectedCoordinates(spacevec);
				seg.setFirst(screenvec[0],screenvec[1]);

				spacevec[0] = 2.0*((i+1)/(float) nvalues2D)-1.0;
				spacevec[1] = 2.0*(j/(float) nvalues2D)-1.0;
				spacevec[2] = values2D[i+1][j];
				screenvec = vg.getProjectedCoordinates(spacevec);
				seg.setEnd(screenvec[0],screenvec[1]);

				seg.setColor(intvalues2D[i][j]);
				seg.draw();
			}
		}

		outtextxy(0,rwf_graphGetMaxY()-2*textheight("Press")-1,
			"Two-dimensional continuous and discrete fBM");
		rwf_miscPressAnyKey();

/*
**		Return
*/
		rwf_graphClose();
		return 0;
	}

/*
**	Read command line options
*/
	i = 1;
	while (i<argc) {
		if (strcmp("-discrete",argv[i])==0) {
			usediscrete = RWFMISC_TRUE;
			nlevels2D = 7;
		}
		else if (strcmp("-exp",argv[i])==0) {
			random_func = rwf_randBiExponential;
		}
		else if (strcmp("-gauss",argv[i])==0) {
			random_func = rwf_randGaussian;
		}
		else if (strcmp("-h",argv[i])==0) {
			i++;
			sscanf(argv[i],"%f",&scale);
			scale = pow(2.0,(-scale));
			scaleset = RWFMISC_TRUE;
		}
		else if (strcmp("-stddev",argv[i])==0) {
			i++;
			sscanf(argv[i],"%f",&stdev);
			stdevset = RWFMISC_TRUE;
		}
		else if (strcmp("-uniform",argv[i])==0) {
			random_func = rwf_randBiUniform;
		}
		else if (strcmp("-2D",argv[i])==0) {
			use2D = RWFMISC_TRUE;
			if (stdevset==RWFMISC_FALSE) stdev = 0.25;
			if (scaleset==RWFMISC_FALSE) scale = 0.20;
		}
		else if (strcmp("-?",argv[i])==0) {
			printf("Usage:  fbmdemo [-?] [-stddev standardDeviation]\n");
			printf("                [-h scaleParameter] -[2D] -[discrete]\n");
			printf("                [-exp] [-gauss] [-uniform]\n");
			printf("    -discrete :  Use discrete fractional Brownian motion\n");
			printf("    -exp      :  Use exponential random number generator\n");
			printf("    -gauss    :  Use Gaussian random number generator\n");
			printf("    -h        :  Set scale parameter\n");
			printf("    -stddev   :  Set standard deviation\n");
			printf("    -uniform  :  Use uniform random number generator\n");
			printf("    -2D       :  Use two-dimensional grid\n");
			printf("    -?        :  Display usage information (i.e., this message)\n");
			return 0;
		}
		else {
			printf("***Unrecognized command-line argument \"%s\"\n",argv[(int) i]);
			return 1;
		}
		i++;
	}

	rwf_graphInitialize(0);
	rwf_randSrand(1010101);

	if (use2D==RWFMISC_FALSE) {
		if (usediscrete==RWFMISC_FALSE) {
			rwf_fbm1DGenerate(0.0,0.0,random_func,stdev,
				scale,nlevels,values);

			//	Now plot the results
			npoints = 1 << nlevels;

			x = xmin;
			xinc = (xmax - xmin) / (float) npoints;
			for (i=0; i<npoints; i++) {
				seg.setFirst(x,values[i]);
				seg.setEnd(x+xinc,values[i+1]);
				seg.draw();
				x += xinc;
			}
		}
		else {
			rwf_fbm1DDiscreteGenerate(0,1,random_func,nlevels,intvalues);

			//	Now plot the results
			npoints = 1 << nlevels;

			x = xmin;
			xinc = (xmax - xmin) / (float) npoints;
			for (i=0; i<npoints; i++) {
				seg.setFirst(x,intvalues[i]);
				seg.setEnd(x+xinc,intvalues[i+1]);
				seg.draw();
				x += xinc;
			}
		}

/*
**		Draw bounding box
*/
		setlinestyle(DOTTED_LINE,1,NORM_WIDTH);
		rwf_graphLineDraw(xmin,ymin,xmax,ymin);
		rwf_graphLineDraw(xmax,ymin,xmax,ymax);
		rwf_graphLineDraw(xmax,ymax,xmin,ymax);
		rwf_graphLineDraw(xmin,ymax,xmin,ymin);
	}
	else {
		if (usediscrete==RWFMISC_FALSE) {
			scale = sqrt(scale);
			nvalues2D = 1 << nlevels2D;
			values2D = rwf_miscNew(nvalues2D+1,float *);
			for (i=0; i<nvalues2D+1; i++) {
				values2D[i] = rwf_miscNew(nvalues2D+1,float);
			}

			rwf_fbm2DGenerate(corners,random_func,stdev,scale,nlevels2D,
				values2D);

			intvalues2D = rwf_miscNew(nvalues2D+1,int *);
			for (i=0; i<nvalues2D+1; i++) {
				intvalues2D[i] = rwf_miscNew(nvalues2D+1,int);
			}

			rwf_fbm2DDiscreteGenerate(intcorners,random_func,nlevels2D,
				intvalues2D);

			//	Now plot the results
			for (i=0; i<nvalues2D+1; i++) {
				for (j=0; j<nvalues2D; j++) {
					spacevec[0] = 2.0*(i/(float) nvalues2D)-1.0;
					spacevec[1] = 2.0*(j/(float) nvalues2D)-1.0;
					spacevec[2] = values2D[i][j];
					screenvec = vg.getProjectedCoordinates(spacevec);
					seg.setFirst(screenvec[0],screenvec[1]);

					spacevec[0] = 2.0*(i/(float) nvalues2D)-1.0;
					spacevec[1] = 2.0*((j+1)/(float) nvalues2D)-1.0;
					spacevec[2] = values2D[i][j+1];
					screenvec = vg.getProjectedCoordinates(spacevec);
					seg.setEnd(screenvec[0],screenvec[1]);

					seg.setColor(intvalues2D[i][j]);
					seg.draw();
				}
			}
			for (j=0; j<nvalues2D+1; j++) {
				for (i=0; i<nvalues2D; i++) {
					spacevec[0] = 2.0*(i/(float) nvalues2D)-1.0;
					spacevec[1] = 2.0*(j/(float) nvalues2D)-1.0;
					spacevec[2] = values2D[i][j];
					screenvec = vg.getProjectedCoordinates(spacevec);
					seg.setFirst(screenvec[0],screenvec[1]);

					spacevec[0] = 2.0*((i+1)/(float) nvalues2D)-1.0;
					spacevec[1] = 2.0*(j/(float) nvalues2D)-1.0;
					spacevec[2] = values2D[i+1][j];
					screenvec = vg.getProjectedCoordinates(spacevec);
					seg.setEnd(screenvec[0],screenvec[1]);

					seg.setColor(intvalues2D[i][j]);
					seg.draw();
				}
			}
		}
		else {
			nvalues2D = 1 << nlevels2D;
			intvalues2D = rwf_miscNew(nvalues2D+1,int *);
			for (i=0; i<nvalues2D+1; i++) {
				intvalues2D[i] = rwf_miscNew(nvalues2D+1,int);
			}

			rwf_fbm2DDiscreteGenerate(intcorners,random_func,nlevels2D,
				intvalues2D);

			for (i=0; i<nvalues2D+1; i++) {
				for (j=0; j<nvalues2D+1; j++) {
					setfillstyle(SOLID_FILL,intvalues2D[i][j]);
					rwf_graphBarDraw(2.0*(i/(float) (nvalues2D+1))-1.0,
						2.0*(j/(float) (nvalues2D+1))-1.0,
						2.0*((i+1)/(float) (nvalues2D+1))-1.0,
						2.0*((j+1)/(float) (nvalues2D+1))-1.0);
				}
			}

/*
**			Draw bounding box
*/
			rwf_graphLineDraw(-1.0,-1.0,1.0,-1.0);
			rwf_graphLineDraw(1.0,-1.0,1.0,1.0);
			rwf_graphLineDraw(1.0,1.0,-1.0,1.0);
			rwf_graphLineDraw(-1.0,1.0,-1.0,-1.0);
		}
	}

/*
**	Return
*/
	rwf_miscPressAnyKey();
	rwf_graphClose();
	return 0;
}
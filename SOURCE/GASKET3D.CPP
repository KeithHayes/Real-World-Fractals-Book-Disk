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


#include "rwfsg.hpp"
#include "rwfgraph.hpp"
#include "rwfrand.hpp"

#include <string.h>
#include <conio.h>
#include <dos.h>


// A special action function for reverse 3D objects
static void gasket3D_drawSpecial(RWFgraph_Object *object, int level,
																 RWFgraph_ViewingGeometry &vg);

static RWFgraph_Object *static_object;


int main(int argc,char *argv[])
{
	int i,numlevels,numvertices;
	float x,y,xmin,xmax,ymin,ymax,height,r,angle;
	RWFgraph_Palette pal(16), pal2(16);
	long seed = 98989L;

	RWFsg_3DGasket            gasket, igasket;
	RWFsg_MengerSponge        sponge, isponge;
	RWFsg_3DReverseGasket     rgasket, irgasket;
	RWFsg_ReverseMengerSponge rsponge, irsponge;

	RWFlist_ObjectPtrTable otable;
	RWFmisc_Boolean random_mode = RWFMISC_TRUE;
	RWFmisc_Boolean palette_cycle = RWFMISC_FALSE;

	RWFaf_3DObject *object = &gasket;
	RWFgraph_Cube cube;
	RWFgraph_Tetrahedron tetra;

	// Set color action functions
	rgasket.set3DAction(rwf_af3DDrawColorCycle);
	rsponge.set3DAction(rwf_af3DDrawColorCycle);

	otable[0] = &tetra;
	numlevels = 4;

/*
**	Read command line options, if any
*/
	i = 1;
	if(argc > 1)
		random_mode = RWFMISC_FALSE;

	while (i<argc) {
		if (strcmp("-?",argv[(int) i])==0) {
			printf("Usage:  gasket3d [-?]\n");
			printf("    -numlevels:  Set number of recursion levels\n");
			printf("    -cube     :  Use cube object\n");
			printf("    -tetra    :  Use tetrahedron object\n");
			printf("    -gasket   :  Draw a gasket\n");
			printf("    -sponge   :  Draw a sponge\n");
			printf("    -rgasket  :  Draw a reverse gasket\n");
			printf("    -rsponge  :  Draw a reverse sponge\n");
			printf("    -random   :  Run in random mode\n");
			printf("    -seed n   :  Set initial random number seed\n");
			printf("    -cycle    :  Cycle the color palette in random mode\n");
			printf("    -?        :  Display usage information (i.e., this message)\n");
			return 0;
		}
		else if (strcmp("-numlevels",argv[i])==0) {
			i++;
			sscanf(argv[i],"%d",&numlevels);
		}
		else if (strcmp("-cube",argv[i])==0) {
			otable[0] = &cube;
		}
		else if (strcmp("-tetra",argv[i])==0) {
			otable[0] = &tetra;
		}
		else if (strcmp("-gasket",argv[i])==0) {
			object = &gasket;
		}
		else if (strcmp("-sponge",argv[i])==0) {
			object = &sponge;
			otable[0] = &cube;
		}
		else if (strcmp("-rgasket",argv[i])==0) {
			object = &rgasket;
		}
		else if (strcmp("-rsponge",argv[i])==0) {
			object = &rsponge;
			otable[0] = &cube;
		}
		else if (strcmp("-random", argv[i])==0) {
			random_mode = RWFMISC_TRUE;
		}
		else if (strcmp("-seed", argv[i])==0) {
			i++;
			sscanf(argv[i],"%ld",&seed);
			random_mode = RWFMISC_TRUE;
		}
		else if (strcmp("-cycle", argv[i])==0) {
			palette_cycle = RWFMISC_TRUE;
		}
		else {
			printf("***Unrecognized command-line argument \"%s\"\n",argv[i]);
			return 1;
		}
		i++;
	}

	rwf_miscTitle("Gasket3D", 10);

/*
**	Initialize display
*/
	rwf_graphInitialize(0);
	x = 1.25 / 2.0;
	xmin = -1.333 * x;
	xmax = 1.333 * x;
	ymin = -1.0 * x;
	ymax = 1.0 * x;
	rwf_graphSetWindow(xmin,ymin,xmax,ymax);


	// Setup color cycling palette
	rwf_graphSetPaletteIndex(0,  50, 90, 110);
	rwf_graphSetPaletteIndex(14,220, 220, 50);
	rwf_graphSetPaletteIndex(8, 0, 255, 0);

	rwf_graphSetColorScale();
	rwf_graphGetPalette(0, pal);
	rwf_graphGetPalette(0, pal2);

	// Setup the viewing geometry

	RWFgraph_ViewingGeometry  vg;
	RWFvec_Vector light_vec(3);

	vg.setLightAttributes(0.5, 0.5);
	light_vec[0] = -1.0;
	light_vec[1] =  1.0;
	light_vec[2] =  1.0;
	vg.setLight(light_vec);
	vg.setLightAttributes(0.6, 0.4);
	vg.setEyepoint(-2.2, 0.0, -0.1);

	rwf_randSrand(seed);
	rsponge.set3DAction(gasket3D_drawSpecial);
	rgasket.set3DAction(gasket3D_drawSpecial);

/*
**	Draw the selected object
*/
BIGLOOP:

	if(random_mode) {
		int which_object = rwf_randRand() * 4;
		switch(which_object) {
			case 0: object = &gasket; break;
			case 1: object = &sponge; break;
			case 2: object = &rgasket; break;
			case 3: object = &rsponge; break;
			default: object = &gasket;
		}
		which_object = rwf_randRand() * 6;
		switch(which_object) {
			case 0: otable[0] = &tetra; break;
			case 1: otable[0] = &cube; break;
			case 2: otable[0] = &igasket; igasket.setMaximumLevel(2); break;
			case 3: otable[0] = &isponge; isponge.setMaximumLevel(2); break;
			case 4: otable[0] = &irgasket; irgasket.setMaximumLevel(2); break;
			case 5: otable[0] = &irsponge; irsponge.setMaximumLevel(2); break;
			default: otable[0] = &tetra;
		}
	}

	otable[0]->identity();
	object->setInitiator(otable);
	static_object = otable[0];

	int color = rwf_randRand() * 14 + 1;
	object->setColor(color);

	float zangle = rwf_randRand() * 360.0;
	float yangle = 35.0 + (rwf_randRand() - 0.5)*10.0;

	object->identity();
	object->rotate(0.0, 0.0, zangle);
	object->rotate(0.0, yangle, 0.0);

	int start_level;
	if(random_mode)
		start_level = numlevels;
	else
		start_level = 1;

	for(int level=start_level; level <= numlevels; level++) {
		object->setMaximumLevel(level);
		object->setMinimumSize(0.005);
		rwf_graphEraseScreen();
		rwf_miscCheckEscape();
		object->draw(vg);
		int index = 1;
		if(random_mode && palette_cycle && level == numlevels) {
			for(int loop=0; loop < 16*10; loop++) {
				for(i=1; i < 16; i++)
					pal2[i] = pal[(i+index)%15 + 1];
				rwf_graphSetPalette(0, pal2);
				rwf_graphSetColorScale();
				delay(100);
				index = (index+1) % 16;
			}
			rwf_graphSetPalette(0, pal);
			rwf_graphSetColorScale();
		} else if (random_mode) {
			delay(1000);
			rwf_miscCheckEscape();
		}	else {
			rwf_miscPressAnyKey();
		}
	}

	object->setMaximumLevel(start_level);
	if(random_mode)
		goto BIGLOOP;
/*
**	Return
*/
	rwf_miscPressAnyKey();
	rwf_graphClose();
	return 0;
}

// This 3D action function lets you assign a different object
// in place of the normal one that would be drawn for a reverse
// object.  This is simpler to use for the reverse Menger Sponge
// since you would have to reconstruct the erase initiator to draw
// the 5 separate objects

static void
gasket3D_drawSpecial(RWFgraph_Object *object, int level,
										 RWFgraph_ViewingGeometry &vg)
{
	if(!object)
		return;

	int savecolor = static_object->getColor();
	static_object->setColor((savecolor + level) % 15 + 1);
	static_object->setMatrix(object->getMatrix());
	static_object->draw(vg);
	static_object->setColor(savecolor);

	return;
}

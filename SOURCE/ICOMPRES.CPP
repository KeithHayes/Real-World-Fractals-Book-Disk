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

#include "rwfimage.hpp"

#include <stdio.h>
#include <conio.h>
#include <string.h>


/*
**	Demonstration of fractal image compression
*/

int main(int argc,char *argv[])
{
	int i;
	char filename[100] = "bobcat.pcx";
	char string[100];

	float max_error = 8.0;
	RWFmisc_Boolean use_quad  = RWFMISC_TRUE;
	int   min_size  = 8;

/*
**	Read command line options, if any
*/
	i = 1;
	while (i<argc) {
		if (strcmp("-?",argv[(int) i])==0) {
			printf("Usage:  icompres [-?] <filename.pcx>\n");
			printf("    -?        :  Display usage information (i.e., this message)\n");
			printf("    <filename>:  Name of PCX file to compress\n");
			printf("                 Must include the .pcx extension\n");
			printf("    -error e  :  Error threshold (should be 5 - 30)\n");
			printf("    -minsize s:  Minimum region size for quad encoding\n");
			printf("    -quad     :  Use quadtree encoding method\n");
			printf("    -8x8      :  Use 8x8 encoding method\n");
			return 0;
		} else if(!strcmp("-quad", argv[i])) {
			use_quad = RWFMISC_TRUE;
		} else if(!strcmp("-8x8", argv[i])) {
			use_quad = RWFMISC_FALSE;
		} else if(!strcmp("-error", argv[i])) {
			i++;
			sscanf(argv[i],"%f",&max_error);
		} else if(!strcmp("-minsize", argv[i])) {
			i++;
			sscanf(argv[i],"%d",&min_size);
		} else {
			strcpy(filename, argv[i]);
		}
		i++;
	}

	rwf_miscTitle("ICompres", 12);

	if(argc == 1) {
		printf("PCX file to compress ? [%s] : ",filename);
		gets(filename);
		if(strlen(filename) < 2)
			strcpy(filename, "bobcat.pcx");
	}

	RWFpcx_File *image_file = rwf_pcxOpen(filename);

	if(!image_file) {
		printf("Error opening file: %s !\n", filename);
		printf("Aborting program ...\n\n");
		return 1;
	}

	int nx, ny, xblock, yblock;
	image_file->getSize(nx, ny);

	printf("Encoding image file : %s\n", filename);
	printf("This image is %d columns by %d rows\n\n",nx,ny);
	printf("Error threshold = %f\n",max_error);
	printf("Minimum region size = %d\n", min_size);
	if(use_quad)
		printf("Using quad tree encoding method\n");
	else
		printf("Using 8x8 encoding method\n");

	printf("\nPress any key to start the encoding...\n");
	getch();

	rwf_graphInitialize(0);
	// Set up gray scale palette
	rwf_graphSetGrayScale(255);

	RWFgraph_PixelBlock pblock;
	RWFlist_RegionMapTable regions;

	// Do the image in 128 x 128 pieces

	int max_size = 128;
	xblock = RWFMISC_MIN(nx, max_size);
	yblock = RWFMISC_MIN(ny, max_size);

	// Make sure region table can hold enough regions
	regions.setMaximumSize((xblock/min_size) * (yblock/min_size) + 1);

	int ix, iy, xsize, ysize, total_regions = 0;

	// First, draw the original image
	for(iy=0; iy < ny; iy += yblock) {
		ysize = RWFMISC_MIN(yblock, ny - iy);
		for(ix=0; ix < nx; ix += xblock) {

			xsize = RWFMISC_MIN(xblock, nx - ix);
			pblock.setSize(xsize, ysize);

			// Draw the block we are processing
			image_file->readBlock(pblock, ix, iy);
			pblock.draw(ix, iy + yblock*2 + 50);
		}
	}

	// Now encode the image

	for(iy=0; iy < ny; iy += yblock) {
		ysize = RWFMISC_MIN(yblock, ny - iy);
		for(ix=0; ix < nx; ix += xblock) {
			// Clear the image
			setfillstyle(SOLID_FILL, BLACK);
			bar(0, 0, xblock*3 + 50, yblock*2);

			xsize = RWFMISC_MIN(xblock, nx - ix);
			pblock.setSize(xsize, ysize);

			// Draw the block we are processing
			image_file->readBlock(pblock, ix, iy);
			pblock.draw(0, 0);
			if(use_quad)
				rwf_imageEncodeQuadTree(pblock, regions, max_error, min_size);
			else
				rwf_imageEncode8x8(pblock, regions);

			// Draw the original image to the side
			pblock.draw(ix + nx + 8, iy + yblock*2 + 50);
			total_regions += regions.size();

			// Initialize pblock to a checker board
			for(int j=0; j < ysize; j++) {
				if( (j >> 1) % 2 == 0) {
					for(i=0; i < xsize; i++)
						pblock.setPixel(i, j, 192);
				} else {
					for(i=0; i < xsize; i++)
						pblock.setPixel(i, j, ((i >> 1)%2)*128 + 64);
				}
			}

			// Now decode the result
			sprintf(string," ");
			for(int pass=0; pass < 16; pass++) {
				rwf_miscCheckEscape();
				pblock.draw(ix, iy + yblock*2 + 50);
				setcolor(0);
				outtextxy(ix, yblock*2 + 20, string);
				sprintf(string,"%d regions found, decoding pass %d of 15",
								regions.size(), pass);
				setcolor(255);
				outtextxy(ix, yblock*2 + 20, string);
				rwf_imageDecodeRegionTable(regions, pblock);
			}
			setcolor(0);
			outtextxy(ix, yblock*2 + 20, string);
		}
	}

	setcolor(255);
	sprintf(string, "All Finished, Total number of regions was %d",total_regions);
	outtextxy(0, yblock*2 + 20,string);
	rwf_miscPressAnyKey();

	rwf_graphClose();
	return 0;
}
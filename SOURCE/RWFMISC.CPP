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

#include "rwfmisc.hpp"
#include "rwfgraph.hpp"

#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <dos.h>

void
rwf_miscCheckEscape(void)
{
	if(kbhit()) {
		getch();
		rwf_miscPressAnyKey();
	}
}

int
rwf_miscCheckBreak(void)
{
	if(kbhit()) {
		int key = getch();
		if(key == 3 || key == 27)
			return 1;
	}

	return 0;
}

static int use_sound = 0;

int
rwf_miscPressAnyKey(void)
{
	use_sound = 1;
	int key = rwf_miscPressAnyKey("Press any key to continue, ESC to exit...");
	use_sound = 0;
  return key;
}

int
rwf_miscPressAnyKey(char *string)
{
	return rwf_miscPressAnyKey(string, 1);
}

int
rwf_miscPressAnyKey(char *string, int allow_escape)
{
	int savecolor = getcolor();
	setcolor(255);
	outtextxy(0,rwf_graphGetMaxY()-textheight("XXX") - 1, string);
	if(use_sound) {
		sound(440);
		delay(100);
		nosound();
	}
	int key = getch();
	if(key == 3 || (key == 27 && allow_escape)) {
		rwf_graphEraseScreen();
		rwf_graphClose();
		printf("\nExiting program...\n");
		exit(1);
	}
	setcolor(0);
	outtextxy(0,rwf_graphGetMaxY()-textheight("Press") - 1, string);
	setcolor(savecolor);

	return key;
}

void
rwf_miscTitle(char *progname, int chapter)
{
	FILE *file;
	char fullname[20];

	clrscr();

	printf("\n");
	printf("  %s - Version 1.0, %s\n", progname, __DATE__);
	printf("             From chapter %d of REAL WORLD FRACTALS, M&T Books 1993\n", chapter);
	printf("             Copyright 1993 - Mark Finlay and Keith A. Blanton\n\n");

	strcpy(fullname, progname);
	strcat(fullname, ".txt");
	file = fopen(fullname, "r");
	if(file) {
		char string[100];
		while(fgets(string,100,file))
			printf("%s",string);
		fclose(file);
	}

	printf("\nPress any key to continue...");
	int key = getch();
	if(key == 3 || key == 27)
		exit(1);

	return;
}

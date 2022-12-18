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

#ifndef RWFMISC_HPP
#define RWFMISC_HPP 1

#include "stdlib.h"



#define RWFMISC_PI 3.141592654
#define RWFMISC_TWOPI (6.283185308)
#define RWFMISC_RADIANS_TO_DEGREES 57.29577951
#define RWFMISC_DEGREES_TO_RADIANS 0.01745329252

#define RWFMISC_MIN(A,B) (((A) > (B)) ? (B) : (A))
#define RWFMISC_MAX(A,B) (((A) < (B)) ? (B) : (A))
#define RWFMISC_NINT(A) ((int) ( ((A) >= 0.0) ? ((A) + 0.5) : ((A) - 0.5) ))
#define RWFMISC_NINTPOS(A) ((int) ((A) + 0.5) )

#define rwf_miscNew(COUNT,TYPE) (TYPE *) calloc((int) (COUNT),sizeof(TYPE))
#define rwf_miscFree(A) free((void *) A); (A) = NULL

#define cosD(angle)	(cos((angle)*RWFMISC_DEGREES_TO_RADIANS))
#define sinD(angle) (sin((angle)*RWFMISC_DEGREES_TO_RADIANS))
#define ABS(x)			( (x) > 0 ? (x) : (-(x)) )

typedef void RWFmisc_Opaque;
typedef enum {RWFMISC_FALSE,RWFMISC_TRUE} RWFmisc_Boolean;

void rwf_miscCheckEscape(void);
int  rwf_miscCheckBreak(void);
int rwf_miscPressAnyKey(void);
int rwf_miscPressAnyKey(char *);
int rwf_miscPressAnyKey(char *, int);
void rwf_miscTitle(char *, int);

#endif

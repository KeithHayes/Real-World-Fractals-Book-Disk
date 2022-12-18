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

#ifndef RWFSG_HPP
#define RWFSG_HPP 1

#include "rwfaf.hpp"

void rwf_sg3DDrawSpecial1(RWFgraph_Object *, int, RWFgraph_ViewingGeometry &);


class RWFsg_Gasket:public RWFaf_Object {
private:

protected:

public:
	RWFsg_Gasket(void);
	void draw(void) {if (!action_func) setAction(&rwf_afFill); process(); return;};
	void fill(void) {if (!action_func) setAction(&rwf_afFill); process(); return;};
};


class RWFsg_Carpet:public RWFaf_Object {
private:

protected:

public:
	RWFsg_Carpet(void);
	void draw(void) {if (!action_func) setAction(&rwf_afFill); process(); return;};
	void fill(void) {if (!action_func) setAction(&rwf_afFill); process(); return;};
};


class RWFsg_ReverseGasket:public RWFaf_Object {
private:

protected:

public:
	RWFsg_ReverseGasket(void);
	void draw(void) {if (!action_func) setAction(&rwf_afFill); process_reverse(); return;};
	void fill(void) {if (!action_func) setAction(&rwf_afFill); process_reverse(); return;};
	void fillColor(void) {if (!action_func) setAction(&rwf_afFillColorCycle); process_reverse(); return;};
};


class RWFsg_ReverseCarpet:public RWFaf_Object {
private:

protected:

public:
	RWFsg_ReverseCarpet(void);
	void draw(void) {if (!action_func) setAction(&rwf_afFill); process_reverse(); return;};
	void fill(void) {if (!action_func) setAction(&rwf_afFill); process_reverse(); return;};
	void fillColor(void) {if (!action_func) setAction(&rwf_afFillColorCycle); process_reverse(); return;};
};


class RWFsg_SpiralHexagon:public RWFaf_Object {
private:

protected:

public:
	RWFsg_SpiralHexagon(void);
	void draw(void) {if (!action_func) setAction(&rwf_afFill); process(); return;};
	void fill(void) {if (!action_func) setAction(&rwf_afFill); process(); return;};
};

class RWFsg_3DGasket:public RWFaf_3DObject {
private:

protected:

public:
	RWFsg_3DGasket(void);
};

class RWFsg_3DReverseGasket:public RWFaf_3DObject {
private:

protected:

public:
	RWFsg_3DReverseGasket(void);

	void draw(RWFgraph_ViewingGeometry &vg) {
		save_vg = &vg;
		if(!action_3Dfunc)
			set3DAction(&rwf_af3DDraw);
		process_reverse();
		return;
	}
};

class RWFsg_MengerSponge:public RWFaf_3DObject {
private:

protected:

public:
	RWFsg_MengerSponge(void);
};

class RWFsg_ReverseMengerSponge:public RWFaf_3DObject {
private:

protected:

public:
	RWFsg_ReverseMengerSponge(void);

	void draw(RWFgraph_ViewingGeometry &vg) {
		save_vg = &vg;
		if(!action_3Dfunc)
			set3DAction(&rwf_af3DDraw);
		process_reverse();
		return;
	}
};



#endif

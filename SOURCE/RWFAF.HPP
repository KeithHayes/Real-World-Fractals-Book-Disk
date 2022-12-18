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

#ifndef RWFAF_HPP
#define RWFAF_HPP 1

#include "rwfmat.hpp"
#include "rwfgraph.hpp"



#define RWFAF_DEFAULT_FRACTAL_LEVEL 5



void rwf_afSetLevel(int);
void rwf_afSetLastLevel(int);
int rwf_afGetLastLevel(void);
int rwf_afGetLevel(void);
void rwf_afDraw(RWFgraph_Object *,int);
void rwf_afFill(RWFgraph_Object *,int);
void rwf_afDrawColorCycle(RWFgraph_Object *,int);
void rwf_afFillColorCycle(RWFgraph_Object *,int);
void rwf_afDrawLastLevel(RWFgraph_Object *,int);
void rwf_af3DDraw(RWFgraph_Object *, int, RWFgraph_ViewingGeometry &);
void rwf_af3DDrawColorCycle(RWFgraph_Object *, int, RWFgraph_ViewingGeometry &);

typedef RWFlist_SimpleTable<void *>	RWFaf_UserDataTable;

class RWFaf_Object : public RWFgraph_Object {
protected:
	int max_level;
	float cur_object_size;
	float min_object_size;
	float *objectsizelist;
	RWFgraph_Object *base_object;
	RWFlist_ObjectPtrTable objectptrlist;
	RWFlist_ObjectPtrTable erase_object;
	RWFmat_MatrixTable transforms;
	RWFaf_UserDataTable user_data;

protected:
	void (*action_func)(RWFgraph_Object *,int);
	int  (*matrix_func) (void *data, int level, RWFmat_Matrix &gmatrix);

public:
	RWFaf_Object(void) {
		max_level = RWFAF_DEFAULT_FRACTAL_LEVEL;
		min_object_size = 0.01;
		cur_object_size = 0.0;
		objectsizelist = NULL;
		action_func = NULL;
		base_object = NULL;
		matrix_func    = NULL;
	};

	void setColor(int);
	void setEraseColor(int);
	void setInitiator(RWFgraph_Object *);
	void setInitiator(RWFlist_ObjectPtrTable &);
	void setInitiator(RWFgraph_Line &line_seg);
	void setInitiator(RWFgraph_LineTable &lt);
	void setInitiator(RWFgraph_Polyline &poly);
	void setEraseInitiator(RWFgraph_Object *);
	void setEraseInitiator(RWFlist_ObjectPtrTable &);
	void setGenerator(RWFmat_MatrixTable &matrixTableIn) {
		transforms = matrixTableIn;
		return;
	}

	void setAction(void (*actionFuncIn)(RWFgraph_Object *,int)) {
		action_func = actionFuncIn;
		return;
	}
	void process(void);
	void generate(RWFmat_Matrix &,int);
	void process_reverse(void);
	void generate_reverse(RWFmat_Matrix &,int);
	void draw(void) {
		if(!action_func)
			setAction(&rwf_afDraw);
		process();
		return;
	}
	void fill(void) {
		if(!action_func)
			setAction(&rwf_afFill);
		process();
		return;
	}
	void draw(RWFgraph_ViewingGeometry &vg) {draw();};
	void setMaximumLevel(int maxLevelIn) {
		max_level = maxLevelIn;
		return;
	}
	void setMinimumSize(float);
	void setMatrixFunction(
		int (*matrixfuncIn) (void *data, int level, RWFmat_Matrix &gmatrix))
		{matrix_func = matrixfuncIn;};
	void setGenerator(RWFaf_UserDataTable &user_dataIn)
		{user_data = user_dataIn;};
};

class RWFaf_3DObject : public RWFaf_Object {
	private:

	int numBase;

	protected:
	RWFgraph_ViewingGeometry *save_vg;
	void (*action_3Dfunc)(RWFgraph_Object *,int, RWFgraph_ViewingGeometry &);

	public:

	RWFaf_3DObject(void) : RWFaf_Object() {
		save_vg = NULL;
		action_3Dfunc = NULL;
		numBase = 0;
		setColor(0);
	};

	void process(void);
	void generate(RWFmat_Matrix &,int);
	void process_reverse(void);
	void generate_reverse(RWFmat_Matrix &,int);
	void setEraseInitiator(RWFlist_ObjectPtrTable &);
	void set3DAction(
			 void (*actionFuncIn)(RWFgraph_Object *,int,RWFgraph_ViewingGeometry &)) {
		action_3Dfunc = actionFuncIn;
		return;
	}
	void draw(RWFgraph_ViewingGeometry &vg) {
		save_vg = &vg;
		if(!action_3Dfunc)
			set3DAction(&rwf_af3DDraw);
		process();
		return;
	}
};

void rwf_matPolylineToMatrixTable(RWFgraph_Polyline &poly,
																	RWFmat_MatrixTable &mtable);

typedef RWFlist_SimpleTable<int>	RWFlist_SortingOrderTable;
typedef RWFlist_SimpleTable<float> RWFlist_RTable;

void rwf_graphSortMatrices(RWFgraph_ViewingGeometry &vg,
													 RWFmat_Matrix &global_matrix,
													 RWFvec_Vector &ref_point,
													 RWFmat_MatrixTable &mtable,
													 RWFlist_SortingOrderTable &order);

#endif

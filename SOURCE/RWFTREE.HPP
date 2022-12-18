
#ifndef RWFTREE_HPP
#define RWFTREE_HPP 1

#include "rwfaf.hpp"
#include "rwfrand.hpp"

class RWFaf_SimpleTree : public RWFaf_Object {
	private:
	protected:
	public:

	RWFaf_SimpleTree(void);
	// both draw() and fill() just draw lines
	// process_reverse() is used insce you want to draw
	// at every level

	void draw(void)
		{if(!action_func) setAction(&rwf_afDraw); process_reverse();};
	void fill(void)
		{if(!action_func) setAction(&rwf_afDraw); process_reverse();};
};

class RWFaf_PolyTree : public RWFaf_Object {
	private:
	protected:
	public:

	RWFaf_PolyTree(void);
	// both draw() and fill() just draw lines
	// process_reverse() is used insce you want to draw
	// at every level

	void draw(void)
		{if(!action_func) setAction(&rwf_afDraw); process_reverse();};
	void fill(void)
		{if(!action_func) setAction(&rwf_afFill); process_reverse();};
};

void rwf_treeLeafDraw(RWFgraph_Object *, int);
void rwf_treeLeafDraw2(RWFgraph_Object *, int);
void rwf_treeLeafDraw3(RWFgraph_Object *, int);
void rwf_tree3DLeafDraw(RWFgraph_Object *, int, RWFgraph_ViewingGeometry &);
void rwf_treeSetLeafColor(int);

class RWFtree_BranchData {
	public:
	int id;
	RWFrand_GaussianRV length, angle;
	RWFrand_UniformRV  offset, side;

	RWFtree_BranchData(void);
};

int rwf_treeFindBranchMatrix(void *data, int level, RWFmat_Matrix &bmatrix);


class RWFtree_Object : public RWFaf_Object {
	private:
	protected:
	public:

	RWFtree_Object(void);
	// process_reverse() is used since you want to draw
	// at every level

	void draw(void)
		{if(!action_func) setAction(&rwf_afDraw); process_reverse();};
	void fill(void)
		{if(!action_func) setAction(&rwf_afFill); process_reverse();};
};

class RWFtree_3DObject : public RWFaf_3DObject {
	private:
	protected:
	public:

	RWFtree_3DObject(void);

	void draw(RWFgraph_ViewingGeometry &vg)
		{if(!action_3Dfunc) set3DAction(&rwf_af3DDraw);
		 save_vg = &vg; process_reverse();};
};

int rwf_tree3DFindBranchMatrix(void *data, int level, RWFmat_Matrix &bmatrix);

// Crater classes

class RWFtree_CraterData {
	public:
	int id;
	// Probability of generating this crater
	RWFrand_UniformRV prob_occuring;
	RWFrand_GaussianRV distance;
	RWFrand_GaussianRV xscale, yscale;
	RWFrand_GaussianRV angle, orientation;

	RWFtree_CraterData(void);
};

void rwf_treeCraterDraw(RWFgraph_Object *, int);

class RWFtree_CraterObject : public RWFaf_Object {
	private:
	protected:
	public:

	RWFtree_CraterObject(void);
	// process_reverse() is used since you want to draw
	// at every level

	void draw(void)
		{if(!action_func) setAction(&rwf_treeCraterDraw); process_reverse();};
	void fill(void)
		{if(!action_func) setAction(&rwf_treeCraterDraw); process_reverse();};
};

class RWFtree_3DCraterData {
	public:
	int id;
	// Probability of generating this crater
	RWFrand_UniformRV prob_occuring;
	RWFrand_GaussianRV distance;
	RWFrand_UniformRV xscale, yscale, zscale;
	RWFrand_GaussianRV  xangle, yangle, zangle;

	RWFtree_3DCraterData(void);
};

class RWFtree_3DCraterObject : public RWFaf_3DObject {
	private:
	protected:
	public:

	RWFtree_3DCraterObject(void);
	// process() is used since you want to draw
	// only at the final level

	void draw(RWFgraph_ViewingGeometry &vg)
		{if(!action_3Dfunc) set3DAction(&rwf_af3DDraw);
		 save_vg = &vg; process();};
};

int rwf_treeFindCraterMatrix(void *data, int level, RWFmat_Matrix &bmatrix);
int rwf_treeFind3DCraterMatrix(void *data, int level, RWFmat_Matrix &bmatrix);


#endif




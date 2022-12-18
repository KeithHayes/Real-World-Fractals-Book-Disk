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

#ifndef RWFMAT_HPP
#define RWFMAT_HPP 1

#include "rwfvec.hpp"
#include "rwflist.hpp"
#include "rwfmisc.hpp"

#include <math.h>


template <class T>
class RWFmat_MatrixTemplate {
private:
	int numRows;
	int numColumns;
	RWFvec_VectorTemplate<T> *elements;

	void create(int, int);

protected:

public:
	RWFmat_MatrixTemplate(void) {
		numRows = 0;
		numColumns = 0;
		elements = NULL;
	};
	RWFmat_MatrixTemplate(int size) {create(size,size);};
	RWFmat_MatrixTemplate(int,int);
	RWFmat_MatrixTemplate(RWFmat_MatrixTemplate<T> &mSrc);
	~RWFmat_MatrixTemplate(void);
	RWFvec_VectorTemplate<T>& operator[](int row) {return elements[row];};
	int nrows(void) {return numRows;};
	int ncols(void) {return numColumns;};

	void resize(int new_ncols, int new_nrows);

	friend RWFmat_MatrixTemplate<T> operator * (RWFmat_MatrixTemplate<T> &,
		RWFmat_MatrixTemplate<T> &);
	friend RWFvec_VectorTemplate<T> operator * (RWFmat_MatrixTemplate<T> &,
		RWFvec_VectorTemplate<T> &);

	RWFmat_MatrixTemplate<T>& operator *= (RWFmat_MatrixTemplate<T> &m)
		{*this = m * (*this); return *this;};
	RWFmat_MatrixTemplate<T>& operator = (RWFmat_MatrixTemplate<T> &);

	float determinant(void);

	RWFmat_MatrixTemplate<T>& translate(RWFvec_VectorTemplate<T> &);
	RWFmat_MatrixTemplate<T>& scale(RWFvec_VectorTemplate<T> &);
	RWFmat_MatrixTemplate<T>& translate(float,float);
	RWFmat_MatrixTemplate<T>& scale(float,float);
	RWFmat_MatrixTemplate<T>& rotate(float);
	RWFmat_MatrixTemplate<T>& translate(float,float,float);
	RWFmat_MatrixTemplate<T>& scale(float,float,float);
	RWFmat_MatrixTemplate<T>& rotate(float,float,float);
	RWFmat_MatrixTemplate<T>& setIdentity(void);
	RWFmat_MatrixTemplate<T>& identity(void)	{return setIdentity();};
};


template <class T>
RWFmat_MatrixTemplate<T>::RWFmat_MatrixTemplate(int numRowsIn,
																								int numColumnsIn)
{
	create(numRowsIn, numColumnsIn);
};


template <class T>
void RWFmat_MatrixTemplate<T>::create(int numRowsIn, int numColumnsIn)
{
	numRows = numRowsIn;
	numColumns = numColumnsIn;
	elements = new RWFvec_VectorTemplate<T> [numRows+1];
	for(int i=0; i <= numRows; i++)
		elements[i].resize(numColumns);

	setIdentity();
}

template <class T>
void RWFmat_MatrixTemplate<T>::resize(int new_ncols, int new_nrows)
{
	if(new_ncols != numColumns || new_nrows != numRows) {
		RWFmat_MatrixTemplate::~RWFmat_MatrixTemplate();
		create(new_ncols, new_nrows);
  }
}

template <class T>
RWFmat_MatrixTemplate<T>::~RWFmat_MatrixTemplate(void)
{
	if (elements) delete [] elements;
	numRows = 0;
	numColumns = 0;
	elements = NULL;
};


template<class T>
RWFmat_MatrixTemplate<T>& RWFmat_MatrixTemplate<T>::setIdentity(void)
{
	if(numColumns == 0)
		return *this;

	for (int j=0; j<=numRows; j++) {
		for(int i=0; i <= numColumns; i++) {
			if(i == j)
				elements[j][i] = 1.0;
			else
				elements[j][i] = 0.0;
		}
	}

	return *this;
}

template<class T>
RWFmat_MatrixTemplate<T> operator * (RWFmat_MatrixTemplate<T>& mSrc1,
	RWFmat_MatrixTemplate<T>& mSrc2)
{
	int i,j,k;
	RWFmat_MatrixTemplate<T> mDst(mSrc1.nrows(),mSrc2.ncols());
	float x;


	for (i=0; i<=mSrc1.nrows(); i++){
		for (j=0; j<=mSrc2.ncols(); j++){
			x = 0.0;
			for (k=0; k<=mSrc1.ncols(); k++){
				x += mSrc1[i][k] * mSrc2[k][j];
			}
			mDst[i][j] = x;
		}
	}

	return mDst;

}


template <class T>
RWFmat_MatrixTemplate<T>&
RWFmat_MatrixTemplate<T>::operator = (RWFmat_MatrixTemplate<T>& mSrc)
{
	int i;

	if ((numRows!=mSrc.nrows()) || (numColumns!=mSrc.ncols())) {
		// Delete the current one
		RWFmat_MatrixTemplate::~RWFmat_MatrixTemplate();
	}
	if (elements==NULL) {
		// Create a new one of the proper size
		create(mSrc.ncols(), mSrc.nrows());
	}

	for (i=0; i<numRows + 1; i++) {
		elements[i] = mSrc[i];
	}

	return *this;
};

template <class T>
RWFmat_MatrixTemplate<T>::RWFmat_MatrixTemplate(RWFmat_MatrixTemplate<T>& mSrc)
{
	numRows = mSrc.nrows();
	numColumns = mSrc.ncols();
	elements = new RWFvec_VectorTemplate<T> [numRows+1];

	for (int i=0; i<numRows + 1; i++) {
		elements[i] = mSrc[i];
	}

};


template <class T>
RWFmat_MatrixTemplate<T>&
RWFmat_MatrixTemplate<T>::translate(RWFvec_VectorTemplate<T> &v)
{
	if(v.size() == 0 || nrows() != v.size())
		return *this;

	if (!elements){
		create(v.size(), v.size());
	}

	for(int i=0; i < v.size(); i++)
		elements[i][numColumns] += v[i];

	return *this;
};


template <class T>
RWFmat_MatrixTemplate<T>&
RWFmat_MatrixTemplate<T>::translate(float xoff,float yoff)
{
	if (!elements){
		create(2, 2);
	}

	if (numRows!=2 || numColumns!=2)
		return *this;

	elements[0][2] += xoff;
	elements[1][2] += yoff;
	return *this;
};


template <class T>
RWFmat_MatrixTemplate<T>&
RWFmat_MatrixTemplate<T>::scale(RWFvec_VectorTemplate<T> &v)
{
	if(v.size() == 0 || ncols() != v.size())
		return *this;

	if (!elements){
		create(v.size(), v.size());
	}

	for(int i=0; i < v.size(); i++) {
		elements[i] *= v[i];
		elements[i][numColumns] *= v[i];
	}

	return *this;
};


template <class T>
RWFmat_MatrixTemplate<T>&
RWFmat_MatrixTemplate<T>::scale(float xscale,float yscale)
{
	if (!elements){
		create(2, 2);
	}

	if (numRows!=2 || numColumns!=2) return *this;

	elements[0] *= xscale;
	elements[0][numColumns] *= xscale;
	elements[1] *= yscale;
	elements[1][numColumns] *= yscale;

	return *this;
};


template <class T>
RWFmat_MatrixTemplate<T>&
RWFmat_MatrixTemplate<T>::rotate(float angle)
{
	float cosangle,sinangle;
	static RWFmat_MatrixTemplate<T> rotmat(2,2);

	if(ncols() != 2)
		return *this;

	if (!elements){
		create(2, 2);
	}

	if (numRows!=2 || numColumns!=2) return *this;

	cosangle = cos(angle*RWFMISC_DEGREES_TO_RADIANS);
	sinangle = sin(angle*RWFMISC_DEGREES_TO_RADIANS);
	rotmat[0][0] =  cosangle;
	rotmat[1][0] =  sinangle;
	rotmat[0][1] = -sinangle;
	rotmat[1][1] =  cosangle;

	*this = rotmat * *this;

	return *this;

};


template <class T>
RWFmat_MatrixTemplate<T>&
RWFmat_MatrixTemplate<T>::translate(float xoff,float yoff, float zoff)
{
	if (!elements){
		create(3, 3);
	}

	if (numRows!=3 || numColumns!=3)
		return *this;

	elements[0][3] += xoff;
	elements[1][3] += yoff;
	elements[2][3] += zoff;

	return *this;
};


template <class T>
RWFmat_MatrixTemplate<T>&
RWFmat_MatrixTemplate<T>::scale(float xscale,float yscale,float zscale)
{
	if (!elements){
		create(3, 3);
	}

	if (numRows!=3 || numColumns!=3)
		return *this;

	elements[0] *= xscale;
	elements[0][numColumns] *= xscale;
	elements[1] *= yscale;
	elements[1][numColumns] *= yscale;
	elements[2] *= zscale;
	elements[2][numColumns] *= zscale;

	return *this;
};


template <class T>
RWFmat_MatrixTemplate<T>&
RWFmat_MatrixTemplate<T>::rotate(float xangle, float yangle, float zangle)
{
	float cosangle,sinangle;
	static RWFmat_MatrixTemplate<T> rotmat(3,3);

	if(ncols() != 3)
		return *this;

	if (!elements){
		create(3, 3);
	}

	if (numRows!=3 || numColumns!=3)
		return *this;

	if(zangle != 0.0) {
		rotmat.identity();
		cosangle = cos(zangle*RWFMISC_DEGREES_TO_RADIANS);
		sinangle = sin(zangle*RWFMISC_DEGREES_TO_RADIANS);
		rotmat[0][0] =  cosangle;
		rotmat[1][0] =  sinangle;
		rotmat[0][1] = -sinangle;
		rotmat[1][1] =  cosangle;
		*this = rotmat * *this;
	}

	if(yangle != 0.0) {
		rotmat.identity();
		cosangle = cos(yangle*RWFMISC_DEGREES_TO_RADIANS);
		sinangle = sin(yangle*RWFMISC_DEGREES_TO_RADIANS);
		rotmat[0][0] =  cosangle;
		rotmat[2][0] =  sinangle;
		rotmat[0][2] = -sinangle;
		rotmat[2][2] =  cosangle;
		*this = rotmat * *this;
	}

	if(xangle != 0.0) {
		rotmat.identity();
		cosangle = cos(xangle*RWFMISC_DEGREES_TO_RADIANS);
		sinangle = sin(xangle*RWFMISC_DEGREES_TO_RADIANS);
		rotmat[1][1] =  cosangle;
		rotmat[2][1] =  sinangle;
		rotmat[1][2] = -sinangle;
		rotmat[2][2] =  cosangle;
		*this = rotmat * *this;
	}

	return *this;
};


// Multiply operators

template<class T>
RWFvec_VectorTemplate<T> operator * (RWFmat_MatrixTemplate<T> &m,
																		 RWFvec_VectorTemplate<T> &v)
{
	RWFvec_VectorTemplate<T> vout(v.size());

	if(m.ncols() != v.size())
		return vout;

	int n = v.size();
	for(int i=0; i < n; i++) {
		vout[i] = m.elements[i] * v;
		vout[i] += m.elements[i][n];
	}

	return vout;
}

template<class T>
RWFvec_VectorTemplate<T> &
operator *= (RWFvec_VectorTemplate<T> &v, RWFmat_MatrixTemplate<T> &m)
{
	v = m*v;
	return v;
}

typedef RWFmat_MatrixTemplate<float> RWFmat_Matrix;
typedef RWFlist_SimpleTable<RWFmat_Matrix> RWFmat_MatrixTable;

// Utility functions

float rwf_matGetGlobalScalingFactor(RWFmat_Matrix &);

#endif

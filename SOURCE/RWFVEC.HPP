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

#ifndef RWFVEC_HPP
#define RWFVEC_HPP 1

#include <stdlib.h>



template <class T>
class RWFvec_VectorTemplate {
private:
	int numElements;
	T *elements;

protected:

public:
	RWFvec_VectorTemplate(void);
	RWFvec_VectorTemplate(int);
	RWFvec_VectorTemplate(RWFvec_VectorTemplate<T> &vSrc);
	~RWFvec_VectorTemplate(void);
	T & operator[](unsigned int index) {return elements[index];};
	int size(void) {return numElements;};
	void resize(int sizeIn);
	RWFvec_VectorTemplate<T>& operator = (RWFvec_VectorTemplate<T> &);
	RWFvec_VectorTemplate<T>& operator = (T);

	float length(void);
	RWFvec_VectorTemplate<T> &normalize(void);

	friend T operator * (RWFvec_VectorTemplate<T> &v1, RWFvec_VectorTemplate<T> &v2);
	friend RWFvec_VectorTemplate<T> operator + (RWFvec_VectorTemplate<T> &v1, RWFvec_VectorTemplate<T> &v2);
	friend RWFvec_VectorTemplate<T> operator - (RWFvec_VectorTemplate<T> &v1, RWFvec_VectorTemplate<T> &v2);
	friend RWFvec_VectorTemplate<T> operator * (RWFvec_VectorTemplate<T> &v1, T scalar);
	friend RWFvec_VectorTemplate<T> operator * (T scalar, RWFvec_VectorTemplate<T> &v1);
	RWFvec_VectorTemplate<T> & operator += (RWFvec_VectorTemplate<T> &v);
	RWFvec_VectorTemplate<T> & operator *= (T scalar);
	RWFvec_VectorTemplate<T> & operator -= (RWFvec_VectorTemplate<T> &v);
	RWFvec_VectorTemplate<T> cross(RWFvec_VectorTemplate<T> &v);
}


template <class T>
RWFvec_VectorTemplate<T>::RWFvec_VectorTemplate(void)
{
	numElements = 0;
	elements = NULL;
};


template <class T>
RWFvec_VectorTemplate<T>::RWFvec_VectorTemplate(int numElementsIn)
{
	int i;
	numElements = numElementsIn;
	elements = new T[numElements+1];
	for (i=0; i<numElements; i++) {
		elements[i] = 0.0;
	}
	elements[numElements] = 1.0;
};

template <class T>
RWFvec_VectorTemplate<T>::~RWFvec_VectorTemplate(void)
{
	if(elements)
		delete elements;
};

template <class T>
void RWFvec_VectorTemplate<T>::resize(int sizeIn)
{
	if(sizeIn > numElements) {
		if(elements)
			delete elements;
		elements = new T [sizeIn+1];
	}
	numElements = sizeIn;
	for(int i=0; i < numElements; i++)
		elements[i] = 0.0;
	elements[numElements] = 1.0;
}


template <class T>
RWFvec_VectorTemplate<T>& RWFvec_VectorTemplate<T>::operator = (RWFvec_VectorTemplate<T>& vSrc)
{
	int i;

	if (numElements!=vSrc.size()) {
		if (elements) delete[] (T *) elements;
		numElements = 0;
		elements = NULL;
	}
	if (elements==NULL) {
		numElements = vSrc.size();
		elements = new T[numElements+1];
	}

	for (i=0; i<numElements+1; i++) {
		elements[i] = vSrc[i];
	}

	return *this;
};

template <class T>
RWFvec_VectorTemplate<T>::RWFvec_VectorTemplate(RWFvec_VectorTemplate<T>& vSrc)
{
	numElements = vSrc.size();
	if(numElements > 0) {
		elements = new T[numElements+1];

		for (int i=0; i<numElements+1; i++) {
			elements[i] = vSrc[i];
		}
	} else {
		elements = 0;
	}
};


template <class T>
RWFvec_VectorTemplate<T>& RWFvec_VectorTemplate<T>::operator = (T value)
{
	int i;

	for (i=0; i<numElements; i++) {
		elements[i] = value;
	}

	return *this;
};


template<class T>
T operator * (RWFvec_VectorTemplate<T> &v1, RWFvec_VectorTemplate<T> &v2)
{
	T sum = 0;

	if(v1.size() != v2.size())
		return sum;

	for(int i=0; i < v1.size(); i++)
		sum += v1.elements[i]*v2.elements[i];

	return sum;
}

template<class T>
RWFvec_VectorTemplate<T>
operator + (RWFvec_VectorTemplate<T> &v1, RWFvec_VectorTemplate<T> &v2)
{
	RWFvec_VectorTemplate<T> vout(v1.size());

	if(v1.size() != v2.size())
		return vout;

	for(int i=0; i < v1.size(); i++)
		vout.elements[i] = v1.elements[i]+v2.elements[i];

	return vout;
}

template<class T>
RWFvec_VectorTemplate<T>
operator - (RWFvec_VectorTemplate<T> &v1, RWFvec_VectorTemplate<T> &v2)
{
	RWFvec_VectorTemplate<T> vout(v1.size());

	if(v1.size() != v2.size())
		return vout;

	for(int i=0; i < v1.size(); i++)
		vout.elements[i] = v1.elements[i]-v2.elements[i];

	return vout;
}

template<class T>
RWFvec_VectorTemplate<T>
operator * (RWFvec_VectorTemplate<T> &v1, T scalar)
{
	RWFvec_VectorTemplate<T> vout(v1.size());

	for(int i=0; i < v1.size(); i++)
		vout.elements[i] = v1.elements[i]*scalar;

	return vout;
}

template<class T>
RWFvec_VectorTemplate<T>
operator * (T scalar, RWFvec_VectorTemplate<T> &v1)
{
	RWFvec_VectorTemplate<T> vout(v1.size());

	for(int i=0; i < v1.size(); i++)
		vout.elements[i] = scalar*v1.elements[i];

	return vout;
}

template<class T>
RWFvec_VectorTemplate<T> &
RWFvec_VectorTemplate<T>::operator += (RWFvec_VectorTemplate<T> &v)
{
	if(v.size() != size())
		return *this;

	for(int i=0; i < size(); i++)
		elements[i] += v.elements[i];

	return *this;
}

template<class T>
RWFvec_VectorTemplate<T> &
RWFvec_VectorTemplate<T>::operator -= (RWFvec_VectorTemplate<T> &v)
{
	if(v.size() != size())
		return *this;

	for(int i=0; i < size(); i++)
		elements[i] -= v.elements[i];

	return *this;
}

template<class T>
RWFvec_VectorTemplate<T> &
RWFvec_VectorTemplate<T>::operator *= (T scalar)
{
	for(int i=0; i < size(); i++)
		elements[i] *= scalar;

	return *this;
}

template<class T>
RWFvec_VectorTemplate<T>
RWFvec_VectorTemplate<T>::cross(RWFvec_VectorTemplate<T> &v)
{
	RWFvec_VectorTemplate<T> vout(v.size());

	if(size() != 3 || v.size() != 3)
		return vout;

	vout.elements[0] = elements[1]*v.elements[2] - elements[2]*v.elements[1];
	vout.elements[1] = elements[2]*v.elements[0] - elements[0]*v.elements[2];
	vout.elements[2] = elements[0]*v.elements[1] - elements[1]*v.elements[0];

	return vout;
}

template<class T>
float RWFvec_VectorTemplate<T>::length(void)
{
	float sum=0.0;

	for(int i=0; i < size(); i++)
		sum += elements[i]*elements[i];

	return sqrt(sum);
}

template<class T>
RWFvec_VectorTemplate<T> &
RWFvec_VectorTemplate<T>::normalize(void)
{
	float l = length();

	if(l > 0.0) {
		for(int i=0; i < size(); i++)
			elements[i] /= l;
	}

	return *this;
}

// Define default vector type
typedef RWFvec_VectorTemplate<float> RWFvec_Vector;



#endif

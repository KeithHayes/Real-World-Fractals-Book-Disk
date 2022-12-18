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

#ifndef RWFLIST_HPP
#define RWFLIST_HPP 1

#include "rwfmisc.hpp"
#include <stdio.h>

#define RWFLIST_ALLOC_INC	5

template <class T>
class RWFlist_SimpleTable {
private:
	int curSize, allocated;
	int maxSize;
	T **items;

	void resize(int);

protected:

public:
	RWFlist_SimpleTable(void);
	RWFlist_SimpleTable(int initial);
	~RWFlist_SimpleTable(void);
	T& operator[](int);
	RWFlist_SimpleTable<T>& operator = (RWFlist_SimpleTable<T> &);
	int size(void) {return curSize;};
	void setMaximumSize(int);
	int getMaximumSize(void) {return maxSize;};
	void reset(void);
	void remove(int);
	void swap(int,int);
};


template <class T>
RWFlist_SimpleTable<T>::RWFlist_SimpleTable(void)
{
	curSize = 0;
	allocated = 0;
	maxSize = 100;
	items = NULL;
};

template <class T>
RWFlist_SimpleTable<T>::RWFlist_SimpleTable(int initial)
{
	curSize = 0;
	allocated = 0;
	maxSize = initial + 100;
	items = NULL;
	(*this)[initial-1];
}

template <class T>
RWFlist_SimpleTable<T>::~RWFlist_SimpleTable(void)
{

	for(int i=0; i < allocated; i++)
		delete items[i];

	if(items)
		delete items;
};


template <class T>
T& RWFlist_SimpleTable<T>::operator[](int index)
{
	if (index>=maxSize) {
		printf("***Requested table index = %d, table size = %d***",
			index,maxSize);
		exit(1);
	}
	resize(index + 1);
	curSize = RWFMISC_MAX(curSize, index + 1);
	return *(items[index]);
};


template <class T>
RWFlist_SimpleTable<T>& RWFlist_SimpleTable<T>::operator = (RWFlist_SimpleTable<T>& lSrc)
{
	int i;

	resize(lSrc.size());
	curSize = lSrc.size();
	maxSize = lSrc.getMaximumSize();

	for (i=0; i<curSize; i++) {
		*(items[i]) = lSrc[i];
	}

	return *this;
};


template <class T>
void RWFlist_SimpleTable<T>::resize(int new_size)
{
	if(new_size > allocated) {
		int old_size = allocated;
		allocated = RWFMISC_MAX(allocated + RWFLIST_ALLOC_INC, new_size);
		T **new_items = new T * [allocated];
		for(int i=0; i < old_size; i++)
			new_items[i] = items[i];
		for(i=old_size; i < allocated; i++)
			new_items[i] = new T;
		if(items)
			delete items;
		items = new_items;
	}

	return;
}



template <class T>
void RWFlist_SimpleTable<T>::setMaximumSize(int newSize)
{
	maxSize = newSize;
};


template <class T>
void RWFlist_SimpleTable<T>::reset(void)
{
	for (int i=0; i<allocated; i++) {
		delete items[i];
	}
	curSize = 0;
	allocated = 0;
	items = NULL;
};


template <class T>
void RWFlist_SimpleTable<T>::remove(int index)
{
	if(index < allocated)
		delete items[index];
	swap(index, allocated-1);
	allocated--;
	curSize--;

	if(curSize == 0)
		reset();

	return;
};


template <class T>
void RWFlist_SimpleTable<T>::swap(int index1,int index2)
{
	T *tempitem;

	if (index1>=maxSize) {
		printf("***Requested table index = %d, table size = %d***",
			index1,maxSize);
		exit(1);
	}
	if (index2>=maxSize) {
		printf("***Requested table index = %d, table size = %d***",
			index2,maxSize);
		exit(1);
	}

	tempitem = items[index1];
	items[index1] = items[index2];
	items[index2] = tempitem;

	curSize = RWFMISC_MAX(index1+1,curSize);
	curSize = RWFMISC_MAX(index2+1,curSize);
	return;
};



#endif

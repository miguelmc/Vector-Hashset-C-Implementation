#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <search.h> 

void grow(vector *v)
{
	if(v->totLength == 0)
	{
		v->totLength = 1;
	}

	v->totLength = v->totLength*2;
	v->elems = realloc(v->elems, v->totLength*v->elemSize);
}

void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation)
{
	v->elemSize = elemSize;
	v->totLength = initialAllocation;
	v->length = 0; 
	v->freeFn = freeFn;
	v->elems = malloc(initialAllocation * elemSize);
}

void VectorDispose(vector *v)
{
	if(v->freeFn != NULL)
	{
		for(int i=0; i< v->length; i++)
		{
			v->freeFn((char*)(v->elems) + i * v->elemSize);
		}
	}
	free(v->elems);
}

int VectorLength(const vector *v)
{ return v->length; }

void *VectorNth(const vector *v, int position)
{
	assert(position >= 0 && position < v->length);
	return ((char*)(v->elems) + position * v->elemSize);
}

void VectorReplace(vector *v, const void *elemAddr, int position)
{
	assert(position >= 0 && position < v->length);
	char* pos = ((char*)(v->elems) + position * v->elemSize);
	if(v->freeFn != NULL)
		v->freeFn(pos);
	memcpy(pos, elemAddr, v->elemSize);
}

void VectorInsert(vector *v, const void *elemAddr, int position)
{
	if(v->length == v->totLength)
		grow(v);	

	assert(position >= 0 || position < v->length);
	char * pos = (char*)v->elems + position*v->elemSize;
	int bufferSize = v->length - position;
	char buffer[bufferSize];
	memcpy(buffer, pos, bufferSize*v->elemSize);
	memcpy(pos, elemAddr, v->elemSize);
	memcpy(pos+v->elemSize, buffer, bufferSize*v->elemSize);
	v->length++;
}
void VectorAppend(vector *v, const void *elemAddr)
{
	if(v->length == v->totLength)
		grow(v);	

	char* pos = (char*)v->elems + v->length*v->elemSize;
	memcpy(pos, elemAddr, v->elemSize);
	v->length++;
}

void VectorDelete(vector *v, int position)
{
	assert(position >= 0 && position < v->length);
	char* target = (char*)v->elems + (position)*v->elemSize;
	if(v->freeFn != NULL)
		v->freeFn(target);
	//lugar en memoria despues del valor que quieres borrar
 	char* pos = target + v->elemSize;
	int bufferSize = v->length - position - 1;
	memmove(target, pos, bufferSize*v->elemSize);
	v->length--;
	if(v->freeFn != NULL)
		v->freeFn((char*)v->elems + v->length*v->elemSize);
}

void VectorSort(vector *v, VectorCompareFunction compare)
{
	assert(compare != NULL);
	qsort(v->elems, v->length, v->elemSize, compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData)
{
	assert(mapFn != NULL);
	for(int i=0; i< v->length; i++)
		mapFn((char*)v->elems + i*v->elemSize, auxData);
}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted)
{ 
	assert(startIndex >= 0 && startIndex <= v->length);
	assert(key != NULL && searchFn != NULL);
	char * loc;
	size_t nElems = v->length - startIndex; 
	if(isSorted == true){
		loc = bsearch(key, (char*)v->elems + startIndex*v->elemSize, nElems, v->elemSize, searchFn);
	}
	else{
		loc = lfind(key, (char*)v->elems + startIndex*v->elemSize, &nElems, v->elemSize, searchFn);
	}
	if(loc == NULL) 
		return -1;
	return loc- (char*)v->elems; 
} 





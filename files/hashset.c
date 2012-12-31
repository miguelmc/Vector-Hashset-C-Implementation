#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn)
{
	assert(elemSize > 0 && numBuckets > 0 && hashfn != NULL && comparefn != NULL);

	h->elemSize = elemSize;
	h->numBuckets = numBuckets;
	h->nElems = 0;
	h->buckets = malloc(sizeof(vector*) * numBuckets);
	for(int i=0; i<numBuckets; i++)
	{
		void * v = (char*)h->buckets + i*sizeof(vector*);
		*(vector**)v = malloc(sizeof(vector));
		VectorNew(*(vector**)v, elemSize, freefn, 1);
	}
	h->hashfn = hashfn;
	h->comparefn = comparefn;
	h->freefn = freefn;
}

void HashSetDispose(hashset *h)
{
	if(h->freefn != NULL);

	for(int i=0; i< h->numBuckets; i++)
	{
		vector* bucket = *(vector**)((char*)h->buckets + i*sizeof(vector*));
		VectorDispose(bucket);
	}

	free(h->buckets);	
}

int HashSetCount(const hashset *h)
{
	return h->nElems;
}

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData)
{
	assert(mapfn != NULL);

	for(int i=0; i< h->numBuckets; i++)
	{
		void* curr = (char*)h->buckets + i*sizeof(vector*);

		for(int j=0; j< VectorLength(*(vector**)curr); j++)
		{
			mapfn(VectorNth(*(vector**)curr, j), auxData);
		}
	}
}

void HashSetEnter(hashset *h, const void *elemAddr)
{
	
	assert(elemAddr != NULL);
	int hashCode = h->hashfn(elemAddr, h->numBuckets);
	assert(hashCode >= 0 && hashCode < h->numBuckets);
	void* bucket = (char*)h->buckets + hashCode*sizeof(vector*);

	//fprintf(stdout, "Hash: %i Letter: %c \n", hashCode, *(char*)elemAddr);
	for(int i=0; i<VectorLength(*(vector**)bucket); i++)
	{
		int c = h->comparefn(elemAddr, VectorNth(*(vector**)bucket, i));
		//fprintf(stdout, "%i", c);
		if(c == 0)
		{
			VectorReplace(*(vector**)bucket, elemAddr, i);
			return;
		}
		else if(c < 0)
		{
			VectorInsert(*(vector**)bucket, elemAddr, i);
			h->nElems++;
			return;
		}
	}
	VectorAppend(*(vector**)bucket, elemAddr);
	h->nElems++;
	//fprintf(stdout, "BucketL: %i Last: %c\n", VectorLength(*(vector**)bucket), *(char*)VectorNth(*(vector**)bucket, 0));
}

void *HashSetLookup(const hashset *h, const void *elemAddr)
{
	assert(elemAddr != NULL);
	int hashCode = h->hashfn(elemAddr, h->numBuckets);
	assert(hashCode >= 0 && hashCode < h->numBuckets);

	void* bucket = (char*)h->buckets + hashCode*sizeof(vector*);
	int found = VectorSearch(*(vector**)bucket, elemAddr, h->comparefn, 0, true);
	if(found == -1) return NULL;
	return VectorNth(*(vector**)bucket, found);
}

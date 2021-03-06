/*
 * Name:        svhtree.c
 * Description: Heap tree.
 * Author:      cosh.cage#hotmail.com
 * File ID:     0809171737E0601191737L00218
 *
 * The following text is copied from the source code of SQLite and padded
 * with a little bit addition to fit the goals for StoneValley project:
 *
 * The author disclaims copyright to this source code. In place of
 * a legal notice, here is a blessing:
 *   May you do good and not evil.
 *   May you find forgiveness for yourself and forgive others.
 *   May you share freely, never taking more than you give.
 *   Hope you never need to push yourself or other people too hard.
 */

#include <stdlib.h> /* Using function malloc, free. */
#include <string.h> /* Using function memcpy. */
#include "svtree.h"

/* Function name: treInitHeapA
 * Description:   Initialize an array implemented heap tree.
 * Parameters:
 *      pheap Pointer to a heap tree that you want to allocate.
 *        num Number of elements in the heap.
 *       size Size of each element in the heap.
 * Return value:  N/A.
 * Caution:       Address of pheap Must Be Allocated first.
 */
void treInitHeapA(P_HEAP_A pheap, size_t num, size_t size)
{
	pheap->irear = 0;
	strInitArrayZ(&pheap->hdarr, num, size);
}

/* Function name: treFreeHeapA
 * Description:   Deallocate a heap tree of which is allocated by function treInitHeapA.
 * Parameter:
 *     pheap Pointer to a heap that you want to deallocate.
 * Return value:  N/A.
 * Caution:       Address of heap Must Be Allocated first.
 */
void treFreeHeapA(P_HEAP_A pheap)
{
	strFreeArrayZ(&pheap->hdarr);
	pheap->irear = 0;
}

/* Function name: treCreateHeapA
 * Description:   Allocate a new array implemented heap dynamically.
 * Parameters:
 *        num Number of elements in the heap.
 *       size Size of each element.
 * Return value:  Pointer to the new allocated heap.
 */
P_HEAP_A treCreateHeapA(size_t num, size_t size)
{
	P_HEAP_A pheap = (P_HEAP_A) malloc(sizeof(HEAP_A));
	if (NULL == pheap)
		return NULL;
	pheap->irear = 0;
	strInitArrayZ(&pheap->hdarr, num, size);
	return pheap;
}

/* Function name: treDeleteHeapA
 * Description:   Delete a heap tree of which is allocated by function treCreateHeapA.
 * Parameter:
 *     pheap Pointer to a heap you want to allocate.
 * Return value:  N/A.
 * Caution:       Address of pheap Must Be Allocated first.
 */
void treDeleteHeapA(P_HEAP_A pheap)
{
	treFreeHeapA(pheap);
	free(pheap);
}

/* Function name: treIsEmptyHeapA_O
 * Description:   Make a judgement whether an array implemented heap is empty or not.
 * Parameter:
 *     pheap Pointer to the heap you want to check.
 * Return value:
 *         TRUE Heap is empty.
 *        FALSE Heap is NOT empty.
 * Tip:           A macro version of this function named treIsEmptyHeapA_M is available.
 */
BOOL treIsEmptyHeapA_O(P_HEAP_A pheap)
{
	return !pheap->irear;
}

/* Function name: treIsFullHeapA_O
 * Description:   Make a judgement for whether an array implemented heap is full or not.
 * Parameter:
 *     pstka Pointer to the heap you want to check.
 * Return value:
 *         TRUE Heap is full.
 *        FALSE Heap is starved. ;D
 * Tip:           A macro version of this function named treIsFullHeapA_M is available.
 */
BOOL treIsFullHeapA_O(P_HEAP_A pheap)
{
	return pheap->irear == strLevelArrayZ(&pheap->hdarr);
}

/* Function name: treInsertHeapA
 * Description:   Insert an element into an array implemented heap.
 * Parameters:
 *      pheap Pointer to the heap you want to operate with.
 *      pitem Pointer to an element you wanna insert into the heap.
 *      ptemp Pointer to a buffer that is used to swap data.
 *            The length of this buffer shall equal to parameter size.
 *       size Size of element.
 *     cbfcmp Pointer to a CBF_COMPARE callback function.
 *            Two parameters of this callback function may point to any element in the heap.
 *            Please refer to the prototype of callback function CBF_COMPARE in file svdef.h.
 *       bmax
 *            TRUE  Insert data into a max heap.
 *            FALSE Insert data into a min heap.
 *            (*) If a heap has been defined as a max heap in creation, do not assign FALSE before deletion, vice versa.
 * Return value:  N/A.
 * Caution:       pheap must be allocated first.
 *                ptemp cannot be written as NULL. This buffer is managed by caller function.
 * Tip:           Please use function treIsFullHeapA to check whether a heap is full before invoking this function.
 *                If the heap is full, do not use this function, otherwise it may produce an access violation error.
 */
void treInsertHeapA(P_HEAP_A pheap, const void * pitem, void * ptemp, size_t size, CBF_COMPARE cbfcmp, BOOL bmax)
{
	REGISTER PUCHAR px, py;
	REGISTER size_t i, j;
	REGISTER int r;
	j = pheap->irear++;
	/* Put new element onto the end of array. */
	memcpy(pheap->hdarr.pdata + j * size, pitem, size);
	while (j != 0)
	{
		/* Fetch parent node of new element. */
		i = (j - 1) >> 1;
		px = pheap->hdarr.pdata + i * size;
		py = pheap->hdarr.pdata + j * size;
		r = cbfcmp(py, px);
		if (bmax ? r > 0 : r < 0)
		{	/* Swap when j > i in a max heap. */
			svSwap(px, py, ptemp, size);
			j = i;
		}
		else /* j <= i in max heap. */
			break;
	}
}

/* Function name: treRemoveHeapA
 * Description:   Remove an element from an array implemented heap.
 * Parameters:
 *      pitem Pointer to an element you wanna remove.
 *      ptemp Pointer to a buffer that is used to swap data.
 *            The length of this buffer shall equal to parameter size.
 *       size Size of element.
 *      pheap Pointer to the heap you want to operate with.
 *     cbfcmp Pointer a CBF_COMPARE callback function.
 *            Two parameters of this callback function may point to any element in the heap.
 *            Please refer to the prototype of callback function CBF_COMPARE in file svdef.h.
 *       bmax
 *            TRUE  Remove data from a max heap.
 *            FALSE Remove data from a min heap.
 *            (*) If a heap has been defined as a min heap in creation, do not assign TRUE before deletion, vice versa.
 * Return value:  N/A.
 * Caution:       pheap must be allocated first.
 *                ptemp cannot be NULL. This buffer is managed by caller function.
 * Tip:           Please use function treIsEmptyHeapA to check whether a heap is empty before invoking this function.
 *                If the heap is empty, do not use this function, otherwise it may produce an access violation error.
 */
void treRemoveHeapA(void * pitem, void * ptemp, size_t size, P_HEAP_A pheap, CBF_COMPARE cbfcmp, BOOL bmax)
{
	REGISTER PUCHAR px, py;
	REGISTER size_t i, j;
	REGISTER int r;
	/* Remove the biggest one. */
	memcpy(pitem, pheap->hdarr.pdata, size);
	/* Put the last element onto the head of array and decrease the real size of the array. */
	memcpy(pheap->hdarr.pdata, pheap->hdarr.pdata + (--pheap->irear) * size, size);
	for (i = 0, j = (i << 1) + 2; i <= pheap->irear; j = (i << 1) + 2)
	{
		px = pheap->hdarr.pdata + i * size;
		py = pheap->hdarr.pdata + j * size;
		r = cbfcmp(px, py);
		if (bmax ? r < 0 : r > 0)
		{	/* Swap when i < j in a max heap. */
			svSwap(px, py, ptemp, size);
			i = j;
		}
		else /* i >= j in max heap. */
			break;
	}
}

/* Function name: trePeepHeapA
 * Description:   Have a peek at the first element of heap.
 * Parameters:
 *      pitem Pointer to the address of an element.
 *       size Size of element in the heap.
 *      pheap Pointer to the heap you want to operate with.
 * Return value:
 *         TRUE Peep succeeded.
 *        FALSE Peep failed.
 * Caution:       pheap must be allocated first.
 *                pitem must be a valid address that can hold an element.
 */
BOOL trePeepHeapA(void * pitem, size_t size, P_HEAP_A pheap)
{
	if (treIsEmptyHeapA(pheap))
		return FALSE; /* Heap is empty. */
	memcpy(pitem, pheap->hdarr.pdata, size);
	return TRUE;
}

#include "sorted-list.h"


NodePtr NodeCreate(void *data)
{
	NodePtr newNode = (NodePtr)malloc(sizeof(struct Node));
	newNode->data = data;
	newNode->next = NULL;
	newNode->refcnt = 0;
	newNode->isRemoved = 0;
	return newNode;
}

void NodeDestroy(NodePtr node, void (*df)(void *))
{
	df(node->data);
	free(node);
	node = NULL;
}

SortedListPtr SLCreate(CompareFuncT cf, CompareFuncT cf2, DestructFuncT df)
{
	if (cf == NULL || df == NULL) return NULL;
	SortedListPtr sl = (SortedListPtr)malloc(sizeof(struct SortedList));
	sl->front = NULL;
	sl->cf = cf;
	sl->cf2 = cf2;
	sl->df = df;
	return sl;
}

void SLDestroy(SortedListPtr list)
{
	NodePtr ptr = list->front;
	NodePtr prev = NULL;
	
	while (ptr != NULL)
	{
		prev = ptr;
		ptr = ptr->next;
		NodeDestroy(prev, list->df);			// destroy node and data.
	}
	if (prev != list->front)
		free(list);				// destroy list.
	list = NULL;
}

// following functions aren't used in indexer program.
int SLInsert(SortedListPtr list, void *newObj)
{
	if (newObj == NULL) return 0;
	NodePtr newNode = NodeCreate(newObj);
	
	if (list->front == NULL)	// empty list;
	{
		list->front = newNode;
		newNode->refcnt++;
		return 1;
	}

	NodePtr curr = list->front;
	NodePtr prev = NULL;
	int compare;

	while (curr != NULL)
	{
		compare = list->cf(newNode->data, curr->data);
		if (compare == 0) return 0;		// duplicates, failure.
		if (compare > 0)				// new data is greater.
		{
			if (curr == list->front)	// if it is added to the front.
			{
				newNode->next = list->front;
				list->front = newNode;
			}
			else
			{
				newNode->next = curr;	// added to the middle. 
				prev->next = newNode;
			}
			newNode->refcnt++;
			return 1;
		}
		prev = curr;
		curr = curr->next;
	}
	prev->next = newNode;				// added to the end of the list. 
	newNode->refcnt++;
	return 1;
}

int SLRemove(SortedListPtr list, void *newObj)
{
	if (list->front == NULL) return 0;	// empty list. 
	
	NodePtr curr = list->front;
	NodePtr prev = NULL;
	int compare;

	while (curr != NULL)				// searches for item.
	{
		compare = list->cf(newObj, curr->data);
		if (compare != 0)
		{
			if (compare > 0) return 0;	// not in list.
			prev = curr;
			curr = curr->next;
			continue;
		}
		if (curr == list->front)		// item is at the front.
			list->front = curr->next;
		else
			prev->next = curr->next;
		curr->refcnt--;
		if (curr->next != NULL)
			curr->next->refcnt++;
		curr->isRemoved = 1;
		if (curr->refcnt == 0)			// destroys node with no references.
		{ 
			NodeDestroy(curr, list->df);
			if (curr->next != NULL)
				curr->next->refcnt--;
		}
		return 1;
	}
	
	return 0;							// item not found in list. 
}

SortedListIteratorPtr SLCreateIterator(SortedListPtr list)
{
	if (list == NULL) return NULL;
	SortedListIteratorPtr iter = (SortedListIteratorPtr)malloc(sizeof(struct SortedListIterator));
	iter->curr = list->front;
	if (iter->curr != NULL)
		iter->curr->refcnt++;
	iter->df = list->df;
	return iter;
}

void SLDestroyIterator(SortedListIteratorPtr iter)
{
	iter->curr = NULL;
	free(iter);
	iter = NULL;
}

void *SLNextItem(SortedListIteratorPtr iter)
{
	if (iter->curr == NULL) return NULL;
	void *data = iter->curr->data;			// store data
	NodePtr prev = iter->curr;				// update iter ptr
	iter->curr = iter->curr->next;
	prev->refcnt--;
	if (iter->curr != NULL)
		iter->curr->refcnt++;
	if (prev->isRemoved)					// recursively call SLNextItem until
	{										// reaching an item not removed.
		if (prev->refcnt == 0)				// frees node with no references. 
		{
			NodeDestroy(prev, iter->df);
			if (iter->curr != NULL)
				iter->curr->refcnt--;
		}
		return SLNextItem(iter);
	}
	return data;
	
}

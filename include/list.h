// list.h

#ifndef _LIST_H
#define _LIST_H

#define ListPush(List, Count, Element) do { \
	if (List == NULL) { \
    List = ListInit(sizeof(Element), 1); List[0] = Element; Count = 1; break; \
  } \
	void* NewList = M_Realloc(List, Count * sizeof(*List), (1 + Count) * (sizeof(Element))); \
	if (NewList) { \
		List = NewList; \
		List[(Count)++] = Element; \
	} \
} while (0); \

#define ListRealloc(List, Count, NewSize) do { \
  if (List == NULL) break; \
  if (NewSize == 0) { ListFree(List, Count); break; } \
	void* NewList = M_Realloc(List, Count * sizeof(*List), (NewSize) * (sizeof(*List))); \
  List = NewList; \
  Count = NewSize; \
} while(0); \

#define ListShrink(List, Count, Num) { \
  if ((Count - Num) >= 0) { \
	  ListRealloc(List, Count, Count - Num); \
  } \
}
 
#define ListAssign(List, Count, Index, Element) { \
	Assert(List != NULL); \
	if (Index < Count) { \
		List[Index] = Element; \
	} else { Assert(0); } \
} \

#define ListFree(List, Count) { \
	if ((List) != NULL) { \
		M_Free(List, Count * sizeof(*List)); \
		Count = 0; \
    List = NULL; \
	}\
}

// Size of type
// Count of elements to allocate
void* ListInit(const u32 Size, const u32 Count);

#endif

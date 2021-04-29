// list.c

void BufferPush(buffer* Buffer, char Element) {
  ListPush(Buffer->Data, Buffer->Count, Element);
}

void BufferFree(buffer* Buffer) {
  ListFree(Buffer->Data, Buffer->Count);
}

void* ListInit(const u32 Size, const u32 Count) {
	void* List = M_Calloc(Size, Count);
	if (!List) {
		fprintf(stderr, "Allocation failed\n");
		return NULL;
	}
	return List;
}

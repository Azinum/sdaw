// list.c

void* ListInit(const u32 Size, const u32 Count) {
	void* List = M_Calloc(Size, Count);
	if (!List) {
		fprintf(stderr, "Allocation failed\n");
		return NULL;
	}
	return List;
}

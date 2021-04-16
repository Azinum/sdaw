// common.c

char* FetchExtension(const char* Path) {
  char* Ext = strrchr(Path, '.');
  return Ext;
}

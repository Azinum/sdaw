// module.c

#include <dlfcn.h>

void* ModuleOpen(const char* Path) {
  const char* FullPath = DataPathConcat(Path);
  return dlopen(FullPath, RTLD_LAZY);
}

void* ModuleSymbol(void* Handle, const char* SymbolName) {
  return dlsym(Handle, SymbolName);
}

void ModuleClose(void* Handle) {
  if (Handle) {
    dlclose(Handle);
  }
}

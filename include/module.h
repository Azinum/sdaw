// module.h

#ifndef _MODULE_H
#define _MODULE_H

void* ModuleOpen(const char* Path);

void* ModuleSymbol(void* Handle, const char* SymbolName);

void ModuleClose(void* Handle);

#endif

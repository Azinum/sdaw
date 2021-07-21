// str.h

#ifndef _STR_H
#define _STR_H

void StringInit(string* String);

void StringMinimize(string* String);

void StringPush(string* String, char Element);

u32 StringPushPrintf(string* String, const char* Format, ...);

u32 StringPushvPrintf(string* String, const char* Format, va_list Args);

u32 StringPrintf(string* String, const char* Format, ...);

u32 StringvPrintf(string* String, const char* Format, va_list Args);

void StringFree(string* String);

void BufferInit(buffer* Buffer);

void BufferPush(buffer* Buffer, char Element);

void BufferFree(buffer* Buffer);

#endif

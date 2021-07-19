// str.h

#ifndef _STR_H
#define _STR_H

void StringInit(string* String);

void StringMinimize(string* String);

void StringPush(string* String, char Element);

void StringPushPrintf(string* String, const char* Format, ...);

void StringPrintf(string* String, const char* Format, ...);

void StringFree(string* String);

void BufferInit(buffer* Buffer);

void BufferPush(buffer* Buffer, char Element);

void BufferFree(buffer* Buffer);

#endif

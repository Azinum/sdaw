// hash.h

#ifndef _HASH_H
#define _HASH_H

typedef i32 ht_value;
#define HTABLE_KEY_SIZE 32 - sizeof(ht_value)
typedef u64 ht_key;

struct item;

typedef struct {
  struct item* Items;
  u32 Count;	// Count of used slots
  u32 Size;	// Total size of the hash Table
} hash_table;

hash_table HtCreate(u32 Size);

hash_table HtCreateEmpty();

i32 HtIsEmpty(const hash_table* Table);

u32 HtInsertElement(hash_table* Table, const ht_key Key, const ht_value Value);

const ht_value* HtLookup(const hash_table* Table, const ht_key Key);

const ht_value* HtLookupByIndex(const hash_table* Table, const u32 Index);

const ht_key* HtLookupKey(const hash_table* Table, const u32 Index);

i32 HtElementExists(const hash_table* Table, const ht_key Key);

void HtRemoveElement(hash_table* Table, const ht_key Key);

u32 HtGetSize(const hash_table* Table);

u32 HtNumElements(const hash_table* Table);

void HtFree(hash_table* Table);

#endif

